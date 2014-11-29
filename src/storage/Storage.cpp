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
 * @file        src/storage/Storage.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file implements policy rules storage procedures
 */

#include <memory>
#include <vector>

#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DefaultBucketDeletionException.h>
#include <exceptions/DefaultBucketSetNoneException.h>
#include <types/pointers.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyCollection.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include "Storage.h"

namespace Cynara {

PolicyResult Storage::checkPolicy(const PolicyKey &key,
                                  const PolicyBucketId &startBucketId /*= defaultPolicyBucketId*/,
                                  bool recursive /*= true*/) {
    auto policies = m_backend.searchBucket(startBucketId, key);
    return minimalPolicy(policies, key, recursive);
};

PolicyResult Storage::minimalPolicy(const PolicyBucket &bucket, const PolicyKey &key,
                                    bool recursive) {
    bool hasMinimal = false;
    PolicyResult minimal = bucket.defaultPolicy();

    auto proposeMinimal = [&minimal, &hasMinimal](const PolicyResult &candidate) {
        if (hasMinimal == false) {
            minimal = candidate;
        } else if (candidate < minimal) {
            minimal = candidate;
        }
        hasMinimal = true;
    };

    for (const auto policy : bucket) {
        const auto &policyResult = policy->result();

        switch (policyResult.policyType()) {
            case PredefinedPolicyType::DENY:
                return policyResult; // Do not expect lower value than DENY
            case PredefinedPolicyType::BUCKET: {
                    if (recursive == true) {
                        auto bucketResults = m_backend.searchBucket(policyResult.metadata(), key);
                        auto minimumOfBucket = minimalPolicy(bucketResults, key, true);
                        if (minimumOfBucket != PredefinedPolicyType::NONE) {
                            proposeMinimal(minimumOfBucket);
                        }
                    }
                    continue;
                }
            case PredefinedPolicyType::ALLOW:
            default:
                break;
        }

        proposeMinimal(policyResult);
    }

    return minimal;
}

void Storage::insertPolicies(const std::map<PolicyBucketId, std::vector<Policy>> &policiesByBucketId) {

    auto pointedBucketExists = [this] (const Policy &policy) -> void {
        if (policy.result().policyType() == PredefinedPolicyType::BUCKET) {
            const auto &bucketId = policy.result().metadata();
            if (m_backend.hasBucket(bucketId) == false) {
                throw BucketNotExistsException(bucketId);
            }
        }
    };

    // TODO: Rewrite, when transactions are supported
    // Check if all of buckets exist
    for (const auto &group : policiesByBucketId) {
        const auto &bucketId = group.first;
        const auto &policies = group.second;

        if (m_backend.hasBucket(bucketId) == false) {
            throw BucketNotExistsException(bucketId);
        }

        std::for_each(policies.cbegin(), policies.cend(), pointedBucketExists);
    }

    // Then insert policies
    for (const auto &group : policiesByBucketId) {
        const PolicyBucketId &bucketId = group.first;
        const auto &policies = group.second;
        for (const auto &policy : policies) {
            m_backend.insertPolicy(bucketId, std::make_shared<Policy>(policy));
        }
    }
}

void Storage::addOrUpdateBucket(const PolicyBucketId &bucketId,
                                const PolicyResult &defaultBucketPolicy) {

    if (bucketId == defaultPolicyBucketId && defaultBucketPolicy == PredefinedPolicyType::NONE)
        throw DefaultBucketSetNoneException();

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

void Storage::deletePolicies(const std::map<PolicyBucketId, std::vector<PolicyKey>> &keysByBucketId) {
    for (const auto &bucket : keysByBucketId) {
        const PolicyBucketId &bucketId = bucket.first;
        for (const auto &policyKey : bucket.second) {
            m_backend.deletePolicy(bucketId, policyKey);
        }
    }
}

PolicyBucket::Policies Storage::listPolicies(const PolicyBucketId &bucketId,
                                             const PolicyKey &filter) const {
    return m_backend.listPolicies(bucketId, filter);
}

void Storage::load(void) {
    m_backend.load();
}

void Storage::save(void) {
    m_backend.save();
}

} // namespace Cynara
