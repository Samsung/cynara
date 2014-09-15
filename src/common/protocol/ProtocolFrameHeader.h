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
 * @file        ProtocolFrameHeader.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Header file for protocol frame header (de)serializer class.
 */

#ifndef SRC_COMMON_PROTOCOL_PROTOCOLFRAMEHEADER_H_
#define SRC_COMMON_PROTOCOL_PROTOCOLFRAMEHEADER_H_

#include <cstddef>

#include <containers/BinaryQueue.h>
#include <protocol/ProtocolSerialization.h>
#include <types/ProtocolFields.h>

namespace Cynara {

class ProtocolFrameSerializer;

class ProtocolFrameHeader: public IStream {
private:
    static const ProtocolFrameSignature m_signature;
    static size_t frameHeaderLength(void) {
            return m_signature.size() +
            sizeof(ProtocolFrameLength) +
            sizeof(ProtocolFrameSequenceNumber);
    }

public:
    ProtocolFrameHeader(BinaryQueuePtr headerContent = nullptr);
    virtual ~ProtocolFrameHeader() {};

    virtual void read(size_t num, void *bytes);
    virtual void write(size_t num, const void *bytes);

    ProtocolFrameSequenceNumber sequenceNumber(void) {
        return m_sequenceNumber;
    }

    bool isHeaderComplete(void) {
        return m_headerComplete;
    }

    bool isFrameComplete(void) {
        return m_headerComplete && m_bodyComplete;
    }

    void resetState(void) {
        m_headerComplete = false;
        m_bodyComplete = false;
    }

    ProtocolFrameLength frameLength(void) {
        return m_frameLength;
    }

private:
    BinaryQueuePtr m_frameHeaderContent;
    ProtocolFrameLength m_frameLength;
    ProtocolFrameSequenceNumber m_sequenceNumber;
    bool m_headerComplete;
    bool m_bodyComplete;

    void setSequenceNumber(ProtocolFrameSequenceNumber sequenceNumber) {
        m_sequenceNumber = sequenceNumber;
    }

    void increaseFrameLength(ProtocolFrameLength size) {
        m_frameLength += size;
    }

    BinaryQueue &headerContent(void) {
        return *m_frameHeaderContent;
    }

    void setHeaderContent(BinaryQueuePtr headerContent) {
        m_frameHeaderContent = headerContent;
    }

    void setHeaderComplete(void) {
        m_headerComplete = true;
    }

    void setBodyComplete(void) {
        m_bodyComplete = true;
    }

    friend class ProtocolFrame;
    friend class ProtocolFrameSerializer;
};

typedef std::shared_ptr<ProtocolFrameHeader> ProtocolFrameHeaderPtr;

} /* namespace Cynara */

#endif /* SRC_COMMON_PROTOCOL_PROTOCOLFRAMEHEADER_H_ */
