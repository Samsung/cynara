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

#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <storage/Storage.h>
#include <storage/StorageBackend.h>
#include <types/pointers.h>
#include <types/PolicyCollection.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include "../../helpers.h"
#include "fakestoragebackend.h"

using namespace Cynara;

TEST(storage, deletePolicies) {
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    auto pk1 = Helpers::generatePolicyKey("1");
    auto pk2 = Helpers::generatePolicyKey("2");
    PolicyBucketId bucketId1 = "bucket-1";
    PolicyBucketId bucketId2 = "bucket-2";

    EXPECT_CALL(backend, deletePolicy(bucketId1, pk1));
    EXPECT_CALL(backend, deletePolicy(bucketId1, pk2));
    EXPECT_CALL(backend, deletePolicy(bucketId2, pk1));

    typedef std::pair<PolicyBucketId, std::vector<PolicyKey>> BucketPoliciesPair;

    storage.deletePolicies({
        BucketPoliciesPair(bucketId1, { pk1, pk2 }),
        BucketPoliciesPair(bucketId2, { pk1 }),
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
    using ::testing::Pointee;
    using ::testing::Return;
    FakeStorageBackend backend;
    Storage storage(backend);

    PolicyBucketId testBucket1 = "test-bucket-1";
    PolicyBucketId testBucket2 = "test-bucket-2";

    typedef std::pair<PolicyBucketId, std::vector<Policy>> BucketPolicyPair;

    auto createPolicy = [] (const std::string &keySuffix, const PolicyType &type) -> Policy {
        return Policy(Helpers::generatePolicyKey(keySuffix), type);
    };

    std::map<PolicyBucketId, std::vector<Policy>> policiesToInsert = {
        BucketPolicyPair(testBucket1, {
            createPolicy("1", PredefinedPolicyType::ALLOW),
            createPolicy("2", PredefinedPolicyType::DENY),
            createPolicy("3", PredefinedPolicyType::DENY)
        }),
        BucketPolicyPair(testBucket2, {
            createPolicy("4", PredefinedPolicyType::ALLOW),
            createPolicy("5", PredefinedPolicyType::ALLOW)
        })
    };

    for (const auto &group : policiesToInsert) {
        const auto &bucketId = group.first;
        const auto &policies = group.second;

        for (const auto &policy : policies) {
            EXPECT_CALL(backend, searchBucket(bucketId, policy.key()))
                .WillOnce(Return(PolicyBucket()));
            EXPECT_CALL(backend, insertPolicy(bucketId, Pointee(policy)));
        }
    }

    storage.insertPolicies(policiesToInsert);
}

TEST(storage, updatePolicies) {
    using ::testing::Pointee;
    using ::testing::Return;
    FakeStorageBackend backend;
    Storage storage(backend);

    PolicyBucketId testBucket1 = "test-bucket-1";
    PolicyBucketId testBucket2 = "test-bucket-2";

    typedef std::pair<PolicyBucketId, std::vector<Policy>> BucketPolicyPair;

    auto createPolicy = [] (const std::string &keySuffix, const PolicyType &type) -> Policy {
        return Policy(Helpers::generatePolicyKey(keySuffix), type);
    };

    std::map<PolicyBucketId, std::vector<Policy>> policiesToInsert = {
        BucketPolicyPair(testBucket1, {
            createPolicy("1", PredefinedPolicyType::ALLOW),
            createPolicy("2", PredefinedPolicyType::DENY),
            createPolicy("3", PredefinedPolicyType::DENY)
        }),
        BucketPolicyPair(testBucket2, {
            createPolicy("4", PredefinedPolicyType::ALLOW),
            createPolicy("5", PredefinedPolicyType::ALLOW)
        })
    };

    for (const auto &group : policiesToInsert) {
        const auto &bucketId = group.first;
        const auto &policies = group.second;

        for (const auto &policy : policies) {
            PolicyBucket searchResult(PolicyCollection { std::make_shared<Policy>(policy) });
            EXPECT_CALL(backend, searchBucket(bucketId, policy.key()))
                .WillOnce(Return(searchResult));
            EXPECT_CALL(backend, deletePolicy(bucketId, policy.key()));
            EXPECT_CALL(backend, insertPolicy(bucketId, Pointee(policy)));
        }
    }

    storage.insertPolicies(policiesToInsert);
}
