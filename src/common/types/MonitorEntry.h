/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/types/MonitorEntry.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines MonitorEntry type
 */

#ifndef SRC_COMMON_TYPES_MONITORENTRY_H_
#define SRC_COMMON_TYPES_MONITORENTRY_H_

#include <time.h>

#include "PolicyKey.h"

namespace Cynara {

class MonitorEntry {
public:
    MonitorEntry(const PolicyKey &key, const int &result, const struct timespec &timestamp)
            : m_key(key), m_result(result), m_timestamp(timestamp) {}

private:
    PolicyKey m_key;
    int m_result; // CYNARA_API_ACCESS_ALLOWED or CYNARA_API_ACCESS_DENIED
    struct timespec m_timestamp;

public:
    const PolicyKey &key(void) const {
        return m_key;
    }

    int result(void) const {
        return m_result;
    }

    const struct timespec &timestamp(void) const {
        return m_timestamp;
    }
};

} // namespace Cynara

#endif /* SRC_COMMON_TYPES_MONITORENTRY_H_ */
