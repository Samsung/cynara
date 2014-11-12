/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
 * @file        src/common/protocol/ProtocolFrameSerializer.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of protocol frame (de)serializer class.
 */

#include <exceptions/InvalidProtocolException.h>
#include <exceptions/OutOfDataException.h>
#include <log/log.h>

#include "ProtocolFrameSerializer.h"

namespace Cynara {

void ProtocolFrameSerializer::deserializeHeader(ProtocolFrameHeader &frameHeader,
                                                BinaryQueue &data) {
    if (!frameHeader.isHeaderComplete()) {
        if ((data.size() < ProtocolFrameHeader::frameHeaderLength())) {
            return;
        }

        LOGD("Deserializing frameHeader");

        frameHeader.setHeaderContent(BinaryQueuePtr(&data, [=] (BinaryQueue *) {}));

        ProtocolFrameSignature signature;
        ProtocolDeserialization::deserialize(frameHeader, frameHeader.m_signature.length(),
                signature);

        LOGD("Deserialized signature = %s", signature.c_str());

        if (ProtocolFrameHeader::m_signature != signature) {
            throw InvalidProtocolException(InvalidProtocolException::InvalidSignature);
        }

        ProtocolDeserialization::deserialize(frameHeader, frameHeader.m_frameLength);
        ProtocolDeserialization::deserialize(frameHeader, frameHeader.m_sequenceNumber);

        LOGD("Deserialized frameLength = %d, sequenceNumber = %d",
             (int)frameHeader.m_frameLength, (int)frameHeader.m_sequenceNumber);

        frameHeader.setHeaderComplete();
    }

    if (data.size() >= (frameHeader.frameLength() - ProtocolFrameHeader::frameHeaderLength())) {
        frameHeader.setBodyComplete();
    }
}

ProtocolFrame ProtocolFrameSerializer::startSerialization(ProtocolFrameSequenceNumber sequenceNumber) {
    LOGD("Serialization started");

    BinaryQueuePtr headerQueue = std::make_shared<BinaryQueue>();
    BinaryQueuePtr bodyQueue = std::make_shared<BinaryQueue>();
    ProtocolFrameHeader header(headerQueue);
    header.setSequenceNumber(sequenceNumber);
    header.increaseFrameLength(ProtocolFrameHeader::frameHeaderLength());
    return ProtocolFrame(header, bodyQueue);
}

void ProtocolFrameSerializer::finishSerialization(ProtocolFrame &frame, BinaryQueue &data) {
    ProtocolFrameHeader &frameHeader = frame.frameHeader();
    ProtocolSerialization::serializeNoSize(frameHeader, ProtocolFrameHeader::m_signature);
    ProtocolSerialization::serialize(frameHeader, frameHeader.m_frameLength);
    ProtocolSerialization::serialize(frameHeader, frameHeader.m_sequenceNumber);

    LOGD("Serialize frameHeader: signature = %s, frameLength = %d, sequenceNumber = %d",
         ProtocolFrameHeader::m_signature.c_str(), (int)frameHeader.m_frameLength,
         (int)frameHeader.m_sequenceNumber);

    data.appendMoveFrom(frameHeader.headerContent());
    data.appendMoveFrom(frame.bodyContent());
}

} /* namespace Cynara */
