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
 * @file        src/common/protocol/ProtocolFrame.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Header for ProtocolFrame class.
 */

#ifndef SRC_COMMON_PROTOCOL_PROTOCOLFRAME_H_
#define SRC_COMMON_PROTOCOL_PROTOCOLFRAME_H_

#include <cstddef>
#include <memory>

#include <containers/BinaryQueue.h>
#include <protocol/ProtocolFrameHeader.h>
#include <protocol/ProtocolSerialization.h>

namespace Cynara {

class ProtocolFrameSerializer;

class ProtocolFrame: public IStream {

public:
    ProtocolFrame(ProtocolFrameHeaderPtr frameHeader, BinaryQueuePtr headerContent);
    virtual ~ProtocolFrame() {};

    ProtocolFrameHeaderPtr frameHeader(void) {
        return m_frameHeader;
    }

    virtual void read(size_t num, void *bytes);
    virtual void write(size_t num, const void *bytes);

private:
    ProtocolFrameHeaderPtr m_frameHeader;
    BinaryQueuePtr m_frameBodyContent;

    BinaryQueue &bodyContent(void) {
        return *m_frameBodyContent;
    }

    friend class ProtocolFrameSerializer;
};

typedef std::shared_ptr<ProtocolFrame> ProtocolFramePtr;

} /* namespace Cynara */

#endif /* SRC_COMMON_PROTOCOL_PROTOCOLFRAME_H_ */
