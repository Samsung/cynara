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
 * @file        src/cyad/PolicyTypeTranslator.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       A helper class to translate strings to PolicyType
 */

#include <algorithm>
#include <stdexcept>

#include <cynara-error.h>

#include <cyad/CommandlineParser/PolicyParsingException.h>

#include "PolicyTypeTranslator.h"

namespace Cynara {

namespace {
    const std::string BUCKET_LITERAL = "bucket";
    const std::string NONE_LITERAL = "none";
} /* namespace anonymous */

PolicyTypeTranslator::PolicyTypeTranslator() {}

PolicyTypeTranslator::~PolicyTypeTranslator() {}

void PolicyTypeTranslator::setDescriptions(cynara_admin_policy_descr **descs) {
    for (int i = 0; descs[i]; ++i) {
        std::string name(descs[i]->name);
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        m_descs.push_back(std::make_pair(descs[i]->result, name));
    }
}

PolicyType PolicyTypeTranslator::translate(const std::string &rawPolicy) {

    std::string policy(rawPolicy);
    std::transform(policy.begin(), policy.end(), policy.begin(), ::tolower);

    auto descComp = [policy] (const PolicyDescriptions::value_type &it) -> bool {
        return it.second == policy;
    };

    const auto it = std::find_if(m_descs.begin(), m_descs.end(), descComp);

    if (it != m_descs.end())
        return it->first;

    if (policy == NONE_LITERAL)
        return CYNARA_ADMIN_NONE;
    if (policy == BUCKET_LITERAL)
        return CYNARA_ADMIN_BUCKET;

    try {
        return std::stoi(policy, nullptr, 0);
    } catch (const std::logic_error &) {
        throw PolicyParsingException();
    }
}

std::string PolicyTypeTranslator::humanize(int policy) {
    auto descComp = [policy] (const PolicyDescriptions::value_type &it) -> bool {
        return it.first == policy;
    };

    const auto it = std::find_if(m_descs.begin(), m_descs.end(), descComp);

    if (it != m_descs.end()) {
        return it->second;
    }

    if (policy == CYNARA_ADMIN_NONE)
        return NONE_LITERAL;
    if (policy == CYNARA_ADMIN_BUCKET)
        return BUCKET_LITERAL;

    return std::to_string(policy);
}

} /* namespace Cynara */
