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
 * @file        ProtocolFrameHeader.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of protocol frame header (de)serializer class.
 */

#include <stdio.h>
#include <string.h>

#include "ProtocolFrameHeader.h"

namespace Cynara {

const ProtocolFrameSignature ProtocolFrameHeader::m_signature = "CPv1";

ProtocolFrameHeader::ProtocolFrameHeader(BinaryQueuePtr headerContent) :
        m_frameHeaderContent(headerContent), m_frameLength(0), m_sequenceNumber(0),
        m_headerComplete(false), m_bodyComplete(false) {
}

void ProtocolFrameHeader::read(size_t num, void *bytes) {
    m_frameHeaderContent->flattenConsume(bytes, num);
}

void ProtocolFrameHeader::write(size_t num, const void *bytes) {
    m_frameHeaderContent->appendCopy(bytes, num);
}

} /* namespace Cynara */
