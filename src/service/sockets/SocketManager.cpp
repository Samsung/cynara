/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        src/service/sockets/SocketManager.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements socket layer manager for cynara
 */

#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <signal.h>
#include <sys/select.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <systemd/sd-daemon.h>

#include <log/log.h>
#include <common.h>
#include <config/PathConfig.h>
#include <exceptions/DescriptorNotExistsException.h>
#include <exceptions/InitException.h>
#include <exceptions/UnexpectedErrorException.h>

#include <logic/Logic.h>
#include <main/Cynara.h>
#include <protocol/ProtocolAdmin.h>
#include <protocol/ProtocolAgent.h>
#include <protocol/ProtocolClient.h>
#include <protocol/ProtocolSignal.h>
#include <request/pointers.h>
#include <request/RequestContext.h>
#include <stdexcept>

#include "SocketManager.h"

namespace Cynara {

SocketManager::SocketManager() : m_working(false), m_maxDesc(-1) {
    FD_ZERO(&m_readSet);
    FD_ZERO(&m_writeSet);
}

SocketManager::~SocketManager() {
}

void SocketManager::run(void) {
    init();
    mainLoop();
}

void SocketManager::init(void) {
    LOGI("SocketManger init start");
    const mode_t clientSocketUMask(0);
    const mode_t adminSocketUMask(0077);
    const mode_t agentSocketUMask(0);

    createDomainSocket(std::make_shared<ProtocolClient>(), PathConfig::SocketPath::client,
                       clientSocketUMask, true);
    createDomainSocket(std::make_shared<ProtocolAdmin>(), PathConfig::SocketPath::admin,
                       adminSocketUMask, false);
    createDomainSocket(std::make_shared<ProtocolAgent>(), PathConfig::SocketPath::agent,
                       agentSocketUMask, false);
    createSignalSocket(std::make_shared<ProtocolSignal>());
    LOGI("SocketManger init done");
}

void SocketManager::mainLoop(void) {
    LOGI("SocketManger mainLoop start");
    m_working  = true;
    while (m_working) {
        fd_set readSet = m_readSet;
        fd_set writeSet = m_writeSet;

        int ret = select(m_maxDesc + 1, &readSet, &writeSet, nullptr, nullptr);

        if (ret < 0) {
            switch (errno) {
            case EINTR:
                continue;
            default:
                int err = errno;
                throw UnexpectedErrorException(err, strerror(err));
            }
        } else if (ret > 0) {
            for (int i = 0; i < m_maxDesc + 1 && ret; ++i) {
                if (FD_ISSET(i, &readSet)) {
                    readyForRead(i);
                    --ret;
                }
                if (FD_ISSET(i, &writeSet)) {
                    readyForWrite(i);
                    --ret;
                }
            }

            for (int i = 0; i < m_maxDesc + 1; ++i) {
                if (m_fds[i].isUsed() && m_fds[i].hasDataToWrite())
                    addWriteSocket(i);
            }
        }
    }
    LOGI("SocketManger mainLoop done");
}

void SocketManager::mainLoopStop(void) {
    m_working = false;
}

void SocketManager::readyForRead(int fd) {
    LOGD("SocketManger readyForRead on fd [%d] start", fd);
    auto &desc = m_fds[fd];
    if (desc.isListen()) {
        readyForAccept(fd);
        return;
    }

    RawBuffer readBuffer(DEFAULT_BUFFER_SIZE);
    ssize_t size = read(fd, readBuffer.data(), DEFAULT_BUFFER_SIZE);

    if (size > 0) {
        LOGD("read [%zd] bytes", size);
        readBuffer.resize(size);
        if (handleRead(fd, readBuffer)) {
            LOGD("SocketManger readyForRead on fd [%d] successfully done", fd);
            return;
        }
        LOGI("interpreting buffer read from [%d] failed", fd);
    } else if (size < 0) {
        int err = errno;
        switch (err) {
            case EAGAIN:
#if EWOULDBLOCK != EAGAIN
            case EWOULDBLOCK:
#endif
            case EINTR:
                return;
            default:
                LOGW("While reading from [%d] socket, error [%d]:<%s>",
                     fd, err, strerror(err));
        }
    } else {
        LOGN("Socket [%d] closed on other end", fd);
    }
    closeSocket(fd);
    LOGD("SocketManger readyForRead on fd [%d] done", fd);
}

void SocketManager::readyForWrite(int fd) {
    LOGD("SocketManger readyForWrite on fd [%d] start", fd);
    auto &desc = m_fds[fd];
    auto &buffer = desc.prepareWriteBuffer();
    size_t size = buffer.size();
    ssize_t result = write(fd, buffer.data(), size);
    if (result == -1) {
        int err = errno;
        switch (err) {
        case EAGAIN:
        case EINTR:
            // select will trigger write once again, nothing to do
            break;
        case EPIPE:
        default:
            LOGD("Error during write to fd [%d]:<%s> ", fd, strerror(err));
            closeSocket(fd);
            break;
        }
        return; // We do not want to propagate error to next layer
    }

    LOGD("written [%zd] bytes", result);
    buffer.erase(buffer.begin(), buffer.begin() + result);

    if (buffer.empty())
        removeWriteSocket(fd);
    LOGD("SocketManger readyForWrite on fd [%d] done", fd);
}

void SocketManager::readyForAccept(int fd) {
    LOGD("SocketManger readyForAccept on fd [%d] start", fd);
    struct sockaddr_un clientAddr;
    unsigned int clientLen = sizeof(clientAddr);
    int clientFd = accept4(fd, (struct sockaddr*) &clientAddr, &clientLen, SOCK_NONBLOCK);
    if (clientFd == -1) {
        int err = errno;
        LOGW("Error in accept on socket [%d]: <%s>", fd, strerror(err));
        return;
    }
    LOGD("Accept on sock [%d]. New client socket opened [%d]", fd, clientFd);

    auto &desc = createDescriptor(clientFd, m_fds[fd].isClient());
    desc.setListen(false);
    desc.setProtocol(m_fds[fd].protocol()->clone());
    addReadSocket(clientFd);
    LOGD("SocketManger readyForAccept on fd [%d] done", fd);
}

void SocketManager::closeSocket(int fd) {
    LOGD("SocketManger closeSocket fd [%d] start", fd);
    Descriptor &desc = m_fds[fd];
    requestTaker()->contextClosed(RequestContext(nullptr, desc.writeQueue()));
    removeReadSocket(fd);
    removeWriteSocket(fd);
    desc.clear();
    close(fd);
    LOGD("SocketManger closeSocket fd [%d] done", fd);
}

bool SocketManager::handleRead(int fd, const RawBuffer &readbuffer) {
    LOGD("SocketManger handleRead on fd [%d] start", fd);
    auto &desc = m_fds[fd];
    desc.pushReadBuffer(readbuffer);

    try {
        while(true) {
            //try extract request from binary data received on socket
            auto req = desc.extractRequest();
            if (!req)   // not enough data to build request yet
                break;
            LOGD("request extracted");

            //build context
            RequestContext context(desc.responseTaker(), desc.writeQueue());
            //pass request to request taker
            req->execute(*requestTaker(), context);
        }
    } catch (const Exception &ex) {
        LOGE("Error handling request <%s>. Closing socket", ex.what());
        return false;
    }
    LOGD("SocketManger handleRead on fd [%d] done", fd);
    return true;
}

void SocketManager::createDomainSocket(ProtocolPtr protocol, const std::string &path, mode_t mask,
                                       bool client) {
    int fd = getSocketFromSystemD(path);
    if (fd == -1)
        fd = createDomainSocketHelp(path, mask);

    auto &desc = createDescriptor(fd, client);
    desc.setListen(true);
    desc.setProtocol(protocol);
    addReadSocket(fd);

    LOGD("Domain socket: [%d] added.", fd);
}

int SocketManager::createDomainSocketHelp(const std::string &path, mode_t mask) {
    int fd;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        int err = errno;
        LOGE("Error during UNIX socket creation: <%s>",  strerror(err));
        throw InitException();
    }

    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        flags = 0;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        int err = errno;
        close(fd);
        LOGE("Error setting \"O_NONBLOCK\" on descriptor [%d] with fcntl: <%s>",
             fd, strerror(err));
        throw InitException();
    }

    sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sun_family = AF_UNIX;
    if (path.length() > sizeof(serverAddress.sun_path)) {
        LOGE("Path for unix domain socket <%s> is [%zu] bytes long, while it should be maximum "
             "[%zu] bytes long", path.c_str(), path.length(), sizeof(serverAddress));
        throw InitException();
    }
    strcpy(serverAddress.sun_path, path.c_str());
    unlink(serverAddress.sun_path);

    mode_t originalUmask;
    originalUmask = umask(mask);

    if (bind(fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        int err = errno;
        close(fd);
        LOGE("Error in bind socket descriptor [%d] to path <%s>: <%s>",
             fd, path.c_str(), strerror(err));
        throw InitException();
    }

    umask(originalUmask);

    if (listen(fd, 5) == -1) {
        int err = errno;
        close(fd);
        LOGE("Error setting listen on file descriptor [%d], path <%s>: <%s>",
             fd, path.c_str(), strerror(err));
        throw InitException();
    }

    return fd;
}

