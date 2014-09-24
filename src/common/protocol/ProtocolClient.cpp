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
 * @file        src/common/protocol/ProtocolClient.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for communication with client
 */

#include <cinttypes>
#include <memory>

#include <common.h>
#include <exceptions/InvalidProtocolException.h>
#include <exceptions/OutOfDataException.h>
#include <log/log.h>
#include <protocol/ProtocolFrameSerializer.h>
#include <protocol/ProtocolOpCode.h>
#include <protocol/ProtocolSerialization.h>
#include <request/CancelRequest.h>
#include <request/CheckRequest.h>
#include <request/RequestContext.h>
#include <response/CancelResponse.h>
#include <response/CheckResponse.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include "ProtocolClient.h"

namespace Cynara {

ProtocolClient::ProtocolClient() {
}

ProtocolClient::~ProtocolClient() {
}

ProtocolPtr ProtocolClient::clone(void) {
    return std::make_shared<ProtocolClient>();
}

RequestPtr ProtocolClient::deserializeCancelRequest(ProtocolFrameHeader &frame) {
    LOGD("Deserialized CancelRequest");
    return std::make_shared<CancelRequest>(frame.sequenceNumber());
}

RequestPtr ProtocolClient::deserializeCheckRequest(ProtocolFrameHeader &frame) {
    std::string clientId, userId, privilegeId;

    ProtocolDeserialization::deserialize(frame, clientId);
    ProtocolDeserialization::deserialize(frame, userId);
    ProtocolDeserialization::deserialize(frame, privilegeId);

    LOGD("Deserialized CheckRequest: client <%s>, user <%s>, privilege <%s>",
         clientId.c_str(), userId.c_str(), privilegeId.c_str());

    return std::make_shared<CheckRequest>(PolicyKey(clientId, userId, privilegeId),
            frame.sequenceNumber());
}

RequestPtr ProtocolClient::extractRequestFromBuffer(BinaryQueue &bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);

        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpCheckPolicyRequest:
            return deserializeCheckRequest(m_frameHeader);
        case OpCancelRequest:
            return deserializeCancelRequest(m_frameHeader);
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

ResponsePtr ProtocolClient::deserializeCancelResponse(ProtocolFrameHeader &frame) {
    LOGD("Deserialized CancelResponse");
    return std::make_shared<CancelResponse>(frame.sequenceNumber());
}

ResponsePtr ProtocolClient::deserializeCheckResponse(ProtocolFrameHeader &frame) {
    PolicyType result;
    PolicyResult::PolicyMetadata additionalInfo;

    ProtocolDeserialization::deserialize(frame, result);
    ProtocolDeserialization::deserialize(frame, additionalInfo);

    const PolicyResult policyResult(result, additionalInfo);

    LOGD("Deserialized CheckResponse: result [%" PRIu16 "], metadata <%s>",
         policyResult.policyType(), policyResult.metadata().c_str());

    return std::make_shared<CheckResponse>(policyResult, frame.sequenceNumber());
}

ResponsePtr ProtocolClient::extractResponseFromBuffer(BinaryQueue &bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpCheckPolicyResponse:
            return deserializeCheckResponse(m_frameHeader);
        case OpCancelResponse:
            return deserializeCancelResponse(m_frameHeader);
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

void ProtocolClient::execute(RequestContextPtr context, CancelRequestPtr request) {
    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    LOGD("Serializing CancelRequest op [%" PRIu8 "]", OpCancelRequest);

    ProtocolSerialization::serialize(*frame, OpCancelRequest);

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolClient::execute(RequestContextPtr context, CheckRequestPtr request) {
    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    LOGD("Serializing CheckRequest: client <%s>, user <%s>, privilege <%s>",
         request->key().client().value().c_str(), request->key().user().value().c_str(),
         request->key().privilege().value().c_str());

    ProtocolSerialization::serialize(*frame, OpCheckPolicyRequest);
    ProtocolSerialization::serialize(*frame, request->key().client().value());
    ProtocolSerialization::serialize(*frame, request->key().user().value());
    ProtocolSerialization::serialize(*frame, request->key().privilege().value());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolClient::execute(RequestContextPtr context, CancelResponsePtr response) {
    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(
            response->sequenceNumber());

    LOGD("Serializing CancelResponse: op [%" PRIu8 "]", OpCancelResponse);

    ProtocolSerialization::serialize(*frame, OpCancelResponse);

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolClient::execute(RequestContextPtr context, CheckResponsePtr response) {
    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(
            response->sequenceNumber());

    LOGD("Serializing CheckResponse: op [%" PRIu8 "], policyType [%" PRIu16 "], metadata <%s>",
         OpCheckPolicyResponse, response->m_resultRef.policyType(),
         response->m_resultRef.metadata().c_str());

    ProtocolSerialization::serialize(*frame, OpCheckPolicyResponse);
    ProtocolSerialization::serialize(*frame, response->m_resultRef.policyType());
    ProtocolSerialization::serialize(*frame, response->m_resultRef.metadata());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

} // namespace Cynara
