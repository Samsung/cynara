/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/protocol/ProtocolMonitorGet.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for communication with
 *              monitor getters API
 */

#include <cinttypes>
#include <memory>

#include <exceptions/InvalidProtocolException.h>
#include <log/log.h>
#include <protocol/ProtocolFrameSerializer.h>
#include <protocol/ProtocolOpCode.h>
#include <protocol/ProtocolSerialization.h>
#include <request/MonitorGetEntriesRequest.h>
#include <request/MonitorGetFlushRequest.h>
#include <request/RequestContext.h>
#include <response/MonitorGetEntriesResponse.h>

#include "ProtocolMonitorGet.h"

namespace Cynara {

ProtocolMonitorGet::ProtocolMonitorGet() {
}

ProtocolMonitorGet::~ProtocolMonitorGet() {
}

ProtocolPtr ProtocolMonitorGet::clone(void) {
    return std::make_shared<ProtocolMonitorGet>();
}

RequestPtr ProtocolMonitorGet::deserializeMonitorGetEntriesRequest(void) {
    uint64_t bufferSize;

    ProtocolDeserialization::deserialize(m_frameHeader, bufferSize);

    LOGD("Deserialized MonitorGetEntriesRequest: bufferSize [%" PRIu16 "]", bufferSize);

    return std::make_shared<MonitorGetEntriesRequest>(static_cast<size_t>(bufferSize),
            m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolMonitorGet::deserializeMonitorGetFlushRequest(void) {
    LOGD("Deserialized MonitorGetFlushRequest");

    return std::make_shared<MonitorGetFlushRequest>(m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolMonitorGet::extractRequestFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (!m_frameHeader.isFrameComplete()) {
        return nullptr;
    }
    ProtocolOpCode opCode;

    m_frameHeader.resetState();
    ProtocolDeserialization::deserialize(m_frameHeader, opCode);
    LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
    switch (opCode) {
        case OpMonitorGetEntriesRequest:
            return deserializeMonitorGetEntriesRequest();
        case OpMonitorGetFlushRequest:
            return deserializeMonitorGetFlushRequest();
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
    }
}

ResponsePtr ProtocolMonitorGet::deserializeMonitorGetEntriesResponse(void) {
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

    LOGD("Deserialized MonitorGetEntriesResponse: number of entries [%" PRIu16 "]", entriesCount);

    return std::make_shared<MonitorGetEntriesResponse>(entries, m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolMonitorGet::extractResponseFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (!m_frameHeader.isFrameComplete()) {
        return nullptr;
    }
    ProtocolOpCode opCode;

    m_frameHeader.resetState();
    ProtocolDeserialization::deserialize(m_frameHeader, opCode);
    LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
    switch (opCode) {
        case OpMonitorGetEntriesResponse:
            return deserializeMonitorGetEntriesResponse();
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
    }
}

void ProtocolMonitorGet::execute(const RequestContext &context,
                                 const MonitorGetEntriesRequest &request)
{
    LOGD("Serializing MonitorGetEntriesRequest: op [%" PRIu8 "], bufferSize [%" PRIu16 "]",
            OpMonitorGetEntriesRequest, request.bufferSize());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request.sequenceNumber());
    ProtocolSerialization::serialize(frame, OpMonitorGetEntriesRequest);
    ProtocolSerialization::serialize(frame, static_cast<uint64_t>(request.bufferSize()));
    ProtocolFrameSerializer::finishSerialization(frame, *context.responseQueue());
}

void ProtocolMonitorGet::execute(const RequestContext &context,
                                 const MonitorGetFlushRequest &request)
{
    LOGD("Serializing MonitorGetFlushRequest: op [%" PRIu8 "]", OpMonitorGetFlushRequest);

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request.sequenceNumber());
    ProtocolSerialization::serialize(frame, OpMonitorGetFlushRequest);
    ProtocolFrameSerializer::finishSerialization(frame, *context.responseQueue());
}

void ProtocolMonitorGet::execute(const RequestContext &context,
                                 const MonitorGetEntriesResponse &response)
{
    ProtocolFrameFieldsCount entriesCount
            = static_cast<ProtocolFrameFieldsCount>(response.entries().size());
    LOGD("Serializing MonitorGetEntriesResponse: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
            "number of entries [%" PRIu16 "]",
            OpMonitorGetEntriesResponse, response.sequenceNumber(), entriesCount);

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(response.sequenceNumber());

    ProtocolSerialization::serialize(frame, OpMonitorGetEntriesResponse);
    ProtocolSerialization::serialize(frame, entriesCount);
    for (const auto &entry : response.entries()) {
        ProtocolSerialization::serialize(frame, entry.key().client().toString());
        ProtocolSerialization::serialize(frame, entry.key().user().toString());
        ProtocolSerialization::serialize(frame, entry.key().privilege().toString());
        ProtocolSerialization::serialize(frame, static_cast<int64_t>(entry.result()));
        ProtocolSerialization::serialize(frame, static_cast<int64_t>(entry.timestamp().tv_sec));
        ProtocolSerialization::serialize(frame, static_cast<int64_t>(entry.timestamp().tv_nsec));
    }

    ProtocolFrameSerializer::finishSerialization(frame, *context.responseQueue());
}

} // namespace Cynara
