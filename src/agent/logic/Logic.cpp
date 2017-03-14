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
 * @file        src/agent/logic/Logic.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-agent class
 */

#include <cinttypes>
#include <memory>

#include <common.h>
#include <config/PathConfig.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolAgent.h>
#include <request/AgentActionRequest.h>
#include <request/AgentRegisterRequest.h>
#include <request/pointers.h>
#include <request/RequestContext.h>
#include <response/AgentActionResponse.h>
#include <response/AgentRegisterResponse.h>
#include <response/pointers.h>
#include <types/ProtocolFields.h>

#include <cynara-error.h>

#include "Logic.h"

namespace {

Cynara::ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static Cynara::ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

} // namespace anonymous

namespace Cynara {

Logic::Logic(const AgentType &agentType) : m_agentType(agentType),
        m_socketClient(PathConfig::SocketPath::agent, std::make_shared<ProtocolAgent>()) {
    m_responseTakerPtr = std::make_shared<ProtocolAgent>();
    m_responseBuffer = std::make_shared<BinaryQueue>();
    if (!m_notify.init()) {
        LOGW("Couldn't initialize notification object.");
        return;
    }
    m_socketClient.setNotifyFd(m_notify.getNotifyFd());
}

int Logic::registerInCynara(void) {
    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    AgentRegisterResponsePtr registerResponsePtr;
    AgentRegisterRequest request(m_agentType, sequenceNumber);
    ResponsePtr response = m_socketClient.askCynaraServer(request);
    if (!response) {
        LOGW("Disconnected by cynara server.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    registerResponsePtr = std::dynamic_pointer_cast<AgentRegisterResponse>(response);
    if (!registerResponsePtr) {
        LOGC("Casting response to AgentRegisterResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    LOGD("registerResponse: answer code [%d]", static_cast<int>(registerResponsePtr->m_code));

    switch (registerResponsePtr->m_code) {
        case AgentRegisterResponse::DONE:
            return CYNARA_API_SUCCESS;
        case AgentRegisterResponse::REJECTED:
            LOGE("Registering agent of type <%s> has been rejected", m_agentType.c_str());
            return CYNARA_API_ACCESS_DENIED;
        default:
            LOGE("Registering agent of type <%s> has finished with unknown error",
                 m_agentType.c_str());
            return CYNARA_API_UNKNOWN_ERROR;
    }
}

int Logic::ensureConnection(void) {
    auto state = m_socketClient.connect();
    switch (state) {
        case SS_CONNECTED:
            return CYNARA_API_SUCCESS;
        case SS_RECONNECTED:
            return registerInCynara();
        case SS_DISCONNECTED:
            LOGE("Agent socket disconnected.");
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
        case SS_REQUEST:
        case SS_QUITREQUEST:
            LOGE("Unexpected state returned : [" << state << "]");
            return CYNARA_API_UNKNOWN_ERROR;
        case SS_ERROR:
            LOGE("Failed to connect to cynara service");
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    LOGE("Unknown state returned : [" << state << "]");
    return CYNARA_API_UNKNOWN_ERROR;
}

int Logic::getRequest(AgentActionResponsePtr &resultPtr) {
    int ret = ensureConnection();
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    ResponsePtr responsePtr = m_socketClient.getBufferedResponse();
    if (responsePtr == nullptr) {
        AgentSocketState state = m_socketClient.waitForEvent();
        switch (state) {
        case AgentSocketState::SS_QUITREQUEST:
            LOGD("Waiting interrupted. Finishing");
            m_notify.snooze();
            return CYNARA_API_INTERRUPTED;
        case AgentSocketState::SS_REQUEST:
        {
            responsePtr = m_socketClient.receiveResponseFromServer();
            if (!responsePtr) {
                LOGW("Disconnected by cynara server.");
                return CYNARA_API_SERVICE_NOT_AVAILABLE;
            }
            break;
        }
        default:
            LOGE("Wrong state returned [" << state << "]");
            return CYNARA_API_UNKNOWN_ERROR;
        }
    }
    AgentActionResponsePtr actionResponsePtr =
        std::dynamic_pointer_cast<AgentActionResponse>(responsePtr);
    if (!actionResponsePtr) {
        LOGC("Casting request to AgentActionResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    LOGD("agentActionResponse: type: [%" PRIu8 "], data length: [%zu]",
         actionResponsePtr->type(), actionResponsePtr->data().size());

    resultPtr = actionResponsePtr;
    return CYNARA_API_SUCCESS;
}

int Logic::putResponse(const AgentResponseType responseType,
                       const ProtocolFrameSequenceNumber sequenceNumber,
                       const RawBuffer &pluginData) {
    if (!m_socketClient.isConnected()) {
        LOGE("Agent not connected to cynara service.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    AgentActionRequest request(responseType, pluginData, sequenceNumber);
    m_responseBuffer->clear();
    RequestContext context(ResponseTakerPtr(), m_responseBuffer);
    request.execute(*m_responseTakerPtr, context);
    return m_socketClient.sendDataToServer(*m_responseBuffer) ? CYNARA_API_SUCCESS :
                                                     CYNARA_API_SERVICE_NOT_AVAILABLE;
}

int Logic::cancelWaiting(void) {
    if (!m_notify.notify()) {
        LOGW("Couldn't notify agent loop");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    return CYNARA_API_SUCCESS;
}

} // namespace Cynara
