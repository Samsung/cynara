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
 * @file        Policy.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines Policy type - used to describe single policy
                rule identified by policy key
 */

#ifndef CYNARA_COMMON_TYPES_POLICY_H
#define CYNARA_COMMON_TYPES_POLICY_H

#include "PolicyKey.h"
#include "PolicyResult.h"
#include "types/pointers.h"
#include "types/PolicyType.h"
#include "types/PolicyBucketId.h"

#include <memory>
#include <tuple>

namespace Cynara {

class Policy
{
public:
    Policy(const PolicyKey &key, const PolicyResult &result) :
        m_key(key), m_result(result) {}

    static PolicyPtr simpleWithKey(const PolicyKey &key, const PolicyType &type) {
        return std::make_shared<Policy>(key, PolicyResult(type));
    }

    static PolicyPtr bucketWithKey(const PolicyKey &key, const PolicyBucketId &bucketId) {
        auto result = PolicyResult(PredefinedPolicyType::BUCKET, bucketId);
        return std::make_shared<Policy>(key, result);
    }

    bool operator==(const Policy &other) const {
        return std::tie(m_key, m_result) == std::tie(other.m_key, other.m_result);
    }

private:
    PolicyKey m_key;
    PolicyResult m_result;

public:
    const PolicyKey &key() const {
        return m_key;
    }

    const PolicyResult &result() const {
        return m_result;
    }
};

} // namespace Cynara

#endif /* CYNARA_COMMON_TYPES_POLICY_H */
