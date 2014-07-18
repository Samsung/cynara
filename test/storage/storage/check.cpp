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
 * @brief       Tests of check in Storage
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

TEST(storage, checkEmpty) {
    using ::testing::ReturnPointee;

    PolicyBucket emptyBucket;

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);
    PolicyKey pk = Helpers::generatePolicyKey();

    EXPECT_CALL(backend, searchDefaultBucket(pk))
        .WillOnce(ReturnPointee(&emptyBucket));

    // Default bucket empty -- return DENY
    auto policyAnswer = storage.checkPolicy(pk);
    ASSERT_EQ(PredefinedPolicyType::DENY, policyAnswer.policyType());
}

TEST(storage, checkSimple) {
    using ::testing::ReturnPointee;

    PolicyBucket bucket;
    FakeStorageBackend backend;

    Cynara::Storage storage(backend);
    PolicyKey pk = Helpers::generatePolicyKey();

    EXPECT_CALL(backend, searchDefaultBucket(pk))
        .WillRepeatedly(ReturnPointee(&bucket));

    // Default bucket empty -- return DENY
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(pk).policyType());

    // Add ALLOW to default bucket -- return ALLOW
    bucket.policyCollection().push_back(Policy::simpleWithKey(pk, PredefinedPolicyType::ALLOW));
    ASSERT_EQ(PredefinedPolicyType::ALLOW, storage.checkPolicy(pk).policyType());

    // Add DENY to default bucket -- return DENY
    bucket.policyCollection().push_back(Policy::simpleWithKey(pk, PredefinedPolicyType::DENY));
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(pk).policyType());
}

TEST(storage, checkBucket) {
    using ::testing::ReturnPointee;

    const PolicyBucketId additionalBucketId = "additional-bucket";

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);
    PolicyKey pk = Helpers::generatePolicyKey();

    PolicyBucket defaultBucket(PolicyCollection({
        Policy::simpleWithKey(pk, PredefinedPolicyType::ALLOW),
        Policy::bucketWithKey(pk, additionalBucketId)
    }));

    PolicyBucket additionalBucket;

    EXPECT_CALL(backend, searchDefaultBucket(pk))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    EXPECT_CALL(backend, searchBucket(defaultPolicyBucketId, pk))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    EXPECT_CALL(backend, searchBucket(additionalBucketId, pk))
        .WillRepeatedly(ReturnPointee(&additionalBucket));


    // Bucket empty -- should return DENY as default bucket value
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(pk).policyType());

    // Add ALLOW to bucket, so return ALLOW
    additionalBucket.policyCollection().push_back(Policy::simpleWithKey(pk, PredefinedPolicyType::ALLOW));
    ASSERT_EQ(PredefinedPolicyType::ALLOW, storage.checkPolicy(pk).policyType());

    // Add DENY to default bucket -- return DENY, even though ALLOW in other bucket
    defaultBucket.policyCollection().push_back(Policy::simpleWithKey(pk, PredefinedPolicyType::DENY));
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(pk).policyType());
}

// Catch a bug, where consecutive buckets were filtered with wildcard policies' keys
// instead of original key being checked
TEST(storage, checkBucketWildcard) {
    using ::testing::Return;
    using ::testing::ReturnPointee;

    const PolicyBucketId additionalBucketId = "additional-bucket";
    const PolicyKey defaultBucketKey = PolicyKey("c", "*", "p");
    const PolicyKey checkKey = PolicyKey("c", "u1", "p");

    PolicyBucket defaultBucket(PolicyCollection({
        Policy::bucketWithKey(defaultBucketKey, additionalBucketId)
    }));

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    EXPECT_CALL(backend, searchDefaultBucket(checkKey))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    EXPECT_CALL(backend, searchBucket(defaultPolicyBucketId, checkKey))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    // Check, if next bucket is filtered with original key
    EXPECT_CALL(backend, searchBucket(additionalBucketId, checkKey))
        .WillRepeatedly(Return(PolicyBucket()));    // additional bucket would yield no records

    // Should return additional bucket's default policy
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(checkKey));
}

TEST(storage, checkBucketWildcardOtherDefault) {
    using ::testing::ReturnPointee;

    const PolicyBucketId additionalBucketId = "additional-bucket";
    const PolicyKey defaultBucketKey = PolicyKey("c", "*", "p");
    const PolicyKey checkKey = PolicyKey("c", "u1", "p");

    PolicyBucket defaultBucket(PolicyCollection({
        Policy::bucketWithKey(defaultBucketKey, additionalBucketId)
    }));

    PolicyBucket additionalBucket(additionalBucketId, PredefinedPolicyType::ALLOW);

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    EXPECT_CALL(backend, searchDefaultBucket(checkKey))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    EXPECT_CALL(backend, searchBucket(defaultPolicyBucketId, checkKey))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    // Check, if next bucket is filtered with original key
    EXPECT_CALL(backend, searchBucket(additionalBucketId, checkKey))
        .WillRepeatedly(ReturnPointee(&additionalBucket));

    // Should return additional bucket's default policy
    ASSERT_EQ(PredefinedPolicyType::ALLOW, storage.checkPolicy(checkKey));
}
