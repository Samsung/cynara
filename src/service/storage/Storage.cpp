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
 * @file        Storage.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements policy rules storage procedures
 */

#include "Storage.h"
#include "StorageBackend.h"
#include "types/pointers.h"
#include "types/PolicyType.h"
#include "exceptions/NotImplementedException.h"
#include "exceptions/DefaultBucketDeletionException.h"

#include <iostream>
#include <memory>


namespace Cynara {

PolicyResult Storage::checkPolicy(const PolicyKey &key) {
    auto policies = m_backend.searchDefaultBucket(key);
    return minimalPolicy(policies, key);
};

PolicyResult Storage::minimalPolicy(const PolicyBucket &bucket, const PolicyKey &key) {
    bool hasMinimal = false;
    PolicyResult minimal = bucket.defaultPolicy();

    const auto &policies = bucket.policyCollection();

    auto proposeMinimal = [&minimal, &hasMinimal](const PolicyResult &candidate) {
        if (hasMinimal == false) {
            minimal = candidate;
        } else if (candidate < minimal) {
            minimal = candidate;
        }
        hasMinimal = true;
    };

    for (const auto &policyRecord : policies) {
        const auto &policyResult = policyRecord->result();

        switch (policyResult.policyType()) {
        case PredefinedPolicyType::DENY:
            return policyResult; // Do not expect lower value than DENY
            break;
        case PredefinedPolicyType::BUCKET: {
                auto bucketResults = m_backend.searchBucket(policyResult.metadata(), key);
                auto minimumOfBucket = minimalPolicy(bucketResults, key);
                proposeMinimal(minimumOfBucket);
                continue;
            }
            break;
        case PredefinedPolicyType::ALLOW:
        default:
            break;
        }

        proposeMinimal(policyResult);
    }

    return minimal;
}

//todo to be removed, after tests get updated
void Storage::insertPolicies(const std::vector<PolicyPolicyBucket> &policies) {
    for (const auto &policyTuple : policies) {
        PolicyBucketId bucketId;
        PolicyPtr policyPtr;
        std::tie(policyPtr, bucketId) = policyTuple;
        auto existingPolicies = m_backend.searchBucket(bucketId, policyPtr->key());
        for (auto existingPolicy : existingPolicies.policyCollection()) {
            m_backend.deletePolicy(bucketId, existingPolicy->key());
        }
        m_backend.insertPolicy(bucketId, policyPtr);
    }
}

void Storage::insertPolicies(const std::map<PolicyBucketId, std::vector<Policy>> &policies) {
    for (const auto &bucket : policies) {
        const PolicyBucketId &bucketId = bucket.first;
        for (const auto &policy : bucket.second) {
                PolicyPtr policyPtr = std::make_shared<Policy>(policy);
                auto existingPolicies = m_backend.searchBucket(bucketId, policyPtr->key());
                for (auto existingPolicy : existingPolicies.policyCollection()) {
                        m_backend.deletePolicy(bucketId, existingPolicy->key());
                }
                m_backend.insertPolicy(bucketId, policyPtr);
        }
    }
}

void Storage::addOrUpdateBucket(const PolicyBucketId &bucketId, const PolicyResult &defaultBucketPolicy) {
    if (m_backend.hasBucket(bucketId)) {
        m_backend.updateBucket(bucketId, defaultBucketPolicy);
    } else {
        m_backend.createBucket(bucketId, defaultBucketPolicy);
    }
}

void Storage::deleteBucket(const PolicyBucketId &bucketId) {
    // TODO: Check if bucket exists

    if (bucketId == defaultPolicyBucketId) {
        throw DefaultBucketDeletionException();
    }

    m_backend.deleteLinking(bucketId);
    m_backend.deleteBucket(bucketId);
}

//todo to be removed, after tests get updated
void Storage::deletePolicies(const std::vector<PolicyKeyBucket> &policies) {
    for (const auto &policy : policies) {
        m_backend.deletePolicy(std::get<1>(policy), std::get<0>(policy));
    }
}

void Storage::deletePolicies(const std::map<PolicyBucketId, std::vector<PolicyKey>> &policies) {
    for (const auto &bucket : policies) {
        const PolicyBucketId &bucketId = bucket.first;
        for (const auto &policyKey : bucket.second) {
            m_backend.deletePolicy(bucketId, policyKey);
        }
    }
}

} // namespace Cynara
