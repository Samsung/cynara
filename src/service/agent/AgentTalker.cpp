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
 * @file        src/service/agent/AgentTalker.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Definition of AgentTalker class
 */

#include <log/log.h>
#include <protocol/ProtocolAgent.h>
#include <request/RequestContext.h>
#include <response/AgentActionResponse.h>
#include <response/pointers.h>

#include <cynara-agent.h>

#include "AgentTalker.h"

namespace Cynara {

void AgentTalker::sendMessage(const AgentResponseType type, const RawBuffer &data) {
    ResponseTakerPtr responseTaker = std::make_shared<ProtocolAgent>();
    auto context = std::make_shared<RequestContext>(responseTaker, m_linkId);
    context->returnResponse(context,
                std::make_shared<AgentActionResponse>(type, data, m_checkId));
}

void AgentTalker::send(const PluginData &agentData) {
    RawBuffer data(agentData.begin(), agentData.end());
    sendMessage(CYNARA_MSG_TYPE_ACTION, data);
}

void AgentTalker::cancel(void) {
    RawBuffer data;
    sendMessage(CYNARA_MSG_TYPE_CANCEL, data);
}

} // namespace Cynara