int SocketManager::getSocketFromSystemD(const std::string &path) {
    int n = sd_listen_fds(0);
    LOGI("sd_listen_fds returns: [%d]", n);
    if (n < 0) {
        LOGE("Error in sd_listend_fds");
        throw InitException();
    }

    for (int fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; ++fd) {
        if (sd_is_socket_unix(fd, SOCK_STREAM, 1, path.c_str(), 0) > 0) {
            LOGI("Useable socket <%s> was passed by SystemD under descriptor [%d]",
                    path.c_str(), fd);
            return fd;
        }
    }
    LOGI("No useable sockets were passed by systemd.");
    return -1;
}

void SocketManager::createSignalSocket(ProtocolPtr protocol) {
    sigset_t mask;

    // Maybe someone will find useful some kind of registering signals with callbacks
    // but for now I'm making it as simple as possible.
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM); // systemd terminates service sending this signal

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        LOGE("sigprocmask failed: <%s>", strerror(errno));
        return;
    }

    int fd = signalfd(-1, &mask, SFD_NONBLOCK);
    if (fd < 0) {
        LOGE("Creating signal file descriptor failed: <%s>", strerror(errno));
        return;
    }

    auto &desc = createDescriptor(fd, false);
    desc.setListen(false);
    desc.setProtocol(protocol);
    addReadSocket(fd);

    LOGD("Signal socket: [%d] added.", fd);
}

Descriptor &SocketManager::createDescriptor(int fd, bool client) {
    if (fd > m_maxDesc) {
        m_maxDesc = fd;
        if (fd >= static_cast<int>(m_fds.size()))
            m_fds.resize(fd + 20);
    }
    auto &desc = m_fds[fd];
    desc.setUsed(true);
    desc.setClient(client);
    return desc;
}

void SocketManager::addReadSocket(int fd) {
    FD_SET(fd, &m_readSet);
}

void SocketManager::removeReadSocket(int fd) {
    FD_CLR(fd, &m_readSet);
}

void SocketManager::addWriteSocket(int fd) {
    FD_SET(fd, &m_writeSet);
}

void SocketManager::removeWriteSocket(int fd) {
    FD_CLR(fd, &m_writeSet);
}

RequestTakerPtr SocketManager::requestTaker(void) {
    return std::static_pointer_cast<RequestTaker>(m_logic);
}

void SocketManager::disconnectAllClients(void) {
    for(int i = 0; i <= m_maxDesc; ++i) {
        auto &desc = m_fds[i];
        if(desc.isUsed() && desc.isClient() && !desc.isListen())
            closeSocket(i);
    }
}

} // namespace Cynara
