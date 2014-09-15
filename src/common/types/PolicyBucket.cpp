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
 * @file        src/common/types/PolicyBucket.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of Cynara::PolicyBucket methods
 */

#include <sstream>

#include <types/PolicyCollection.h>
#include <types/PolicyKeyHelpers.h>

#include "PolicyBucket.h"

namespace Cynara {

PolicyBucket PolicyBucket::filtered(const PolicyKey &key) const {
    PolicyBucket result;

    const auto &policies = m_policyCollection;
    const auto variants = PolicyKeyHelpers::keyVariants(key);

    for (const auto &variant : variants) {
        const auto policyIter = policies.find(variant);
        if (policyIter != policies.end()) {
            result.m_policyCollection[policyIter->first] = policyIter->second;
        }
    }

    // Inherit original policy
    result.setDefaultPolicy(defaultPolicy());
    return result;
}

void PolicyBucket::insertPolicy(PolicyPtr policy) {
    const auto gluedKey = PolicyKeyHelpers::glueKey(policy->key());
    m_policyCollection[gluedKey] = policy;
}

void PolicyBucket::deletePolicy(const PolicyKey &key) {
    const auto gluedKey = PolicyKeyHelpers::glueKey(key);
    m_policyCollection.erase(gluedKey);
}

void PolicyBucket::deletePolicy(std::function<bool(PolicyPtr)> predicate) {
    auto &policies = m_policyCollection;

    for (auto iter = policies.begin(); iter != policies.end(); ) {
        if (predicate(iter->second)) {
            policies.erase(iter++);
        } else {
            ++iter;
        }
    }
}

PolicyMap PolicyBucket::makePolicyMap(const PolicyCollection &policies) {
    PolicyMap result;
    for (const auto &policy : policies) {
        const auto gluedKey = PolicyKeyHelpers::glueKey(policy->key());
        result[gluedKey] = policy;
    }
    return result;
}

}  // namespace Cynara
