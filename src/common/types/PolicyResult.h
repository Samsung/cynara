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
/*
 * @file        PolicyResult.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Definitions of PolicyResult and friends
 */

#ifndef POLICYRESULT_H_
#define POLICYRESULT_H_

#include "types/PolicyType.h"

namespace Cynara {

class PolicyResult {
public:
    typedef std::string PolicyMetadata;

public:
    PolicyResult(const PolicyType &policyType) : m_type(policyType) {}
    PolicyResult(const PolicyType &policyType, const PolicyMetadata &metadata)
        : m_type(policyType), m_metaData(metadata) {}

private:
    PolicyType m_type;
    PolicyMetadata m_metaData;

public:
    PolicyType policyType() const { return m_type; }
    const PolicyMetadata& metaData() const { return m_metaData; }

    bool operator <(const PolicyResult& other) const {
        return this->m_type < other.m_type;
    }
};

} // namespace Cynara


#endif /* POLICYRESULT_H_ */
