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
 * @file        src/service/agent/AgentTalker.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Declaration of AgentTalker class
 */

#ifndef SRC_SERVICE_AGENT_AGENTTALKER_H_
#define SRC_SERVICE_AGENT_AGENTTALKER_H_

#include <memory>

#include <containers/RawBuffer.h>
#include <types/Agent.h>
#include <types/Link.h>
#include <types/ProtocolFields.h>

#include <cynara-plugin.h>

namespace Cynara {

class AgentTalker {
public:
    AgentTalker(const AgentType &agentType, const LinkId &linkId,
                const ProtocolFrameSequenceNumber checkId) : m_agentType(agentType),
                    m_checkId(checkId), m_linkId (linkId) {}
    ~AgentTalker() {}

    void send(const PluginData &agentData);
    void cancel(void);

    const AgentType &agentType(void) const {
        return m_agentType;
    }

    const LinkId linkId(void) const {
        return m_linkId;
    }

    ProtocolFrameSequenceNumber checkId(void) const {
        return m_checkId;
    }

private:
    const AgentType m_agentType;
    const ProtocolFrameSequenceNumber m_checkId;
    const LinkId m_linkId;

    void sendMessage(const AgentResponseType type, const RawBuffer &data);
};

typedef std::shared_ptr<AgentTalker> AgentTalkerPtr;

} // namespace Cynara

#endif /* SRC_SERVICE_AGENT_AGENTTALKER_H_ */
