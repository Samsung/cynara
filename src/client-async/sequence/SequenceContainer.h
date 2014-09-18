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
 * @file        src/client-async/sequence/SequenceContainer.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains declaration of SequenceContainer class for
 *              storing check requests identifiers in libcynara-client-async
 */

#ifndef SRC_CLIENT_ASYNC_SEQUENCE_SEQUENCECONTAINER_H_
#define SRC_CLIENT_ASYNC_SEQUENCE_SEQUENCECONTAINER_H_

#include <vector>

#include <types/ProtocolFields.h>

namespace Cynara {

class SequenceContainer {
public:
    SequenceContainer();
    ~SequenceContainer() {}

    bool get(ProtocolFrameSequenceNumber &sequenceNumber);
    bool release(ProtocolFrameSequenceNumber sequenceNumber);
    void clear(void);

private:
    std::vector<int> m_sequenceVector;
};

} // namespace Cynara

#endif // SRC_CLIENT_ASYNC_SEQUENCE_SEQUENCECONTAINER_H_
