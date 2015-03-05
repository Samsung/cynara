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
 * @file        src/service/request/CheckRequestManager.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Definition of CheckRequestManager class
 */

#include <cstdint>

#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <request/RequestContext.h>

#include "CheckRequestManager.h"

namespace Cynara {

CheckContextPtr CheckRequestManager::createContext(const PolicyKey &key,
                                                   const RequestContext &request,
                                                   ProtocolFrameSequenceNumber checkId,
                                                   const ServicePluginInterfacePtr &plugin,
                                                   const AgentTalkerPtr &agentTalkerPtr) {

    CheckContextPtr checkPtr = std::make_shared<CheckContext>(key, request, checkId, plugin,
                                                              agentTalkerPtr);
    if (m_checks[request.responseQueue()].insert(std::make_pair(checkId, checkPtr)).second) {
        return checkPtr;
    }

    return CheckContextPtr();
}

CheckContextPtr CheckRequestManager::getContext(const LinkId &linkId,
                                                ProtocolFrameSequenceNumber checkId) {
    const auto checkMap = m_checks.find(linkId);
    if (checkMap == m_checks.end()) {
        return CheckContextPtr();
    }

    const auto checkContext = checkMap->second.find(checkId);
    return checkContext != checkMap->second.end() ? checkContext->second : CheckContextPtr();
}

CheckContextPtr CheckRequestManager::getContext(const AgentTalkerPtr &talker) {
    for (const auto &checkMap : m_checks) {
        for (const auto &check : checkMap.second) {
            if (check.second->m_agentTalker == talker) {
                return check.second;
            }
        }
    }
    return nullptr;
}

void CheckRequestManager::removeRequest(const CheckContextPtr &checkContextPtr) {
    auto it = m_checks.find(checkContextPtr->m_requestContext.responseQueue());
    if (it != m_checks.end()) {
        it->second.erase(checkContextPtr->m_checkId);
        if (it->second.empty()) {
            m_checks.erase(it);
        }
    }
}

void CheckRequestManager::cancelRequests(const LinkId &linkId, CancelRequestFunction cancelFunction)
{
    auto checkMap = m_checks.find(linkId);
    if (checkMap == m_checks.end())
        return;

    if (cancelFunction) {
        for (auto p : checkMap->second) {
            cancelFunction(p.second);
        }
    }
}

} // namespace Cynara
