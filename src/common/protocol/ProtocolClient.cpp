/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
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
#include <request/MonitorEntriesPutRequest.h>
#include <request/RequestContext.h>
#include <request/SimpleCheckRequest.h>
#include <response/CancelResponse.h>
#include <response/CheckResponse.h>
#include <response/SimpleCheckResponse.h>
#include <types/MonitorEntry.h>
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

RequestPtr ProtocolClient::deserializeCancelRequest(void) {
    LOGD("Deserialized CancelRequest");
    return std::make_shared<CancelRequest>(m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolClient::deserializeCheckRequest(void) {
    std::string clientId, userId, privilegeId;

    ProtocolDeserialization::deserialize(m_frameHeader, clientId);
    ProtocolDeserialization::deserialize(m_frameHeader, userId);
    ProtocolDeserialization::deserialize(m_frameHeader, privilegeId);

    LOGD("Deserialized CheckRequest: client <%s>, user <%s>, privilege <%s>",
         clientId.c_str(), userId.c_str(), privilegeId.c_str());

    return std::make_shared<CheckRequest>(PolicyKey(clientId, userId, privilegeId),
                                          m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolClient::deserializeSimpleCheckRequest(void) {
    std::string clientId, userId, privilegeId;

    ProtocolDeserialization::deserialize(m_frameHeader, clientId);
    ProtocolDeserialization::deserialize(m_frameHeader, userId);
    ProtocolDeserialization::deserialize(m_frameHeader, privilegeId);

    LOGD("Deserialized SimpleCheckRequest: client <%s>, user <%s>, privilege <%s>",
         clientId.c_str(), userId.c_str(), privilegeId.c_str());

    return std::make_shared<SimpleCheckRequest>(PolicyKey(clientId, userId, privilegeId),
                                                m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolClient::deserializeMonitorEntriesPutRequest(void) {
    ProtocolFrameFieldsCount entriesCount;

    ProtocolDeserialization::deserialize(m_frameHeader, entriesCount);
    std::vector<MonitorEntry> entries;
    entries.reserve(entriesCount);

    for (ProtocolFrameFieldsCount fields = 0; fields < entriesCount; fields++) {
        std::string clientId, userId, privilegeId;
        int64_t result, tv_sec, tv_nsec;

        ProtocolDeserialization::deserialize(m_frameHeader, clientId);
        ProtocolDeserialization::deserialize(m_frameHeader, userId);
        ProtocolDeserialization::deserialize(m_frameHeader, privilegeId);
        ProtocolDeserialization::deserialize(m_frameHeader, result);
        ProtocolDeserialization::deserialize(m_frameHeader, tv_sec);
        ProtocolDeserialization::deserialize(m_frameHeader, tv_nsec);

        PolicyKey key(clientId, userId, privilegeId);
        struct timespec timestamp;
        timestamp.tv_sec = static_cast<__time_t>(tv_sec);
        timestamp.tv_nsec = static_cast<__syscall_slong_t>(tv_nsec);
        entries.emplace_back(MonitorEntry(key, static_cast<size_t>(result), timestamp));
    }

    LOGD("Deserialized MonitorEntriesPutRequest: number of entries [%" PRIu16 "]", entriesCount);

    return std::make_shared<MonitorEntriesPutRequest>(entries, m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolClient::extractRequestFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);

        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpCheckPolicyRequest:
            return deserializeCheckRequest();
        case OpCancelRequest:
            return deserializeCancelRequest();
        case OpSimpleCheckPolicyRequest:
            return deserializeSimpleCheckRequest();
        case OpMonitorEntriesPutRequest:
            return deserializeMonitorEntriesPutRequest();
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

ResponsePtr ProtocolClient::deserializeCancelResponse(void) {
    LOGD("Deserialized CancelResponse");
    return std::make_shared<CancelResponse>(m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolClient::deserializeCheckResponse(void) {
    PolicyType result;
    PolicyResult::PolicyMetadata additionalInfo;

    ProtocolDeserialization::deserialize(m_frameHeader, result);
    ProtocolDeserialization::deserialize(m_frameHeader, additionalInfo);

    const PolicyResult policyResult(result, additionalInfo);

    LOGD("Deserialized CheckResponse: result [%" PRIu16 "], metadata <%s>",
         policyResult.policyType(), policyResult.metadata().c_str());

    return std::make_shared<CheckResponse>(policyResult, m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolClient::deserializeSimpleCheckResponse() {
    int32_t retValue;
    PolicyType result;
    PolicyResult::PolicyMetadata additionalInfo;

    ProtocolDeserialization::deserialize(m_frameHeader, retValue);
    ProtocolDeserialization::deserialize(m_frameHeader, result);
    ProtocolDeserialization::deserialize(m_frameHeader, additionalInfo);

    const PolicyResult policyResult(result, additionalInfo);

    LOGD("Deserialized SimpleCheckResponse: retVal [%" PRIi32 "%] result [%" PRIu16 "],"
         " metadata <%s>", retValue, policyResult.policyType(), policyResult.metadata().c_str());

    return std::make_shared<SimpleCheckResponse>(retValue, policyResult,
                                                 m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolClient::extractResponseFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpCheckPolicyResponse:
            return deserializeCheckResponse();
        case OpCancelResponse:
            return deserializeCancelResponse();
        case OpSimpleCheckPolicyResponse:
            return deserializeSimpleCheckResponse();
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

void ProtocolClient::execute(const RequestContext &context, const CancelRequest &request) {
    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request.sequenceNumber());

    LOGD("Serializing CancelRequest op [%" PRIu8 "]", OpCancelRequest);

    ProtocolSerialization::serialize(frame, OpCancelRequest);

    ProtocolFrameSerializer::finishSerialization(frame, *(context.responseQueue()));
}

void ProtocolClient::execute(const RequestContext &context, const CheckRequest &request) {
    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request.sequenceNumber());

    LOGD("Serializing CheckRequest: client <%s>, user <%s>, privilege <%s>",
         request.key().client().value().c_str(), request.key().user().value().c_str(),
         request.key().privilege().value().c_str());

    ProtocolSerialization::serialize(frame, OpCheckPolicyRequest);
    ProtocolSerialization::serialize(frame, request.key().client().value());
    ProtocolSerialization::serialize(frame, request.key().user().value());
    ProtocolSerialization::serialize(frame, request.key().privilege().value());

    ProtocolFrameSerializer::finishSerialization(frame, *(context.responseQueue()));
}

void ProtocolClient::execute(const RequestContext &context, const SimpleCheckRequest &request) {
    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request.sequenceNumber());

    LOGD("Serializing SimpleCheckRequest: client <%s>, user <%s>, privilege <%s>",
         request.key().client().value().c_str(), request.key().user().value().c_str(),
         request.key().privilege().value().c_str());

    ProtocolSerialization::serialize(frame, OpSimpleCheckPolicyRequest);
    ProtocolSerialization::serialize(frame, request.key().client().value());
    ProtocolSerialization::serialize(frame, request.key().user().value());
    ProtocolSerialization::serialize(frame, request.key().privilege().value());

    ProtocolFrameSerializer::finishSerialization(frame, *(context.responseQueue()));
}

void ProtocolClient::execute(const RequestContext &context,
                             const MonitorEntriesPutRequest &request) {
    ProtocolFrameFieldsCount entriesCount
            = static_cast<ProtocolFrameFieldsCount>(request.monitorEntries().size());
    LOGD("Serializing MonitorEntriesPutRequest: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
            "number of entries [%" PRIu16 "]",
         OpMonitorEntriesPutRequest, request.sequenceNumber(), entriesCount);

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request.sequenceNumber());

    ProtocolSerialization::serialize(frame, OpMonitorEntriesPutRequest);
    ProtocolSerialization::serialize(frame, entriesCount);
    for (const auto &entry : request.monitorEntries()) {
        ProtocolSerialization::serialize(frame, entry.key().client().toString());
        ProtocolSerialization::serialize(frame, entry.key().user().toString());
        ProtocolSerialization::serialize(frame, entry.key().privilege().toString());
        ProtocolSerialization::serialize(frame, static_cast<int64_t>(entry.result()));
        ProtocolSerialization::serialize(frame, static_cast<int64_t>(entry.timestamp().tv_sec));
        ProtocolSerialization::serialize(frame, static_cast<int64_t>(entry.timestamp().tv_nsec));
    }

    ProtocolFrameSerializer::finishSerialization(frame, *context.responseQueue());
}

void ProtocolClient::execute(const RequestContext &context, const CancelResponse &response) {
    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(
            response.sequenceNumber());

    LOGD("Serializing CancelResponse: op [%" PRIu8 "]", OpCancelResponse);

    ProtocolSerialization::serialize(frame, OpCancelResponse);

    ProtocolFrameSerializer::finishSerialization(frame, *(context.responseQueue()));
}

void ProtocolClient::execute(const RequestContext &context, const CheckResponse &response) {
    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(
            response.sequenceNumber());

    LOGD("Serializing CheckResponse: op [%" PRIu8 "], policyType [%" PRIu16 "], metadata <%s>",
         OpCheckPolicyResponse, response.m_resultRef.policyType(),
         response.m_resultRef.metadata().c_str());

    ProtocolSerialization::serialize(frame, OpCheckPolicyResponse);
    ProtocolSerialization::serialize(frame, response.m_resultRef.policyType());
    ProtocolSerialization::serialize(frame, response.m_resultRef.metadata());

    ProtocolFrameSerializer::finishSerialization(frame, *(context.responseQueue()));
}

void ProtocolClient::execute(const RequestContext &context, const SimpleCheckResponse &response) {
    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(
            response.sequenceNumber());

    LOGD("Serializing SimpleCheckResponse: op [%" PRIu8 "], retVal [%" PRIi32 "],"
         " policyType [%" PRIu16 "], metadata <%s>", OpCheckPolicyResponse,
         response.getReturnValue(), response.getResult().policyType(),
         response.getResult().metadata().c_str());

    ProtocolSerialization::serialize(frame, OpSimpleCheckPolicyResponse);
    ProtocolSerialization::serialize(frame, response.getReturnValue());
    ProtocolSerialization::serialize(frame, response.getResult().policyType());
    ProtocolSerialization::serialize(frame, response.getResult().metadata());

    ProtocolFrameSerializer::finishSerialization(frame, *(context.responseQueue()));
}

} // namespace Cynara
