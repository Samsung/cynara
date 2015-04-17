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
 * @file        src/common/log/AuditLog.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements privilege check logging utility.
 */

#ifdef BUILD_WITH_SYSTEMD
#include <systemd/sd-journal.h>
#else
#include <syslog.h>
#endif

#include "AuditLog.h"

namespace Cynara {

AuditLog::AuditLog() : m_logLevel(AL_DENY) {
    init();
}

void AuditLog::init(void) {
    char *env_val = getenv("CYNARA_AUDIT_LEVEL");
    if (env_val) {
        m_logLevel = stringToLevel(env_val);
    }
}

AuditLog::AuditLevel AuditLog::stringToLevel(const std::string &name) {
    if (name == "NONE")
        return AL_NONE;

    if (name == "DENY")
        return AL_DENY;

    if (name == "ALLOW")
        return AL_ALLOW;

    if (name == "OTHER")
        return AL_OTHER;

    if (name == "ALL")
        return AL_ALL;

    return AL_NONE;
}

const char *AuditLog::policyResultToString(const PolicyResult &policyResult) {
    if (policyResult.policyType() == PredefinedPolicyType::DENY)
        return "DENY";

    if (policyResult.policyType() == PredefinedPolicyType::ALLOW)
        return "ALLOW";

    return "OTHER";
}

void AuditLog::log(const PolicyKey &policyKey, const PolicyResult &policyResult) {
    if (m_logLevel == AL_NONE)
        return;

    PolicyType policyType = policyResult.policyType();
    namespace PPT = PredefinedPolicyType;

    if (m_logLevel == AL_ALL || (m_logLevel == AL_DENY && policyType == PPT::DENY) ||
        (m_logLevel == AL_ALLOW && policyType == PPT::ALLOW) ||
        (m_logLevel == AL_OTHER && policyType != PPT::ALLOW && policyType != PPT::DENY)) {
#ifdef BUILD_WITH_SYSTEMD
            sd_journal_send("MESSAGE=%s;%s;%s => %s", policyKey.client().toString().c_str(),
                            policyKey.user().toString().c_str(),
                            policyKey.privilege().toString().c_str(),
                            policyResultToString(policyResult), "PRIORITY=%i", LOG_INFO,
                            "CYNARA_LOG_TYPE=AUDIT", NULL);
#else // BUILD_WITH_SYSTEMD
            syslog(LOG_INFO, "CYNARA AUDIT MESSAGE=%s;%s;%s => %s", policyKey.client().toString().c_str(),
                   policyKey.user().toString().c_str(),
                   policyKey.privilege().toString().c_str(),
                   policyResultToString(policyResult));
#endif // BUILD_WITH_SYSTEMD
    }
}

} /* namespace Cynara */
