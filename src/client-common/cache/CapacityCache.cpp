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
 * @file        src/client-common/cache/CapacityCache.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains capacity cache implementation.
 */

#include <cinttypes>

#include <cynara-error.h>
#include <log/log.h>

#include <cache/CapacityCache.h>

namespace Cynara {

int CapacityCache::get(const ClientSession &session, const PolicyKey &key) {
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

        auto &cachedValue = resultIt->second;
        auto &policyResult = std::get<0>(cachedValue);

        ClientPluginInterfacePtr plugin = findPlugin(policyResult.policyType());
        if (!plugin) {
            LOGE("No plugin registered for given policyType: [%" PRIu16 "]",
                 policyResult.policyType());
            return CYNARA_API_ACCESS_DENIED;
        }

        //Is it still usable?

        auto &prevSession = std::get<1>(cachedValue);
        auto usageIt = std::get<2>(cachedValue);
        bool updateSession = false;
        if (plugin->isUsable(session, prevSession, updateSession, policyResult)) {
            LOGD("Entry usable.");
            m_keyUsage.splice(m_keyUsage.begin(), m_keyUsage, usageIt);

            if (updateSession) {
                prevSession = session;
            }

            return plugin->toResult(session, policyResult);
        }
        //Remove unusable entry
        LOGD("Entry not usable");
        m_keyUsage.erase(usageIt);
        m_keyValue.erase(resultIt);
        return CYNARA_API_CACHE_MISS;
    }
}

void CapacityCache::clear(void) {
    m_keyUsage.clear();
    m_keyValue.clear();
    m_pluginManager.invalidateAll();
    for (auto &plugin : m_plugins) {
        plugin.second->invalidate();
    }
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

    ClientPluginInterfacePtr plugin = findPlugin(result.policyType());
    if (!plugin) {
        LOGE("No plugin registered for given policyType: [%" PRIu16 "]", result.policyType());
        return CYNARA_API_ACCESS_DENIED;
    }

    PolicyResult storedResult = result;

    if (m_capacity > 0) {
        std::string cacheKey = keyToString(key);
        auto resultIt = m_keyValue.find(cacheKey);
        if (plugin->isCacheable(session, storedResult)) {
            LOGD("Entry cacheable");
            if (m_keyValue.size() == m_capacity) {
                LOGD("Capacity reached.");
                evict();
            }

            //Move value usage to front
            if (resultIt != m_keyValue.end()) {
                auto usageIt = std::get<2>(resultIt->second);
                m_keyUsage.splice(m_keyUsage.begin(), m_keyUsage, usageIt);
            } else {
                m_keyUsage.push_front(cacheKey);
            }

            m_keyValue[cacheKey] = std::make_tuple(storedResult, session, m_keyUsage.begin());
        } else {
            //Remove element
            if (resultIt != m_keyValue.end()) {
                auto usageIt = std::get<2>(resultIt->second);
                m_keyUsage.erase(usageIt);
                m_keyValue.erase(resultIt);
            }
        }
    }
    return plugin->toResult(session, storedResult);
}

ClientPluginInterfacePtr CapacityCache::findPlugin(PolicyType policyType) {
    ClientPluginInterfacePtr plugin;

    auto pluginIt = m_plugins.find(policyType);
    if (pluginIt != m_plugins.end()) {
        plugin = pluginIt->second;
    } else {
        plugin = std::dynamic_pointer_cast<ClientPluginInterface>(
                                                m_pluginManager.getPlugin(policyType));
    }

    return plugin;
}

} // namespace Cynara
