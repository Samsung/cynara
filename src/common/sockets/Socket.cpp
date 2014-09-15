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

#include <exceptions/InitException.h>
#include <exceptions/NoMemoryException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>

#include "Socket.h"

namespace Cynara {

Socket::Socket(const std::string &socketPath, int timeoutMiliseconds)
    : m_sock(-1), m_connectionInProgress(false), m_socketPath(socketPath),
      m_pollTimeout(timeoutMiliseconds), m_sendBufferPos(0), m_sendBufferEnd(0) {
}

Socket::~Socket() {
    close();
}

void Socket::close(void) {
    if (m_sock > -1)
        ::close(m_sock);
    m_sock = -1;
    m_sendBufferPos = 0;
    m_sendBufferEnd = 0;
    m_sendQueue.clear();
}

bool Socket::waitForSocket(int event) {
    int ret;
    pollfd desc[1];
    desc[0].fd = m_sock;
    desc[0].events = event;

    if (event != POLLHUP)
        ret = TEMP_FAILURE_RETRY(poll(desc, 1, m_pollTimeout));
    else
        ret = TEMP_FAILURE_RETRY(poll(desc, 1, 0));

    if (ret == -1) {
        int err = errno;
        LOGE("'poll' function error [%d] : <%s>", err, strerror(err));
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
        LOGE("'getsockopt' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }
    return err;
}

bool Socket::isConnected(void) {
    if (m_connectionInProgress)
        return true;

    if (m_sock < 0)
        return false;

    if (getSocketError() != 0)
        return false;

    return !waitForSocket(POLLHUP);
}

void Socket::createSocket(void) {
    int flags;

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
}

Socket::ConnectionStatus Socket::connectSocket(void) {
    sockaddr_un clientAddr;

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
        switch (err) {
            case EINPROGRESS:
                m_connectionInProgress = true;
                return ConnectionStatus::CONNECTION_IN_PROGRESS;
            case ECONNREFUSED:
                //no one is listening
                return ConnectionStatus::CONNECTION_FAILED;
            default:
                close();
                LOGE("'connect' function error [%d] : <%s>", err, strerror(err));
                throw UnexpectedErrorException(err, strerror(err));
        }
    }
    return ConnectionStatus::CONNECTION_SUCCEEDED;
}

Socket::SendStatus Socket::sendBuffer(void) {
    while (m_sendBufferEnd != m_sendBufferPos) {
        if (!waitForSocket(POLLOUT)) {
            LOGD("No POLLOUT event");
            return SendStatus::PARTIAL_DATA_SENT;
        }

        ssize_t t = TEMP_FAILURE_RETRY(send(m_sock, m_sendBuffer.data() + m_sendBufferPos,
                                            m_sendBufferEnd - m_sendBufferPos, MSG_NOSIGNAL));
        if (t == -1) {
            int err = errno;
            switch (err) {
                case EAGAIN:
#if EWOULDBLOCK != EAGAIN
                case EWOULDBLOCK:
#endif
                    continue;
                case ENOMEM:
                    throw NoMemoryException("'send' function failed due to ENOMEM");
                case EPIPE:
                    LOGN("Connection closed by server");
                    return SendStatus::CONNECTION_LOST;
                default:
                    LOGE("'send' function error [%d] : <%s>", err, strerror(err));
                    throw UnexpectedErrorException(err, strerror(err));
            }
        }
        m_sendBufferPos += static_cast<size_t>(t);
    }
    return SendStatus::ALL_DATA_SENT;
}

Socket::ConnectionStatus Socket::connect(void) {
    close();

    createSocket();

    ConnectionStatus status = connectSocket();
    if (status != ConnectionStatus::CONNECTION_SUCCEEDED)
        return status;

    return isConnected() ? ConnectionStatus::CONNECTION_SUCCEEDED
                         : ConnectionStatus::CONNECTION_FAILED;
}

Socket::ConnectionStatus Socket::completeConnection(void) {
    if (!m_connectionInProgress)
        return ConnectionStatus::ALREADY_CONNECTED;

    if (!waitForSocket(POLLOUT))
        return ConnectionStatus::CONNECTION_IN_PROGRESS;

    m_connectionInProgress = false;
    return isConnected() ? ConnectionStatus::CONNECTION_SUCCEEDED
                         : ConnectionStatus::CONNECTION_FAILED;
}

int Socket::getSockFd(void) {
    return m_sock;
}

bool Socket::isDataToSend(void) {
    return !m_sendQueue.empty() || m_sendBufferEnd != 0;
}

Socket::SendStatus Socket::sendToServer(BinaryQueue &queue) {
    m_sendQueue.appendMoveFrom(queue);

    SendStatus status = sendBuffer();
    if (status != SendStatus::ALL_DATA_SENT)
        return status;

    if (m_sendQueue.size() > m_sendBuffer.size())
        m_sendBuffer.resize(m_sendQueue.size());

    m_sendBufferEnd = m_sendQueue.size();
    m_sendBufferPos = 0;

    m_sendQueue.flattenConsume(m_sendBuffer.data(), m_sendQueue.size());

    return sendBuffer();
}

bool Socket::receiveFromServer(BinaryQueue &queue) {
    if (!waitForSocket(POLLIN)) {
        LOGD("No POLLIN event");
        return true;
    }

    RawBuffer buffer(BUFSIZ);
    ssize_t size = 0;
    while (true) {
        size = TEMP_FAILURE_RETRY(read(m_sock, buffer.data(), BUFSIZ));
        if (size == 0) {
            LOGW("read return 0 / Connection closed by server.");
            return false;
        }

        if (size == -1) {
            int err = errno;
            switch (err) {
                case EAGAIN:
#if EWOULDBLOCK != EAGAIN
                case EWOULDBLOCK:
#endif
                    return true;
                default:
                    LOGE("'read' function error [%d] : <%s>", err, strerror(err));
                    throw UnexpectedErrorException(err, strerror(err));
            }
        }

        queue.appendCopy(buffer.data(), static_cast<size_t>(size));
    }
}

} // namespace Cynara
