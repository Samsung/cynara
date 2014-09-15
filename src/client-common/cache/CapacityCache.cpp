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
 * @file        CapacityCache.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains capacity cache implementation.
 */

#include <cinttypes>

#include <cynara-client-error.h>
#include <log/log.h>

#include <cache/CapacityCache.h>

namespace Cynara {

int CapacityCache::get(const ClientSession &session, const PolicyKey &key) {
    //This can be very time heavy. This part is welcomed to be optimized.
    if (session != m_session) {
        LOGD("Session changed from %s to %s.", m_session.c_str(), session.c_str());
        clear();
        m_session = session;
        return CYNARA_API_CACHE_MISS;
    }
    auto resultIt = m_keyValue.find(keyToString(key));
    //Do we have entry in cache?
    if (resultIt == m_keyValue.end()) {
        LOGD("No entry for client=%s user=%s privilege=%s.",
                key.client().toString().c_str(),
                key.user().toString().c_str(),
                key.privilege().toString().c_str());
        return CYNARA_API_CACHE_MISS;
    } else {
        LOGD("Entry available for client=%s user=%s privilege=%s",
                key.client().toString().c_str(),
                key.user().toString().c_str(),
                key.privilege().toString().c_str());

        auto pluginIt = m_plugins.find(resultIt->second.first.policyType());
        if (pluginIt == m_plugins.end()) {
            LOGE("No plugin registered for given PolicyType : %" PRIu16,
                    resultIt->second.first.policyType());
            return CYNARA_API_ACCESS_DENIED;
        }

        //Is it still usable?
        InterpreterInterfacePtr plugin = pluginIt->second;
        if (plugin->isUsable(resultIt->second.first)) {
            LOGD("Entry usable.");
            m_keyUsage.splice(m_keyUsage.begin(), m_keyUsage, resultIt->second.second);
            return plugin->toResult(resultIt->second.first);
        }
        //Remove unusable entry
        LOGD("Entry not usable");
        auto usageIt = resultIt->second.second;
        m_keyUsage.erase(usageIt);
        m_keyValue.erase(resultIt);
        return CYNARA_API_CACHE_MISS;
    }
}

void CapacityCache::clear(void) {
    m_keyUsage.clear();
    m_keyValue.clear();
    m_session.clear();
}

std::string CapacityCache::keyToString(const PolicyKey &key) {
    const char separator = '\1';
    auto clientStr = key.client().toString();
    auto privilegeStr = key.privilege().toString();
    auto userStr = key.user().toString();
    return clientStr + privilegeStr + userStr + separator +
            std::to_string(clientStr.size()) + separator +
            std::to_string(privilegeStr.size()) + separator +
            std::to_string(userStr.size());
}

void CapacityCache::evict(void) {

    auto lastUsedKey = m_keyUsage.back();
    m_keyUsage.pop_back();

    auto value_it = m_keyValue.find(lastUsedKey);
    m_keyValue.erase(value_it);
}

int CapacityCache::update(const ClientSession &session,
                          const PolicyKey &key,
                          const PolicyResult &result) {
    //This can be very time heavy. This part is welcomed to be optimized.
    if (session != m_session) {
        LOGD("Session changed from %s to %s.", m_session.c_str(), session.c_str());
        clear();
        m_session = session;
    }

    auto pluginIt = m_plugins.find(result.policyType());

    //No registered plugin for returned type of policy
    if (pluginIt == m_plugins.end()) {
        LOGE("No registered plugin for given PolicyType: %" PRIu16,
                result.policyType());
        return CYNARA_API_ACCESS_DENIED;
    }
    auto plugin = pluginIt->second;

    if (m_capacity != 0) {
        if (plugin->isCacheable(result)) {
            LOGD("Entry cacheable");
            if (m_keyValue.size() == m_capacity) {
                LOGD("Capacity reached.");
                evict();
            }
            m_keyUsage.push_front(keyToString(key));
            m_keyValue[keyToString(key)] = std::make_pair(result, m_keyUsage.begin());
        }
    }
    return plugin->toResult(result);
}

} // namespace Cynara
