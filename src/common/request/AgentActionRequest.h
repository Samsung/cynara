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
 * @file        src/common/request/AgentActionRequest.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines agent register request class
 */

#ifndef SRC_COMMON_REQUEST_AGENTACTIONTREQUEST_H_
#define SRC_COMMON_REQUEST_AGENTACTIONTREQUEST_H_

#include <types/Agent.h>

#include <containers/RawBuffer.h>
#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class AgentActionRequest : public Request {
public:
    AgentActionRequest(const AgentRequestType type, const RawBuffer &data,
                       ProtocolFrameSequenceNumber sequenceNumber) : Request(sequenceNumber),
                       m_type(type), m_data(data) {
    }

    virtual ~AgentActionRequest() {};

    const RawBuffer &data(void) const {
        return m_data;
    }

    AgentRequestType type(void) const {
        return m_type;
    }

    virtual void execute(RequestPtr self, RequestTakerPtr taker, RequestContextPtr context) const;

private:
    const AgentRequestType m_type;
    const RawBuffer m_data;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_AGENTACTIONTREQUEST_H_ */
