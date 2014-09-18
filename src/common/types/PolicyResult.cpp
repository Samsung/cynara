/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/types/PolicyResult.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       PolicyResult implementation
 */

#include "PolicyResult.h"

namespace Cynara {

PolicyResult::PolicyResult() : m_type(PredefinedPolicyType::DENY) {}

PolicyResult::PolicyResult(const PolicyType &policyType) : m_type(policyType) {}

PolicyResult::PolicyResult(const PolicyType &policyType, const PolicyMetadata &metadata)
    : m_type(policyType) , m_metadata(metadata) {}

const PolicyType &PolicyResult::policyType(void) const {
    return m_type;
}

const PolicyResult::PolicyMetadata &PolicyResult::metadata(void) const {
    return m_metadata;
}

bool PolicyResult::operator <(const PolicyResult &other) const {
    return this->m_type < other.m_type;
}

bool PolicyResult::operator ==(const PolicyResult &other) const {
    return std::tie(m_type, m_metadata) == std::tie(other.m_type, other.m_metadata);
}

bool PolicyResult::operator !=(const PolicyResult &other) const {
    return !(*this == other);
}

bool PolicyResult::operator ==(const PolicyType &policyType) const {
    return (m_type == policyType) && m_metadata.empty();
}

bool PolicyResult::operator !=(const PolicyType &policyType) const {
    return !(*this == policyType);
}

} // namespace Cynara

