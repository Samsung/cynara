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
 * @file        src/common/protocol/Protocol.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines protocol base class
 */

#ifndef SRC_COMMON_PROTOCOL_PROTOCOL_H_
#define SRC_COMMON_PROTOCOL_PROTOCOL_H_

#include <memory>

#include <containers/BinaryQueue.h>
#include <protocol/ProtocolFrameHeader.h>
#include <request/pointers.h>
#include <request/RequestTaker.h>
#include <response/pointers.h>
#include <response/ResponseTaker.h>

namespace Cynara {

class Protocol;
typedef std::shared_ptr<Protocol> ProtocolPtr;

class Protocol : public RequestTaker, public ResponseTaker {
public:
    Protocol() = default;
    virtual ~Protocol() {};

    virtual ProtocolPtr clone(void) = 0;

    virtual RequestPtr extractRequestFromBuffer(BinaryQueuePtr bufferQueue) = 0;
    virtual ResponsePtr extractResponseFromBuffer(BinaryQueuePtr bufferQueue) = 0;

    using RequestTaker::execute;
    using ResponseTaker::execute;

    ProtocolFrameHeader &frameHeader(void) {
        return m_frameHeader;
    }

protected:
    ProtocolFrameHeader m_frameHeader;
};

} // namespace Cynara

#endif /* SRC_COMMON_PROTOCOL_PROTOCOL_H_ */
