/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains monitor cache header
 */

#ifndef SRC_CLIENT_COMMON_CACHE_MONITORCACHE_H_
#define SRC_CLIENT_COMMON_CACHE_MONITORCACHE_H_

#include <ctime>
#include <vector>

#include <types/MonitorEntry.h>

namespace Cynara {

class MonitorCache {
public:

    static const __time_t MAX_LOG_AGE_SEC = 120;
    static const std::size_t CACHE_CAPACITY = 100;

    void update(const PolicyKey &policyKey, int result);
    void clear();
    bool shouldFlush();

    const std::vector<MonitorEntry> &entries() const {
        return m_monitorEntries;
    }

private:
    std::vector<MonitorEntry> m_monitorEntries;
    __time_t m_cacheAgeSec;
};

} // namespace Cynara


#endif // SRC_CLIENT_COMMON_CACHE_MONITORCACHE_H_
