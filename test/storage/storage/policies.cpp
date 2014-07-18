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

#include <memory>
#include <tuple>

using namespace Cynara;

TEST(storage, deletePolicies) {
    using ::testing::_;
    using ::testing::Eq;
    using ::testing::TypedEq;
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    auto pk1 = Helpers::generatePolicyKey("1");
    auto pk2 = Helpers::generatePolicyKey("2");
    PolicyBucketId bucketId1 = "bucket";
    PolicyBucketId bucketId2 = "bucket-2";


    EXPECT_CALL(backend, deletePolicy(bucketId1, pk1));
    EXPECT_CALL(backend, deletePolicy(bucketId2, pk1));
    EXPECT_CALL(backend, deletePolicy(bucketId1, pk2));

    storage.deletePolicies({
        std::make_tuple(pk1, bucketId1),
        std::make_tuple(pk1, bucketId2),
        std::make_tuple(pk2, bucketId1),
    });
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
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId testBucket1 = "test-bucket-1";
    PolicyBucketId testBucket2 = "test-bucket-2";

    std::vector<Storage::PolicyPolicyBucket> policiesToInsert = {
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("1"), PredefinedPolicyType::ALLOW), testBucket1),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("2"), PredefinedPolicyType::DENY), testBucket1),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("3"), PredefinedPolicyType::DENY), testBucket1),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("4"), PredefinedPolicyType::ALLOW), testBucket2),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("5"), PredefinedPolicyType::ALLOW), testBucket2),
    };

    for (const auto &policy : policiesToInsert) {
        PolicyBucketId bucketId;
        PolicyPtr policyPtr;
        std::tie(policyPtr, bucketId) = policy;
        EXPECT_CALL(backend, searchBucket(bucketId, policyPtr->key()))
            .WillOnce(Return(PolicyBucket()));
        EXPECT_CALL(backend, insertPolicy(bucketId, policyPtr));
    }

    storage.insertPolicies(policiesToInsert);
}

TEST(storage, updatePolicies) {
    using ::testing::Return;
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId testBucket1 = "test-bucket-1";
    PolicyBucketId testBucket2 = "test-bucket-2";

    std::vector<Storage::PolicyPolicyBucket> policiesToInsert = {
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("1"), PredefinedPolicyType::ALLOW), testBucket1),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("2"), PredefinedPolicyType::DENY), testBucket1),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("3"), PredefinedPolicyType::DENY), testBucket1),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("4"), PredefinedPolicyType::ALLOW), testBucket2),
        std::make_tuple(Policy::simpleWithKey(Helpers::generatePolicyKey("5"), PredefinedPolicyType::ALLOW), testBucket2),
    };


    PolicyCollection pc({std::get<0>(policiesToInsert.at(0))});

    for (const auto &policy : policiesToInsert) {
        PolicyBucketId bucketId;
        PolicyPtr policyPtr;
        std::tie(policyPtr, bucketId) = policy;
        EXPECT_CALL(backend, searchBucket(bucketId, policyPtr->key()))
            .WillOnce(Return(PolicyBucket(PolicyCollection({std::get<0>(policy)}))));
        EXPECT_CALL(backend, deletePolicy(bucketId, policyPtr->key()));
        EXPECT_CALL(backend, insertPolicy(bucketId, policyPtr));
    }

    storage.insertPolicies(policiesToInsert);
}
