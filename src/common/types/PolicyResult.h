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
 * @file        PolicyResult.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Definitions of PolicyResult and friends
 */

#ifndef POLICYRESULT_H_
#define POLICYRESULT_H_

#include "types/PolicyType.h"

#include <tuple>

namespace Cynara {

class PolicyResult {
public:
    typedef std::string PolicyMetadata;

public:
    PolicyResult() : m_type(PredefinedPolicyType::DENY) {}
    PolicyResult(const PolicyType &policyType) : m_type(policyType) {}
    PolicyResult(const PolicyType &policyType, const PolicyMetadata &metadata)
        : m_type(policyType), m_metadata(metadata) {}

private:
    PolicyType m_type;
    PolicyMetadata m_metadata;

public:
    const PolicyType &policyType() const {
        return m_type;
    }

    const PolicyMetadata& metadata() const {
        return m_metadata;
    }

    bool operator <(const PolicyResult &other) const {
        return this->m_type < other.m_type;
    }

    bool operator ==(const PolicyResult &other) const {
        return std::tie(m_type, m_metadata) == std::tie(other.m_type, other.m_metadata);
    }

    bool operator !=(const PolicyResult &other) const {
        return !(*this == other);
    }

    bool operator ==(const PolicyType &policyType) const {
        return (m_type == policyType) && m_metadata.empty();
    }

    bool operator !=(const PolicyType &policyType) const {
        return !(*this == policyType);
    }
};

} // namespace Cynara


#endif /* POLICYRESULT_H_ */
