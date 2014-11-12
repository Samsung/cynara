/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/client-async/sockets/SocketClientAsync.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains declaration of cynara's socket asynchronous
                client
 */

#ifndef SRC_CLIENT_ASYNC_SOCKETS_SOCKETCLIENTASYNC_H_
#define SRC_CLIENT_ASYNC_SOCKETS_SOCKETCLIENTASYNC_H_

#include <memory>
#include <string>

#include <containers/BinaryQueue.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <response/pointers.h>
#include <sockets/Socket.h>

namespace Cynara {

class SocketClientAsync;
typedef std::shared_ptr<SocketClientAsync> SocketClientAsyncPtr;

class SocketClientAsync {
public:
    SocketClientAsync(const std::string &socketPath, ProtocolPtr protocol);
    ~SocketClientAsync() {};

    Socket::ConnectionStatus connect(void);
    Socket::ConnectionStatus completeConnection(void);
    int getSockFd(void);
    bool isConnected(void);
    void appendRequest(RequestPtr request);
    bool isDataToSend(void);
    Socket::SendStatus sendToCynara(void);
    bool receiveFromCynara(void);
    ResponsePtr getResponse(void);

private:
    Socket m_socket;
    ProtocolPtr m_protocol;
    BinaryQueuePtr m_readQueue;
    BinaryQueuePtr m_writeQueue;
};

} // namespace Cynara

#endif /* SRC_CLIENT_ASYNC_SOCKETS_SOCKETCLIENTASYNC_H_ */
