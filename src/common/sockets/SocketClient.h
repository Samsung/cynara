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
 * @file        SocketClient.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of cynara's socket client
 */

#ifndef SRC_COMMON_SOCKETS_SOCKETCLIENT_H_
#define SRC_COMMON_SOCKETS_SOCKETCLIENT_H_

#include <memory>

#include <containers/BinaryQueue.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <response/pointers.h>
#include <response/ResponseTaker.h>
#include <sockets/Socket.h>

namespace Cynara {

class SocketClient;
typedef std::shared_ptr<SocketClient> SocketClientPtr;

class SocketClient {
private:
    Socket m_socket;
    ProtocolPtr m_protocol;
    BinaryQueue m_readQueue;
    BinaryQueue m_writeQueue;

public:
    SocketClient(const std::string &socketPath, ProtocolPtr protocol);
    virtual ~SocketClient() {};

    //returns pointer to response
    //        or nullptr when connection to cynara service is lost
    ResponsePtr askCynaraServer(RequestPtr request);

    bool isConnected(void);
};

} // namespace Cynara

#endif /* SRC_COMMON_SOCKETS_SOCKETCLIENT_H_ */
