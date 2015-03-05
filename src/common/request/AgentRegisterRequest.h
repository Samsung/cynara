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
 * @file        src/common/request/AgentRegisterRequest.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines agent register request class
 */

#ifndef SRC_COMMON_REQUEST_AGENTREGISTERREQUEST_H_
#define SRC_COMMON_REQUEST_AGENTREGISTERREQUEST_H_

#include <types/Agent.h>

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class AgentRegisterRequest : public Request {
public:
    AgentRegisterRequest(const AgentType &agentType, ProtocolFrameSequenceNumber sequenceNumber) :
        Request(sequenceNumber), m_agentType(agentType) {
    }

    virtual ~AgentRegisterRequest() {};

    const AgentType &agentType(void) const {
        return m_agentType;
    }

    virtual void execute(const Request &self, RequestTaker &taker,
                         const RequestContext &context) const;

private:
    AgentType m_agentType;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_AGENTREGISTERREQUEST_H_ */
