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
/*
 * @file        ProtocolClient.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for communication with client
 */

#include <common.h>
#include <memory>

#include <exceptions/InvalidProtocolException.h>
#include <exceptions/OutOfDataException.h>
#include <log/log.h>
#include <protocol/ProtocolFrameSerializer.h>
#include <protocol/ProtocolOpCode.h>
#include <protocol/ProtocolSerialization.h>
#include <request/CheckRequest.h>
#include <request/RequestContext.h>
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

RequestPtr ProtocolClient::deserializeCheckRequest(ProtocolFrameHeader &frame) {
    std::string clientId, userId, privilegeId;

    ProtocolDeserialization::deserialize(frame, clientId);
    ProtocolDeserialization::deserialize(frame, userId);
    ProtocolDeserialization::deserialize(frame, privilegeId);

    LOGD("Deserialized CheckRequest: client = %s, user = %s, privilege = %s",
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
        LOGD("Deserialized opCode = %d", (int)opCode);
        switch (opCode) {
        case OpCheckPolicy:
            return deserializeCheckRequest(m_frameHeader);
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

ResponsePtr ProtocolClient::deserializeCheckResponse(ProtocolFrameHeader &frame) {
    PolicyType result;
    PolicyResult::PolicyMetadata additionalInfo;

    ProtocolDeserialization::deserialize(frame, result);
    ProtocolDeserialization::deserialize(frame, additionalInfo);

    const PolicyResult policyResult(result, additionalInfo);

    LOGD("Deserialized CheckResponse: result = %d, metadata = %s",
         (int)policyResult.policyType(), policyResult.metadata().c_str());

    return std::make_shared<CheckResponse>(policyResult, frame.sequenceNumber());
}

ResponsePtr ProtocolClient::extractResponseFromBuffer(BinaryQueue &bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode = %d", (int)opCode);
        switch (opCode) {
        case OpCheckPolicy:
            return deserializeCheckResponse(m_frameHeader);
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

void ProtocolClient::execute(RequestContextPtr context, CheckRequestPtr request) {
    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    LOGD("Serializing CheckRequest: client = %s, user = %s, privilege = %s",
         request->key().client().value().c_str(),
         request->key().user().value().c_str(), request->key().privilege().value().c_str());

    ProtocolSerialization::serialize(*frame, OpCheckPolicy);
    ProtocolSerialization::serialize(*frame, request->key().client().value());
    ProtocolSerialization::serialize(*frame, request->key().user().value());
    ProtocolSerialization::serialize(*frame, request->key().privilege().value());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolClient::execute(RequestContextPtr context, CheckResponsePtr response) {
    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(response->sequenceNumber());

    LOGD("Serializing CheckResponse: op [%d], policyType [%d], metadata <%s>",
         (int)OpCheckPolicy, (int)response->m_resultRef.policyType(),
         response->m_resultRef.metadata().c_str());

    ProtocolSerialization::serialize(*frame, OpCheckPolicy);
    ProtocolSerialization::serialize(*frame, response->m_resultRef.policyType());
    ProtocolSerialization::serialize(*frame, response->m_resultRef.metadata());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

} // namespace Cynara
