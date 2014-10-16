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
 * @file        src/client-common/cache/CacheInterface.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains cache interface definitions.
 */

#ifndef SRC_CLIENT_COMMON_CACHE_CACHEINTERFACE_H_
#define SRC_CLIENT_COMMON_CACHE_CACHEINTERFACE_H_

#include <map>
#include <memory>
#include <string>

#include <cynara-client.h>
#include <cynara-client-plugin.h>
#include <types/ClientSession.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

namespace Cynara {

class PluginCache;
typedef std::shared_ptr<PluginCache> PluginCachePtr;

class PluginCache {
public:
    PluginCache() {}
    virtual int get(const ClientSession &session, const PolicyKey &key) = 0;
    virtual int update(const ClientSession &session,
                       const PolicyKey &key,
                       const PolicyResult &result) = 0;

    void registerPlugin(const PolicyType policyType, ClientPluginInterfacePtr plugin) {
        m_plugins[policyType] = plugin;
    }

    virtual void clear(void) {
        m_plugins.clear();
    }

    virtual ~PluginCache() {};

protected:
    std::map<PolicyType, ClientPluginInterfacePtr> m_plugins;
};

} // namespace Cynara

#endif // SRC_CLIENT_COMMON_CACHE_CACHEINTERFACE_H_
