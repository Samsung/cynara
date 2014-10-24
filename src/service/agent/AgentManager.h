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
 * @file        src/service/agent/AgentManager.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Declaration of AgentManager class
 */

#ifndef SRC_SERVICE_AGENT_AGENTMANAGER_H_
#define SRC_SERVICE_AGENT_AGENTMANAGER_H_

#include <functional>
#include <map>

#include <containers/BinaryQueue.h>
#include <response/AgentRegisterResponse.h>
#include <types/Agent.h>
#include <types/Link.h>
#include <types/ProtocolFields.h>

#include <agent/AgentTalker.h>

namespace Cynara {

class AgentManager {
public:
    typedef std::map<ProtocolFrameSequenceNumber, AgentTalkerPtr> Talkers;
    typedef std::function<void(const AgentTalkerPtr &agentTalkerPtr)> TalkerCleanupFunction;

    AgentManager() : m_sequenceNumber(0) {}
    ~AgentManager() {}

    typedef enum {
        RR_DONE,
        RR_REJECTED,
        RR_ERROR
    } RegisterResult;

    AgentRegisterResponse::Code registerAgent(const AgentType &agentType, const LinkId &linkId);

    AgentTalkerPtr createTalker(const AgentType &agentType);
    void removeTalker(const AgentTalkerPtr &agentTalkerPtr);
    AgentTalkerPtr getTalker(const LinkId &linkId, ProtocolFrameSequenceNumber requestId) const;
    void cleanupAgent(const LinkId &linkId, TalkerCleanupFunction cleanupFunction);

private:
    std::map<AgentType, LinkId> m_agents;
    std::map<LinkId, Talkers> m_talkers;
    ProtocolFrameSequenceNumber m_sequenceNumber;

    ProtocolFrameSequenceNumber generateSequenceNumber(const AgentType &agentType);
    bool getAgentType(const LinkId &linkId, AgentType &agentType) const;
    void unregisterAgent(const LinkId &linkId);
};

} // namespace Cynara

#endif /* SRC_SERVICE_AGENT_AGENTMANAGER_H_ */
