/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/client-async/sequence/SequenceContainer.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of SequenceContainer class for
 *              storing check requests identifiers in libcynara-client-async
 */

#include <algorithm>
#include <cinttypes>
#include <climits>
#include <cstring>

#include "SequenceContainer.h"

namespace Cynara {

SequenceContainer::SequenceContainer()
    : m_sequenceVector(((static_cast<size_t>(UINT16_MAX) + 1) / CHAR_BIT) / sizeof(int), -1) {
}

bool SequenceContainer::get(ProtocolFrameSequenceNumber &sequenceNumber) {
    for (size_t index = 0; index < m_sequenceVector.size(); ++index) {
        int pos = ffs(m_sequenceVector[index]);
        if (pos != 0) {
            sequenceNumber = static_cast<uint16_t>(index * sizeof(int) * CHAR_BIT - 1 + pos);
            m_sequenceVector[index] ^= 1 << (pos - 1);
            return true;
        }
    }
    return false;
}

bool SequenceContainer::release(ProtocolFrameSequenceNumber sequenceNumber) {
    size_t index = static_cast<size_t>(sequenceNumber) / (CHAR_BIT * sizeof(int));
    int pos = static_cast<int>(sequenceNumber) % (CHAR_BIT * sizeof(int));
    int i = m_sequenceVector[index] | 1 << pos;
    if (i == m_sequenceVector[index])
        return false;
    m_sequenceVector[index] = i;
    return true;
}

void SequenceContainer::clear(void) {
    memset(m_sequenceVector.data(), -1, m_sequenceVector.size() * sizeof(int));
}

} // namespace Cynara
