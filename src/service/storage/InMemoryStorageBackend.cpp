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
 * @file        InMemoryStorageBackend.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of InMemoryStorageBackend
 */

#include "InMemoryStorageBackend.h"

namespace Cynara {

InMemoryStorageBackend::InMemoryStorageBackend() {
    // Make sure, there's always default bucket
    this->buckets().insert({ defaultPolicyBucketId, PolicyBucket() });
}

InMemoryStorageBackend::~InMemoryStorageBackend() {}

PolicyBucket InMemoryStorageBackend::searchDefaultBucket(const PolicyKey &key) {
    return searchBucket(defaultPolicyBucketId, key);
}

PolicyBucket InMemoryStorageBackend::searchBucket(const PolicyBucketId &bucketId,
                                                  const PolicyKey &key) {
    const auto &bucket = this->buckets().at(bucketId);
    return bucket.filtered(key);
}

void InMemoryStorageBackend::insertPolicy(const PolicyBucketId &bucketId, PolicyPtr policy) {
    try {
        auto &bucket = buckets().at(bucketId);
        auto &policies = bucket.policyCollection();
        policies.push_back(policy);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::createBucket(const PolicyBucketId &bucketId,
                                          const PolicyResult &defaultPolicy) {
    PolicyBucket newBucket;
    newBucket.setDefaultPolicy(defaultPolicy);
    buckets().insert({ bucketId, newBucket });
}

void InMemoryStorageBackend::deleteBucket(const PolicyBucketId &bucketId) {
    auto bucketErased = buckets().erase(bucketId);
    if (bucketErased == 0) {
        throw BucketNotExistsException(bucketId);
    }
}

bool InMemoryStorageBackend::hasBucket(const PolicyBucketId &bucketId) {
    return buckets().find(bucketId) != buckets().end();
}

void InMemoryStorageBackend::deletePolicy(const PolicyBucketId &bucketId, const PolicyKey &key) {
    try {
        // TODO: Move the erase code to PolicyCollection maybe?
        auto &bucket = buckets().at(bucketId);
        auto &policies = bucket.policyCollection();
        policies.erase(remove_if(policies.begin(), policies.end(),
                [key](PolicyPtr policy) -> bool {
                    return policy->key() == key;
            }), policies.end());
    } catch(const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::deleteLinking(const PolicyBucketId &bucketId) {
    auto bucketIdMatches = [&bucketId] (PolicyPtr policy) -> bool {
        auto policyResult = policy->result();

        // Check bucket id only if policy is a bucket policy
        // TODO: Maybe move the test to PolicyResult
        if (policyResult.policyType() == PredefinedPolicyType::BUCKET) {
            return policyResult.metadata() == bucketId;
        }
        return false;
    };

    for(auto &bucketIter : buckets()) {
        // TODO: Move the erase code to PolicyCollection maybe?
        auto &bucket = bucketIter.second;
        auto &policies = bucket.policyCollection();
        policies.erase(remove_if(policies.begin(), policies.end(), bucketIdMatches),
                policies.end());
    }
}

} /* namespace Cynara */
