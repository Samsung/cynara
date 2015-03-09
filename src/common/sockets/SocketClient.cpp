/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/sockets/SocketClient.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of cynara's socket client
 */

#include <log/log.h>
#include <request/Request.h>
#include <request/RequestContext.h>
#include <sockets/Socket.h>

#include "SocketClient.h"

namespace Cynara {

SocketClient::SocketClient(const std::string &socketPath, ProtocolPtr protocol)
        : m_socket(socketPath), m_protocol(protocol) {
    m_writeQueue = std::make_shared<BinaryQueue>();
    m_readQueue = std::make_shared<BinaryQueue>();
}

bool SocketClient::connect(void) {
    switch (m_socket.connect()) {
        case Socket::ConnectionStatus::CONNECTION_FAILED:
            LOGW("Error connecting to Cynara. Service not available.");
            return false;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            if (m_socket.completeConnection() == Socket::ConnectionStatus::CONNECTION_FAILED) {
                LOGW("Error connecting to Cynara. Service not available.");
                return false;
            }
        default:
            return true;
    }
}

bool SocketClient::isConnected(void) {
    return m_socket.isConnected();
}

ResponsePtr SocketClient::askCynaraServer(const Request &request) {
    //pass request to protocol
    RequestContext context(ResponseTakerPtr(), m_writeQueue);
    request.execute(*m_protocol, context);

    //send request to cynara
    if (m_socket.sendToServer(*m_writeQueue) == Socket::SendStatus::CONNECTION_LOST) {
        LOGW("Disconnected while sending request to Cynara.");
        return nullptr;
    }

    // receive response from cynara
    while (true) {
        if (!m_socket.receiveFromServer(*m_readQueue)) {
            LOGW("Disconnected while receiving response from Cynara.");
            return nullptr;
        }
        ResponsePtr response = m_protocol->extractResponseFromBuffer(m_readQueue);
        if (response) {
            return response;
        }
    }
}

} // namespace Cynara
