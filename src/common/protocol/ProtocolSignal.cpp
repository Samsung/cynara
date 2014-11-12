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
 * @file        src/common/protocol/ProtocolSignal.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for signals
 */

#include <memory>
#include <sys/signalfd.h>

#include <common.h>
#include <exceptions/NotImplementedException.h>
#include <request/SignalRequest.h>

#include "ProtocolSignal.h"

namespace Cynara {

ProtocolSignal::ProtocolSignal() {
}

ProtocolSignal::~ProtocolSignal() {
}

void ProtocolSignal::execute(RequestContextPtr context UNUSED, SignalRequestPtr request UNUSED) {
    throw NotImplementedException();
}

ProtocolPtr ProtocolSignal::clone(void) {
    return std::make_shared<ProtocolSignal>();
}

RequestPtr ProtocolSignal::extractRequestFromBuffer(BinaryQueuePtr bufferQueue) {
    if (bufferQueue->size() >= sizeof(struct signalfd_siginfo)) {
        struct signalfd_siginfo sigInfo;
        bufferQueue->flattenConsume(&sigInfo, sizeof(sigInfo));
        return std::make_shared<SignalRequest>(sigInfo);
    }

    return nullptr;
}

ResponsePtr ProtocolSignal::extractResponseFromBuffer(BinaryQueuePtr bufferQueue UNUSED) {
    throw NotImplementedException();
}

} // namespace Cynara
