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
 * @file        src/client-common/cache/CapacityCache.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains capacity cache header.
 */

#ifndef  SRC_CLIENT_COMMON_CACHE_CAPACITYCACHE_H_
#define  SRC_CLIENT_COMMON_CACHE_CAPACITYCACHE_H_

#include <list>
#include <unordered_map>

#include <cache/CacheInterface.h>

namespace Cynara {

class CapacityCache : public PluginCache {
public:
    static const std::size_t CACHE_DEFAULT_CAPACITY = 10000;

    CapacityCache(std::size_t capacity = CACHE_DEFAULT_CAPACITY) :
        m_capacity(capacity) {}

    int get(const ClientSession &session, const PolicyKey &key);
    int update(const ClientSession& session,
               const PolicyKey &key,
               const PolicyResult &result);
    void clear(void);

private:
    typedef std::list<std::string> KeyUsageList;
    typedef std::unordered_map<std::string,
        std::pair<PolicyResult,
                  KeyUsageList::iterator>> KeyValueMap;

    static std::string keyToString(const PolicyKey &key);
    void evict(void);


    std::size_t m_capacity;
    ClientSession m_session;

    KeyUsageList m_keyUsage;
    KeyValueMap m_keyValue;
};

} //namespace Cynara

#endif // SRC_CLIENT_COMMON_CACHE_CAPACITYCACHE_H_




