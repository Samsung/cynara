/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/protocol/ProtocolAgent.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for communication with agent
 */

#include <inttypes.h>
#include <memory>

#include <exceptions/InvalidProtocolException.h>
#include <log/log.h>
#include <protocol/ProtocolFrameSerializer.h>
#include <protocol/ProtocolOpCode.h>
#include <protocol/ProtocolSerialization.h>
#include <request/AgentActionRequest.h>
#include <request/AgentRegisterRequest.h>
#include <request/RequestContext.h>
#include <response/AgentActionResponse.h>
#include <response/AgentRegisterResponse.h>

#include "ProtocolAgent.h"

namespace Cynara {

ProtocolAgent::ProtocolAgent() {
}

ProtocolAgent::~ProtocolAgent() {
}

ProtocolPtr ProtocolAgent::clone(void) {
    return std::make_shared<ProtocolAgent>();
}

RequestPtr ProtocolAgent::deserializeActionRequest(void) {
    AgentRequestType requestType;
    RawBuffer data;

    ProtocolDeserialization::deserialize(m_frameHeader, requestType);
    ProtocolDeserialization::deserialize(m_frameHeader, data);

    LOGD("Deserialized AgentActionRequest: requestType [%" PRIu8 "], data lengtgh <%zu>",
         requestType, data.size());

    return std::make_shared<AgentActionRequest>(requestType, data, m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAgent::deserializeRegisterRequest(void) {
    AgentType agentType;

    ProtocolDeserialization::deserialize(m_frameHeader, agentType);

    LOGD("Deserialized AgentRegisterRequest: agent type <%s>", agentType.c_str());

    return std::make_shared<AgentRegisterRequest>(agentType, m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAgent::extractRequestFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
            case OpAgentActionRequest:
                return deserializeActionRequest();
            case OpAgentRegisterRequest:
                return deserializeRegisterRequest();
            default:
                throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
                break;
        }
    }

    return nullptr;
}

ResponsePtr ProtocolAgent::deserializeActionResponse(void) {
    AgentResponseType responseType;
    RawBuffer data;

    ProtocolDeserialization::deserialize(m_frameHeader, responseType);
    ProtocolDeserialization::deserialize(m_frameHeader, data);

    LOGD("Deserialized AgentActionResponse: responseType [%" PRIu8 "], data lengtgh <%zu>",
         responseType, data.size());

    return std::make_shared<AgentActionResponse>(responseType, data,
                                                 m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolAgent::deserializeRegisterResponse(void) {
    ProtocolResponseCode result;

    ProtocolDeserialization::deserialize(m_frameHeader, result);

    LOGD("Deserialized AgentRegisterResponse: result [%d]", static_cast<int>(result));

    return std::make_shared<AgentRegisterResponse>(static_cast<AgentRegisterResponse::Code>(result),
                                                   m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolAgent::extractResponseFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
            case OpAgentActionResponse:
                return deserializeActionResponse();
            case OpAgentRegisterResponse:
                return deserializeRegisterResponse();
            default:
                throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
                break;
        }
    }

    return nullptr;
}

void ProtocolAgent::execute(RequestContextPtr context, AgentActionRequestPtr request) {
    LOGD("Serializing AgentActionRequest: op [%" PRIu8 "], requestType [%" PRIu8 "], "
         "data lengtgh <%zu>", OpAgentActionRequest, request->type(), request->data().size());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpAgentActionRequest);
    ProtocolSerialization::serialize(frame, request->type());
    ProtocolSerialization::serialize(frame, request->data());

    ProtocolFrameSerializer::finishSerialization(frame, *context->responseQueue());
}

void ProtocolAgent::execute(RequestContextPtr context, AgentRegisterRequestPtr request) {
    LOGD("Serializing AgentRegisterRequest: op [%" PRIu8 "], agent type <%s>",
         OpAgentRegisterRequest, request->agentType().c_str());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpAgentRegisterRequest);
    ProtocolSerialization::serialize(frame, request->agentType());

    ProtocolFrameSerializer::finishSerialization(frame, *context->responseQueue());
}

void ProtocolAgent::execute(RequestContextPtr context, AgentRegisterResponsePtr response) {
    LOGD("Serializing AgentRegisterResponse: op [%" PRIu8 "], response code: [%d]",
         OpAgentRegisterResponse, static_cast<int>(response->m_code));

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(response->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpAgentRegisterResponse);
    ProtocolSerialization::serialize(frame, static_cast<ProtocolResponseCode>(response->m_code));

    ProtocolFrameSerializer::finishSerialization(frame, *context->responseQueue());
}

void ProtocolAgent::execute(RequestContextPtr context, AgentActionResponsePtr response) {
    LOGD("Serializing AgentActionResponse: op [%" PRIu8 "], responseType [%" PRIu8 "], "
         "data lengtgh <%zu>", OpAgentActionResponse, response->type(), response->data().size());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(response->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpAgentActionResponse);
    ProtocolSerialization::serialize(frame, response->type());
    ProtocolSerialization::serialize(frame, response->data());

    ProtocolFrameSerializer::finishSerialization(frame, *context->responseQueue());
}

} // namespace Cynara
