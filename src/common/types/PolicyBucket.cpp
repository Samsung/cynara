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

#include <cstring>
#include <sstream>

#include <exceptions/InvalidBucketIdException.h>
#include <types/PolicyCollection.h>
#include <types/PolicyKeyHelpers.h>

#include "PolicyBucket.h"

namespace Cynara {

const char PolicyBucket::m_idSeparators[] = "-_";

PolicyBucket::PolicyBucket(const PolicyBucketId &id, const PolicyResult &defaultPolicy)
    : m_defaultPolicy(defaultPolicy), m_id(id) {
    idValidator(id);
}

PolicyBucket::PolicyBucket(const PolicyBucketId &id, const PolicyCollection &policies)
    : m_policyCollection(makePolicyMap(policies)), m_defaultPolicy(PredefinedPolicyType::DENY),
      m_id(id) {
    idValidator(id);
}

PolicyBucket::PolicyBucket(const PolicyBucketId &id, const PolicyResult &defaultPolicy,
                           const PolicyCollection &policies)
    : m_policyCollection(makePolicyMap(policies)), m_defaultPolicy(defaultPolicy), m_id(id) {
    idValidator(id);
}

PolicyBucket PolicyBucket::filtered(const PolicyKey &key) const {
    PolicyBucket result(m_id + "_filtered");

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

PolicyBucket::Policies PolicyBucket::listPolicies(const PolicyKey &filter) const {
    PolicyBucket::Policies policies;
    for (auto iter = m_policyCollection.begin(); iter != m_policyCollection.end(); ++iter) {
        auto &policyPtr = iter->second;
        if (policyPtr->key().matchFilter(filter))
            policies.push_back(*policyPtr);
    }
    return policies;
}

PolicyBucket::BucketIds PolicyBucket::getSubBuckets(void) const {
    PolicyBucket::BucketIds buckets;
    for (auto iter = m_policyCollection.begin(); iter != m_policyCollection.end(); ++iter) {
        auto &policyPtr = iter->second;
        if (policyPtr->result().policyType() == PredefinedPolicyType::BUCKET)
            buckets.insert(policyPtr->result().metadata());
    }
    return buckets;
}

PolicyMap PolicyBucket::makePolicyMap(const PolicyCollection &policies) {
    PolicyMap result;
    for (const auto &policy : policies) {
        const auto gluedKey = PolicyKeyHelpers::glueKey(policy->key());
        result[gluedKey] = policy;
    }
    return result;
}

void PolicyBucket::idValidator(const PolicyBucketId &id) {
    auto isCharInvalid = [] (char c) {
        return !(std::isalnum(c) || isIdSeparator(c));
    };

    if (id.end() != find_if(id.begin(), id.end(), isCharInvalid)) {
        throw InvalidBucketIdException(id);
    }
}

bool PolicyBucket::isIdSeparator(char c) {
    return strchr(m_idSeparators, c) != nullptr;
}

}  // namespace Cynara
