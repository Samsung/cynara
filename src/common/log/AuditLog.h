/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/log/AuditLog.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines privilege check logging utility.
 */

#ifndef SRC_COMMON_LOG_AUDITLOG_H
#define SRC_COMMON_LOG_AUDITLOG_H

#include <string>

#include <types/Policy.h>

namespace Cynara {

class AuditLog {
public:
    AuditLog();

    void log(const PolicyKey &policyKey, const PolicyResult &policyResult);

private:
    typedef enum {
        AL_NONE,
        AL_DENY,
        AL_ALLOW,
        AL_OTHER,
        AL_ALL
    } AuditLevel;

    int m_logLevel;

    void init(void);
    static AuditLevel stringToLevel(const std::string &name);
    static const char *policyResultToString(const PolicyResult &policyResult);
};

#endif /* SRC_COMMON_LOG_AUDITLOG_H */

} // namespace Cynara
