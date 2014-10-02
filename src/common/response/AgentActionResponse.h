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
 * @file        src/common/response/AgentActionResponse.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines class for responding to agent action request
 */

#ifndef SRC_COMMON_RESPONSE_AGENTACTIONRESPONSE_H_
#define SRC_COMMON_RESPONSE_AGENTACTIONRESPONSE_H_

#include <containers/RawBuffer.h>
#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>
#include <types/Agent.h>

namespace Cynara {

class AgentActionResponse : public Response {
public:
    AgentActionResponse(const AgentResponseType type, const RawBuffer &data,
                        ProtocolFrameSequenceNumber sequenceNumber) : Response(sequenceNumber),
                        m_type(type), m_data(data) {}

    ~AgentActionResponse() {}

    virtual void execute(ResponsePtr self, ResponseTakerPtr taker, RequestContextPtr context) const;

    const RawBuffer &data(void) const {
        return m_data;
    }

    AgentResponseType type(void) const {
        return m_type;
    }

private:
    const AgentResponseType m_type;
    const RawBuffer m_data;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_AGENTACTIONRESPONSE_H_ */
