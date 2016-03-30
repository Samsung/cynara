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
 * @file        src/monitor/logic/Logic.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class -- main libcynara-monitor class
 */


#include <mutex>

#include <cynara-error.h>
#include <log/log.h>
#include <types/MonitorEntry.h>

#include "Logic.h"

namespace Cynara {

int Logic::entriesGet(std::vector<MonitorEntry> &entries) {
    std::unique_lock<std::mutex> guard(m_mutexGuard, std::defer_lock);
    if (!guard.try_lock()) {
        LOGW("This function is not re-entrant");
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    }

    std::unique_lock<std::mutex> lock(m_mutexCond);
    m_go = false;
    m_conditionVariable.wait(lock, [&]() -> bool { return m_go; });
    entries.clear();
    return CYNARA_API_SUCCESS;
}

int Logic::entriesFlush(void) {
    std::lock_guard<std::mutex> lock(m_mutexCond);
    m_go = true;
    m_conditionVariable.notify_one();
    return CYNARA_API_SUCCESS;
}

} // namespace Cynara