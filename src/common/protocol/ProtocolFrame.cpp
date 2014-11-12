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
 * @file        src/common/protocol/ProtocolFrame.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of ProtocolFrame class.
 */

#include <string.h>

#include "ProtocolFrame.h"

namespace Cynara {

ProtocolFrame::ProtocolFrame(ProtocolFrameHeader frameHeader, BinaryQueuePtr data) :
        m_frameHeader(frameHeader), m_frameBodyContent(data) {
}

void ProtocolFrame::read(size_t num, void *bytes) {
    m_frameBodyContent->flattenConsume(bytes, num);
}

void ProtocolFrame::write(size_t num, const void *bytes) {
    m_frameBodyContent->appendCopy(bytes, num);
    m_frameHeader.increaseFrameLength(num);
}

} /* namespace Cynara */
