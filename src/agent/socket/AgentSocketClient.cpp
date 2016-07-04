/*
 *  Copyright (c) 2014-2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/agent/socket/AgentSocket.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of agent socket client
 */

#include <cstring>
#include <memory>
#include <poll.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

#include <log/log.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <request/Request.h>
#include <request/RequestContext.h>
#include <response/pointers.h>
#include <sockets/Socket.h>

#include "AgentSocketClient.h"

namespace Cynara {

AgentSocketClient::AgentSocketClient(const std::string &socketPath, ProtocolPtr protocol)
        : m_socket(socketPath), m_protocol(protocol), m_notifyFd(-1) {
    m_writeQueue = std::make_shared<BinaryQueue>();
    m_readQueue = std::make_shared<BinaryQueue>();
}

void AgentSocketClient::setNotifyFd(int notifyFd) {
    m_notifyFd = notifyFd;
}

ResponsePtr AgentSocketClient::askCynaraServer(const Request &request) {
    //pass request to protocol
    RequestContext context(ResponseTakerPtr(), m_writeQueue);
    request.execute(*m_protocol, context);

    //send request to cynara
    if (!sendDataToServer(*m_writeQueue)) {
        return nullptr;
    }

    // receive response from cynara
    return receiveResponseFromServer();
}

AgentSocketState AgentSocketClient::waitForEvent(void) {
    int cynaraFd = m_socket.getSockFd();
    pollfd desc[2] = {};
    desc[0].fd = cynaraFd;
    desc[0].events = POLLIN;
    nfds_t eventCount = 1;

    if (m_notifyFd == -1) {
        LOGW("Notification fd not set, agent will be waiting forever");
    } else {
        desc[1].fd = m_notifyFd;
        desc[1].events = POLLIN;
        eventCount = 2;
    }

    int ret = TEMP_FAILURE_RETRY(poll(desc, eventCount, -1));
    if (ret == -1) {
        int err = errno;
        LOGE("Poll returned with error: " << strerror(err));
        return AgentSocketState::SS_ERROR;
    }
    if (desc[1].revents & POLLIN) {
        LOGD("Poll returned with notification to return");
        return AgentSocketState::SS_QUITREQUEST;
    }

    if (desc[0].revents & POLLIN) {
        LOGD("Poll returned with fetch entries");
        return AgentSocketState::SS_REQUEST;
    }

    // This should not occur
    LOGE("Poll woke up, but no known event received");
    return AgentSocketState::SS_ERROR;
}

ResponsePtr AgentSocketClient::getBufferedResponse(void) {
    return m_protocol->extractResponseFromBuffer(m_readQueue);
}

ResponsePtr AgentSocketClient::receiveResponseFromServer(void) {
    ResponsePtr response;
    while (!(response = m_protocol->extractResponseFromBuffer(m_readQueue))) {
        if (!m_socket.receiveFromServer(*m_readQueue)) {
            LOGW("Error receiving data from Cynara. Service not available.");
            return nullptr;
        }
    }
    return response;
}

bool AgentSocketClient::sendDataToServer(BinaryQueue &data) {
    if (m_socket.sendToServer(data) == Socket::SendStatus::CONNECTION_LOST) {
        LOGW("Error sending data to Cynara. Service not available.");
        return false;
    }

    return true;
}

bool AgentSocketClient::isConnected(void) {
    return m_socket.isConnected();
}

AgentSocketState AgentSocketClient::connect(void) {
    if (isConnected())
        return SS_CONNECTED;

    if (m_socket.connect() == Socket::ConnectionStatus::CONNECTION_SUCCEEDED) {
        resetState();
        return SS_RECONNECTED;
    }

    return SS_DISCONNECTED;
}

void AgentSocketClient::resetState(void) {
    m_readQueue->clear();
    m_writeQueue->clear();
}

} // namespace Cynara
