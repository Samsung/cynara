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
 * @file        check.cpp
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

    EXPECT_CALL(backend, searchBucket(defaultPolicyBucketId, pk))
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

    EXPECT_CALL(backend, searchBucket(defaultPolicyBucketId, pk))
        .WillRepeatedly(ReturnPointee(&bucket));

    // Default bucket empty -- return DENY
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(pk).policyType());

    // Add ALLOW to default bucket -- return ALLOW
    bucket.insertPolicy(Policy::simpleWithKey(pk, PredefinedPolicyType::ALLOW));
    ASSERT_EQ(PredefinedPolicyType::ALLOW, storage.checkPolicy(pk).policyType());

    // Add DENY to default bucket -- return DENY
    bucket.insertPolicy(Policy::simpleWithKey(pk, PredefinedPolicyType::DENY));
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(pk).policyType());
}

// TODO: Refactorize to resemble checkNonrecursive()
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

    EXPECT_CALL(backend, searchBucket(defaultPolicyBucketId, pk))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    EXPECT_CALL(backend, searchBucket(additionalBucketId, pk))
        .WillRepeatedly(ReturnPointee(&additionalBucket));


    // Bucket empty -- should return DENY as default bucket value
    ASSERT_EQ(PredefinedPolicyType::DENY, storage.checkPolicy(pk).policyType());

    // Add ALLOW to bucket, so return ALLOW
    additionalBucket.insertPolicy(Policy::simpleWithKey(pk, PredefinedPolicyType::ALLOW));
    ASSERT_EQ(PredefinedPolicyType::ALLOW, storage.checkPolicy(pk).policyType());

    // Add DENY to default bucket -- return DENY, even though ALLOW in other bucket
    defaultBucket.insertPolicy(Policy::simpleWithKey(pk, PredefinedPolicyType::DENY));
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

    EXPECT_CALL(backend, searchBucket(defaultPolicyBucketId, checkKey))
        .WillRepeatedly(ReturnPointee(&defaultBucket));

    // Check, if next bucket is filtered with original key
    EXPECT_CALL(backend, searchBucket(additionalBucketId, checkKey))
        .WillRepeatedly(ReturnPointee(&additionalBucket));

    // Should return additional bucket's default policy
    ASSERT_EQ(PredefinedPolicyType::ALLOW, storage.checkPolicy(checkKey));
}

TEST(storage, checkNonrecursive) {
    using ::testing::ReturnPointee;

    PolicyKey pk = Helpers::generatePolicyKey();
    PolicyBucketId bucketId = "a-bucket";

    PolicyBucket bucket(bucketId, PredefinedPolicyType::ALLOW,
                               { Policy::bucketWithKey(pk, "must-not-be-touched") });
    FakeStorageBackend backend;

    Cynara::Storage storage(backend);

    EXPECT_CALL(backend, searchBucket(bucketId, pk))
        .WillOnce(ReturnPointee(&bucket));

    ASSERT_EQ(PredefinedPolicyType::ALLOW, storage.checkPolicy(pk, bucketId, false));
}

/*
 * bucket1 contains policy (with key pk) pointing to bucket2
 * bucket2 is empty and it's default policy is NONE
 * Because NONE policy in bucket2, check should yield default policy of bucket1 and not of bucket2
 */
TEST(storage, noneBucket) {
    using ::testing::ReturnPointee;
    using PredefinedPolicyType::ALLOW;
    using PredefinedPolicyType::NONE;

    auto pk = Helpers::generatePolicyKey();

    PolicyBucket bucket2("bucket-2", NONE);
    PolicyBucket bucket1("bucket-1", ALLOW, { Policy::bucketWithKey(pk, bucket2.id()) });

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    EXPECT_CALL(backend, searchBucket(bucket1.id(), pk))
        .WillOnce(ReturnPointee(&bucket1));
    EXPECT_CALL(backend, searchBucket(bucket2.id(), pk))
        .WillOnce(ReturnPointee(&bucket2));

    ASSERT_EQ(ALLOW, storage.checkPolicy(pk, bucket1.id(), true));
}

/*
 * Scenario similar to noneBucket, but bucket2 contains matching policy.
 * In this case this policy should be returned.
 */
TEST(storage, noneBucketNotEmpty) {
    using ::testing::ReturnPointee;
    using PredefinedPolicyType::ALLOW;
    using PredefinedPolicyType::DENY;
    using PredefinedPolicyType::NONE;

    auto pk = Helpers::generatePolicyKey();

    PolicyBucket bucket2("bucket-2", NONE, { Policy::simpleWithKey(pk, DENY) });
    PolicyBucket bucket1("bucket-1", ALLOW, { Policy::bucketWithKey(pk, bucket2.id()) });

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    EXPECT_CALL(backend, searchBucket(bucket1.id(), pk))
        .WillOnce(ReturnPointee(&bucket1));
    EXPECT_CALL(backend, searchBucket(bucket2.id(), pk))
        .WillOnce(ReturnPointee(&bucket2));

    ASSERT_EQ(DENY, storage.checkPolicy(pk, bucket1.id(), true));
}

/*
 * Single empty bucket with default policy of NONE
 * -- searching for any key should yield NONE
 */
TEST(storage, singleNoneBucket) {
    using ::testing::ReturnPointee;
    using PredefinedPolicyType::NONE;

    auto pk = Helpers::generatePolicyKey();

    PolicyBucket bucket("bucket", NONE, {});

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    EXPECT_CALL(backend, searchBucket(bucket.id(), pk))
        .WillOnce(ReturnPointee(&bucket));

    ASSERT_EQ(NONE, storage.checkPolicy(pk, bucket.id(), true));
}
