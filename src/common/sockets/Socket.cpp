/*
 *  Copyright (c) 2000 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/common/sockets/Socket.cpp
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of UNIX client socket class
 */

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <containers/BinaryQueue.h>
#include <containers/RawBuffer.h>
#include <exceptions/InitException.h>
#include <exceptions/ServerConnectionErrorException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>

#include "Socket.h"

namespace Cynara {

Socket::Socket(const std::string &socketPath, int timeoutMiliseconds) : m_sock(-1),
    m_socketPath(socketPath), m_pollTimeout(timeoutMiliseconds) {
}

Socket::~Socket() {
    close();
}

void Socket::close(void) noexcept {
    if (m_sock > -1)
        ::close(m_sock);
    m_sock = -1;
}

bool Socket::waitForSocket(int event) {
    int ret;
    pollfd desc[1];
    desc[0].fd = m_sock;
    desc[0].events = event;

    ret = TEMP_FAILURE_RETRY(poll(desc, 1, m_pollTimeout));

    if (ret == -1) {
        int err = errno;
        LOGE("'poll' function error [%d] : <%s>", err, strerror(err));
        close();
        throw UnexpectedErrorException(err, strerror(err));
    } else if (ret == 0) {
        LOGD("Poll timeout");
    }

    return (ret == 1);
}

int Socket::getSocketError(void) {
    int err = 0;
    socklen_t len = sizeof(err);
    int ret = getsockopt(m_sock, SOL_SOCKET, SO_ERROR, &err, &len);
    if (ret < 0) {
        int err = errno;
        close();
        LOGE("'getsockopt' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }
    return err;
}

bool Socket::isConnected(void) {
    if (m_sock < 0)
        return false;

    if (getSocketError() != 0) {
        close();
        return false;
    }

    return true;
}

bool Socket::connect(void) {
    sockaddr_un clientAddr;
    int flags;

    if (isConnected())
        return true;

    close();

    m_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_sock < 0) {
        int err = errno;
        LOGE("'socket' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }

    if ((flags = fcntl(m_sock, F_GETFL, 0)) < 0 ||
        fcntl(m_sock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        int err = errno;
        close();
        LOGE("'fcntl' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }

    memset(&clientAddr, 0, sizeof(clientAddr));

    clientAddr.sun_family = AF_UNIX;

    if (m_socketPath.length() >= sizeof(clientAddr.sun_path)) {
        close();
        LOGE("Error: socket path <%s> is too long [%zu]. Max len is [%zu]", m_socketPath.c_str(),
             m_socketPath.length(), sizeof(clientAddr.sun_path));
        throw InitException();
    }

    strcpy(clientAddr.sun_path, m_socketPath.c_str());

    LOGD("ClientAddr.sun_path <%s>", clientAddr.sun_path);

    int retval = TEMP_FAILURE_RETRY(::connect(m_sock, (struct sockaddr*)&clientAddr,
                                            SUN_LEN(&clientAddr)));
    if (retval == -1) {
        int err = errno;
        if (err == EINPROGRESS) {
            if (!waitForSocket(POLLOUT)) {
                return false;
            }
            err = getSocketError();
        }
        if (err == ECONNREFUSED) {
            //no one is listening
            return false;
        }
        close();
        LOGE("'connect' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }

    return isConnected();
}

bool Socket::sendToServer(BinaryQueue &queue) {
    bool retry = false;

    RawBuffer buffer(queue.size());
    queue.flattenConsume(buffer.data(), queue.size());

    do {
        if (!connect()) {
            LOGE("Error connecting to socket");
            throw ServerConnectionErrorException();
        }

        retry = false;
        ssize_t done = 0;
        while ((buffer.size() - done) > 0) {
            if (! waitForSocket(POLLOUT)) {
                LOGE("Error in poll(POLLOUT)");
                throw ServerConnectionErrorException();
            }
            ssize_t t = TEMP_FAILURE_RETRY(send(m_sock, buffer.data() + done,
                                           buffer.size() - done, MSG_NOSIGNAL));
            if (t == -1) {
                int err = errno;
                if (err == EPIPE) {
                    close();
                    LOGN("Connection closed by server. Retrying to connect.");
                    retry = true;
                    break;
                }
                close();
                LOGE("'write' function error [%d] : <%s>", err, strerror(err));
                throw UnexpectedErrorException(err, strerror(err));
            }
            done += t;
        }
    } while (retry);

    return true;
}

bool Socket::waitAndReceiveFromServer(BinaryQueue &queue)
{
    if (!waitForSocket(POLLIN)) {
        LOGE("Error in poll(POLLIN)");
        throw ServerConnectionErrorException();
    }

    RawBuffer readBuffer(BUFSIZ);
    ssize_t size = TEMP_FAILURE_RETRY(read(m_sock, readBuffer.data(), BUFSIZ));

    if (size == -1) {
        int err = errno;
        LOGE("'read' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }

    if (size == 0) {
        LOGW("read return 0 / Connection closed by server.");
        return false;
    }
    queue.appendCopy(readBuffer.data(), size);

    return true;
}

bool Socket::receiveFromServer(BinaryQueue &queue)
{
    RawBuffer readBuffer(BUFSIZ);
    ssize_t size = TEMP_FAILURE_RETRY(read(m_sock, readBuffer.data(), BUFSIZ));

    if (size == -1) {
        int err = errno;
        if (err == EAGAIN) {
            LOGD("is connected, but no data available");
            return true;
        }
        LOGE("'read' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }

    if (size == 0) {
        LOGW("read return 0 / Connection closed by server.");
        return false;
    }
    queue.appendCopy(readBuffer.data(), size);

    return true;
}

} // namespace Cynara
