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
 * @brief       This file contains monitor cache implementation
 */

#include <ctime>

#include <cynara-error.h>
#include <log/log.h>

#include "MonitorCache.h"

namespace Cynara {

MonitorCache::MonitorCache(ClockFunction clockFunction)
        : m_clockFunction(clockFunction) {}

void MonitorCache::update(const PolicyKey &policyKey, int result) {
    struct timespec ts;
    // https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=da15cfdae03351c689736f8d142618592e3cebc3
    auto ret = m_clockFunction(CLOCK_REALTIME_COARSE, &ts);
    if (ret != 0) {
        LOGE("Could not update monitor entries. clock_gettime() failed with [%d]", ret);
        // TODO: Decide what to do. Something very bad must have happened, if clock_gettime() failed
        return;
    }

    // We trust the client to deny access on errors
    if (result != CYNARA_API_ACCESS_ALLOWED)
        result = CYNARA_API_ACCESS_DENIED;

    m_monitorEntries.push_back({ policyKey, result, ts });
    m_cacheAgeSec = ts.tv_sec - m_monitorEntries.at(0).timestamp().tv_sec;
}

void MonitorCache::clear() {
    m_monitorEntries.clear();
    m_cacheAgeSec = 0;
}

bool MonitorCache::shouldFlush() {
    if (m_monitorEntries.size() == 0)
        return false;
    return m_monitorEntries.size() >= CACHE_CAPACITY || m_cacheAgeSec >= MAX_LOG_AGE_SEC;
}

} // namespace Cynara