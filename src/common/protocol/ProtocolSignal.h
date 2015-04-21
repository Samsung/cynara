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
 * @file        src/common/protocol/ProtocolSignal.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines protocol class for signals
 */

#ifndef SRC_COMMON_PROTOCOL_PROTOCOLSIGNAL_H_
#define SRC_COMMON_PROTOCOL_PROTOCOLSIGNAL_H_

#include <request/pointers.h>

#include "Protocol.h"

namespace Cynara {

class ProtocolSignal : public Protocol {
public:
    ProtocolSignal();
    virtual ~ProtocolSignal();

    virtual ProtocolPtr clone(void);

    virtual RequestPtr extractRequestFromBuffer(BinaryQueuePtr bufferQueue);
    virtual ResponsePtr extractResponseFromBuffer(BinaryQueuePtr bufferQueue);

    using Protocol::execute;

    virtual void execute(const RequestContext &context, const SignalRequest &request);
};

} // namespace Cynara

#endif /* SRC_COMMON_PROTOCOL_PROTOCOLSIGNAL_H_ */
