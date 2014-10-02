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
 * @file        src/agent/socket/AgentSocketClient.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of agent socket client
 */

#ifndef SRC_AGENT_SOCKET_AGENTSOCKETCLIENT_H_
#define SRC_AGENT_SOCKET_AGENTSOCKETCLIENT_H_

#include <memory>
#include <string>

#include <containers/BinaryQueue.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <response/pointers.h>
#include <sockets/Socket.h>

namespace Cynara {

class AgentSocketClient;
typedef std::shared_ptr<AgentSocketClient> AgentSocketPtr;

typedef enum {
    SS_DISCONNECTED,
    SS_CONNECTED,
    SS_RECONNECTED,
} AgentSocketState;

class AgentSocketClient {
public:
    AgentSocketClient(const std::string &socketPath, ProtocolPtr protocol);
    virtual ~AgentSocketClient() {};

    bool isConnected(void);
    AgentSocketState connect(void);

    ResponsePtr receiveResponseFromServer(void);
    bool sendDataToServer(BinaryQueue &data);
    ResponsePtr askCynaraServer(RequestPtr request);

private:
    Socket m_socket;
    ProtocolPtr m_protocol;
    BinaryQueuePtr m_readQueue;
    BinaryQueuePtr m_writeQueue;

    void resetState(void);
};

} // namespace Cynara

#endif /* SRC_AGENT_SOCKET_AGENTSOCKETCLIENT_H_ */
