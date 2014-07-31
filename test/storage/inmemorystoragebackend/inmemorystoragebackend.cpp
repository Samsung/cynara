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
 * @file        inmemorystoragebackend.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests of InMemeoryStorageBackend
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "exceptions/DefaultBucketDeletionException.h"
#include "exceptions/BucketNotExistsException.h"
#include "storage/InMemoryStorageBackend.h"
#include "storage/StorageBackend.h"
#include "types/PolicyCollection.h"
#include "types/PolicyKey.h"
#include "types/PolicyResult.h"
#include "types/PolicyType.h"

#include "../../helpers.h"
#include "fakeinmemorystoragebackend.h"
#include "inmemeorystoragebackendfixture.h"

using namespace Cynara;

TEST_F(InMemeoryStorageBackendFixture, defaultPolicyIsDeny) {
    using ::testing::ReturnRef;

    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .WillOnce(ReturnRef(m_buckets));

    createBucket(defaultPolicyBucketId);

    // Policy key will not be found -- should fall back to default policy for bucket
    auto filteredBucket = backend.searchDefaultBucket(Helpers::generatePolicyKey());
    auto defaultPolicy = filteredBucket.defaultPolicy();

    ASSERT_EQ(PredefinedPolicyType::DENY, defaultPolicy.policyType());
}

// TODO: Refactorize this test to be shorter and clearer
TEST_F(InMemeoryStorageBackendFixture, deleteLinking) {
    using ::testing::ReturnRef;
    using ::testing::IsEmpty;
    using ::testing::UnorderedElementsAre;

    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .Times(4)
        .WillRepeatedly(ReturnRef(m_buckets));

    const PolicyBucketId testBucket1 = "test-bucket-1";
    const PolicyBucketId testBucket2 = "test-bucket-2";
    const PolicyBucketId testBucket3 = "test-bucket-3";

    // Create 2 buckets
    for (const auto &bucketId : { testBucket1, testBucket2, testBucket3 }) {
        PolicyResult defaultPolicy(PredefinedPolicyType::DENY);
        backend.createBucket(bucketId, defaultPolicy);
    }

    // These policies do not link to buckets, so should stay untouched
    PolicyCollection policiesToStay = {
        Policy::simpleWithKey(Helpers::generatePolicyKey("1"), PredefinedPolicyType::DENY),
        Policy::simpleWithKey(Helpers::generatePolicyKey("1"), PredefinedPolicyType::DENY),
        Policy::simpleWithKey(Helpers::generatePolicyKey("2"), PredefinedPolicyType::ALLOW),
    };

    // Add some policies to 1st bucket, which link to 2nd bucket
    addToBucket(testBucket1, {
        Policy::bucketWithKey(Helpers::generatePolicyKey("1"), testBucket2), /* 1st */
        Policy::bucketWithKey(Helpers::generatePolicyKey("2"), testBucket2), /* 2nd */
        policiesToStay.at(0)
    });

    // Add some policies to 2nd bucket
    addToBucket(testBucket2, {
        policiesToStay.at(1), policiesToStay.at(2)
    });

    // Add some policies to 3rd bucket, which link to 2nd bucket
    addToBucket(testBucket3, {
        Policy::bucketWithKey(Helpers::generatePolicyKey("X"), testBucket2),
        Policy::bucketWithKey(Helpers::generatePolicyKey("Y"), testBucket2),
    });

    // Should delete 1st and 2nd policy from 1st bucket
    backend.deleteLinking(testBucket2);

    ASSERT_THAT(m_buckets.at(testBucket1),
            UnorderedElementsAre(policiesToStay.at(0)));
    ASSERT_THAT(m_buckets.at(testBucket2),
            UnorderedElementsAre(policiesToStay.at(1), policiesToStay.at(2)));
    ASSERT_THAT(m_buckets.at(testBucket3), IsEmpty());
}

TEST_F(InMemeoryStorageBackendFixture, insertPolicy) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAre;
    using PredefinedPolicyType::ALLOW;

    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .WillOnce(ReturnRef(m_buckets));

    PolicyBucketId bucketId = "test-bucket";
    createBucket(bucketId);

    auto policyToAdd = Policy::simpleWithKey(Helpers::generatePolicyKey(), ALLOW);
    backend.insertPolicy(bucketId, policyToAdd);

    ASSERT_THAT(m_buckets.at(bucketId), UnorderedElementsAre(policyToAdd));
}

TEST_F(InMemeoryStorageBackendFixture, insertPolicyToNonexistentBucket) {
    using ::testing::ReturnRef;

    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .WillOnce(ReturnRef(m_buckets));

    EXPECT_THROW(backend.insertPolicy("non-existent", nullptr), BucketNotExistsException);
}

TEST_F(InMemeoryStorageBackendFixture, deletePolicy) {
    using ::testing::ReturnRef;
    using ::testing::IsEmpty;
    using ::testing::UnorderedElementsAre;
    using ::testing::UnorderedElementsAreArray;
    using ::testing::ContainerEq;
    using PredefinedPolicyType::ALLOW;

    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .WillOnce(ReturnRef(m_buckets));

    PolicyBucketId bucketId = "test-bucket";
    createBucket(bucketId);

    auto policyToDelete = Policy::simpleWithKey(Helpers::generatePolicyKey(), ALLOW);

    PolicyCollection otherPolicies = {
        Policy::simpleWithKey(Helpers::generatePolicyKey("other-policy-1"), ALLOW),
        Policy::simpleWithKey(Helpers::generatePolicyKey("other-policy-2"), ALLOW),
        Policy::simpleWithKey(Helpers::generatePolicyKey("other-policy-3"), ALLOW),
    };

    addToBucket(bucketId, {
        otherPolicies.at(0),
        otherPolicies.at(1),
        policyToDelete,
        otherPolicies.at(2),
    });

    backend.deletePolicy(bucketId, policyToDelete->key());

    // Check if only policyToDelete has been deleted
    EXPECT_THAT(m_buckets.at(bucketId), UnorderedElementsAreArray(otherPolicies));
}

TEST_F(InMemeoryStorageBackendFixture, deletePolicyFromNonexistentBucket) {
    using ::testing::ReturnRef;
    using ::testing::IsEmpty;
    using ::testing::UnorderedElementsAre;

    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .WillOnce(ReturnRef(m_buckets));

    EXPECT_THROW(backend.deletePolicy("non-existent", Helpers::generatePolicyKey()),
                 BucketNotExistsException);
}
