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
 * @file        buckets.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests of policies in Storage
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "types/PolicyType.h"
#include "types/PolicyKey.h"
#include "types/PolicyResult.h"
#include "types/PolicyCollection.h"
#include "types/pointers.h"
#include "exceptions/DefaultBucketDeletionException.h"
#include "storage/Storage.h"
#include "storage/StorageBackend.h"


#include "fakestoragebackend.h"
#include "../../helpers.h"

#include <map>
#include <memory>
#include <tuple>
#include <vector>

using namespace Cynara;

TEST(storage, deletePolicies) {
    using ::testing::_;
    using ::testing::Eq;
    using ::testing::TypedEq;
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    auto pk1 = Helpers::generatePolicyKey("1");
    auto pk2 = Helpers::generatePolicyKey("2");
    PolicyBucketId bucketId1 = "bucket-1";
    PolicyBucketId bucketId2 = "bucket-2";

    EXPECT_CALL(backend, deletePolicy(bucketId1, pk1));
    EXPECT_CALL(backend, deletePolicy(bucketId2, pk1));
    EXPECT_CALL(backend, deletePolicy(bucketId1, pk2));

    std::map<PolicyBucketId, std::vector<PolicyKey>> policies;
    policies[bucketId1].push_back(pk1);
    policies[bucketId2].push_back(pk1);
    policies[bucketId1].push_back(pk2);

    storage.deletePolicies(policies);
}

// TODO: isn't it the same test as storage.deleteBucket?
TEST(storage, deleteBucketWithLinkedPolicies) {
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId bucketId = "test-bucket";

    EXPECT_CALL(backend, deleteLinking(bucketId));
    EXPECT_CALL(backend, deleteBucket(bucketId));

    storage.deleteBucket(bucketId);
}


TEST(storage, insertPolicies) {
    using ::testing::Return;
    using ::testing::_;
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId testBucket1 = "test-bucket-1";
    PolicyBucketId testBucket2 = "test-bucket-2";

    std::map<PolicyBucketId, std::vector<Policy>> policiesToInsert;
    policiesToInsert[testBucket1].push_back(Policy(Helpers::generatePolicyKey("1"), PredefinedPolicyType::ALLOW));
    policiesToInsert[testBucket1].push_back(Policy(Helpers::generatePolicyKey("2"), PredefinedPolicyType::DENY));
    policiesToInsert[testBucket1].push_back(Policy(Helpers::generatePolicyKey("3"), PredefinedPolicyType::DENY));
    policiesToInsert[testBucket2].push_back(Policy(Helpers::generatePolicyKey("4"), PredefinedPolicyType::ALLOW));
    policiesToInsert[testBucket2].push_back(Policy(Helpers::generatePolicyKey("5"), PredefinedPolicyType::ALLOW));

    for (const auto &bucket : policiesToInsert) {
        const PolicyBucketId &bucketId = bucket.first;
        for (const auto &policy : bucket.second) {
            EXPECT_CALL(backend, searchBucket(bucketId, policy.key()))
                .WillOnce(Return(PolicyBucket()));
            EXPECT_CALL(backend, insertPolicy(bucketId, _));
        }
    }

    storage.insertPolicies(policiesToInsert);
}

TEST(storage, updatePolicies) {
    using ::testing::Return;
    using ::testing::_;
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId testBucket1 = "test-bucket-1";
    PolicyBucketId testBucket2 = "test-bucket-2";

    std::map<PolicyBucketId, std::vector<Policy>> policiesToInsert;
    policiesToInsert[testBucket1].push_back(Policy(Helpers::generatePolicyKey("1"), PredefinedPolicyType::ALLOW));
    policiesToInsert[testBucket1].push_back(Policy(Helpers::generatePolicyKey("2"), PredefinedPolicyType::DENY));
    policiesToInsert[testBucket1].push_back(Policy(Helpers::generatePolicyKey("3"), PredefinedPolicyType::DENY));
    policiesToInsert[testBucket2].push_back(Policy(Helpers::generatePolicyKey("4"), PredefinedPolicyType::ALLOW));
    policiesToInsert[testBucket2].push_back(Policy(Helpers::generatePolicyKey("5"), PredefinedPolicyType::ALLOW));

    storage.insertPolicies(policiesToInsert);

    for (const auto &bucket : policiesToInsert) {
        const PolicyBucketId &bucketId = bucket.first;
        for (const auto &policy : bucket.second) {
            EXPECT_CALL(backend, searchBucket(bucketId, policy.key()));
            EXPECT_CALL(backend, deletePolicy(bucketId, policy.key()));
            EXPECT_CALL(backend, insertPolicy(bucketId, _));
        }
    }

    storage.insertPolicies(policiesToInsert);
}
