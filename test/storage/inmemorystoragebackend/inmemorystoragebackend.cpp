/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/storage/inmemorystoragebackend/inmemorystoragebackend.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests of InMemeoryStorageBackend
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "exceptions/BucketNotExistsException.h"
#include "exceptions/BucketDeserializationException.h"
#include "exceptions/DatabaseCorruptedException.h"
#include "exceptions/DefaultBucketDeletionException.h"
#include "exceptions/FileNotFoundException.h"
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

const std::string InMemeoryStorageBackendFixture::m_indexFileName("buckets");
const std::string InMemeoryStorageBackendFixture::m_backupFileNameSuffix("~");
const std::string InMemeoryStorageBackendFixture::m_fakeDbPath("/fake/database/path");

TEST_F(InMemeoryStorageBackendFixture, defaultPolicyIsDeny) {
    using ::testing::ReturnRef;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
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

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
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

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
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

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
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

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
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

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillOnce(ReturnRef(m_buckets));

    EXPECT_THROW(backend.deletePolicy("non-existent", Helpers::generatePolicyKey()),
                 BucketNotExistsException);
}

// Database dir is empty
TEST_F(InMemeoryStorageBackendFixture, load_no_db) {
    using ::testing::ReturnRef;
    auto testDbPath = std::string(CYNARA_TESTS_DIR) + "/empty_db/";
    FakeInMemoryStorageBackend backend(testDbPath);
    EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));
    EXPECT_THROW(backend.load(), DatabaseCorruptedException);
    ASSERT_DB_EMPTY(m_buckets);
}

// Database dir contains index with default bucket, but no file for this bucket
TEST_F(InMemeoryStorageBackendFixture, load_no_default) {
    using ::testing::ReturnRef;
    auto testDbPath = std::string(CYNARA_TESTS_DIR) + "/db2/";
    FakeInMemoryStorageBackend backend(testDbPath);
    EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));
    EXPECT_THROW(backend.load(), DatabaseCorruptedException);
    ASSERT_DB_EMPTY(m_buckets);
}

// Database contains index with default bucket and an empty bucket file
TEST_F(InMemeoryStorageBackendFixture, load_default_only) {
    using ::testing::ReturnRef;
    using ::testing::Return;
    auto testDbPath = std::string(CYNARA_TESTS_DIR) + "/db3/";
    FakeInMemoryStorageBackend backend(testDbPath);
    EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));
    EXPECT_CALL(backend, postLoadCleanup(false)).WillOnce(Return());
    backend.load();
    ASSERT_DB_VIRGIN(m_buckets);
}

// Database contains index with default bucket and an additional bucket
// There are files for both buckets present
TEST_F(InMemeoryStorageBackendFixture, load_2_buckets) {
    using ::testing::ReturnRef;
    using ::testing::Return;
    using ::testing::IsEmpty;

    auto testDbPath = std::string(CYNARA_TESTS_DIR) + "/db4/";

    FakeInMemoryStorageBackend backend(testDbPath);
    EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));
    EXPECT_CALL(backend, postLoadCleanup(false)).WillOnce(Return());
    backend.load();

    std::vector<std::string> bucketIds = { "", "additional" };

    for(const auto &bucketId : bucketIds) {
        SCOPED_TRACE(bucketId);
        const auto bucketIter = m_buckets.find(bucketId);
        ASSERT_NE(m_buckets.end(), bucketIter);

        const auto &bucketPolicies = bucketIter->second;
        ASSERT_THAT(bucketPolicies, IsEmpty());
    }
}

// Database contains index with 2 buckets; 1st bucket is valid, but second is corrupted
TEST_F(InMemeoryStorageBackendFixture, second_bucket_corrupted) {
    using ::testing::ReturnRef;
    auto testDbPath = std::string(CYNARA_TESTS_DIR) + "/db5/";
    FakeInMemoryStorageBackend backend(testDbPath);
    EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));
    EXPECT_THROW(backend.load(), DatabaseCorruptedException);
    ASSERT_DB_EMPTY(m_buckets);
}

/**
 * @brief   Database was corrupted, restore from backup (which is present)
 * @test    Scenario:
 * - There still is guard file in earlier prepared Cynara's policy database directory (db6)
 * - Execution of load() should use backup files (present, with different contents than primaries)
 * - Loaded database is checked - backup files were empty, so should recently loaded policies
 */
TEST_F(InMemeoryStorageBackendFixture, load_from_backup) {
    using ::testing::ReturnRef;
    using ::testing::Return;
    using ::testing::IsEmpty;
    using ::testing::InSequence;

    auto testDbPath = std::string(CYNARA_TESTS_DIR) + "/db6/";
    FakeInMemoryStorageBackend backend(testDbPath);

    {
        // Calls are expected in a specific order
        InSequence s;
        EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));
        EXPECT_CALL(backend, postLoadCleanup(true)).WillOnce(Return());
        backend.load();
    }

    std::vector<std::string> bucketIds = { "", "additional" };
    for(const auto &bucketId : bucketIds) {
        SCOPED_TRACE(bucketId);
        const auto bucketIter = m_buckets.find(bucketId);
        ASSERT_NE(m_buckets.end(), bucketIter);

        const auto &bucketPolicies = bucketIter->second;
        ASSERT_THAT(bucketPolicies, IsEmpty());
    }
}

/**
 * @brief   Erase from non-exiting bucket should throw BucketNotExistsException
 * @test    Scenario:
 * - For different filter keys:
 *     - try erase policies from "non-existing-bucket"
 *     - expect BucketNotExistsException is thrown
 */
TEST_F(InMemeoryStorageBackendFixture, erasePoliciesEmptyBase) {
    using ::testing::ReturnRef;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    PolicyBucketId bucketId = "non-existing-bucket";
    const bool NON_RECURSIVE = false;

    for (const auto &filter : filters()) {
        SCOPED_TRACE(filter.name());

        EXPECT_THROW(backend.erasePolicies(bucketId, NON_RECURSIVE, filter.key()),
                     BucketNotExistsException);
    }
}

/**
 * @brief   Erase from single bucket
 * @test    Scenario:
 * - For different filter keys:
 *     - create "test-bucket"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket"
 *     - erase policies from "test-bucket"
 *     - check if received results match expected
 */
TEST_F(InMemeoryStorageBackendFixture, erasePoliciesSingleBucket) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAreArray;

    PolicyBucketId bucketId = "test-bucket";
    const bool NON_RECURSIVE = false;

    for (const auto &filter : filters()) {
        SCOPED_TRACE(filter.name());

        FakeInMemoryStorageBackend backend(m_fakeDbPath);
        EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));

        createBucket(bucketId);
        addToBucket(bucketId, fullPoliciesCollection());

        backend.erasePolicies(bucketId, NON_RECURSIVE, filter.key());
        ASSERT_THAT(m_buckets.at(bucketId),
                    UnorderedElementsAreArray(expectedEraseResult(filter.name())));
    }
}

/**
 * @brief   Verify if recursive erase doesn't propagate to not linked bucket
 * @test    Scenario:
 * - For different filter keys:
 *     - create "test-bucket"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket"
 *     - create "test-bucket2"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket2"
 *     - erase policies from "test-bucket"
 *     - check if received results match expected in case of "test-bucket"
 *     - check if policies in "test-bucket2" remain unaffected
 */
TEST_F(InMemeoryStorageBackendFixture, erasePoliciesRecursiveNotLinkedBuckets) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAreArray;

    PolicyBucketId bucketId = "test-bucket";
    PolicyBucketId bucketId2 = "test-bucket2";
    const bool RECURSIVE = true;

    for (const auto &filter : filters()) {
        SCOPED_TRACE(filter.name());

        FakeInMemoryStorageBackend backend(m_fakeDbPath);
        EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));

        createBucket(bucketId);
        addToBucket(bucketId, fullPoliciesCollection());

        createBucket(bucketId2);
        addToBucket(bucketId2, fullPoliciesCollection());

        backend.erasePolicies(bucketId, RECURSIVE, filter.key());
        ASSERT_THAT(m_buckets.at(bucketId),
                    UnorderedElementsAreArray(expectedEraseResult(filter.name())));
        ASSERT_THAT(m_buckets.at(bucketId2),
                    UnorderedElementsAreArray(fullPoliciesCollection()));
    }
}

/**
 * @brief   Verify if recursive erase does propagate to linked bucket
 * @test    Scenario:
 * - For different filter keys:
 *     - create "test-bucket"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket"
 *     - create "test-bucket2"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket2"
 *     - add policy linking test buckets
 *     - erase policies from "test-bucket"
 *     - check if received results match expected in case of "test-bucket"
 *     - check if received results match expected in case of "test-bucket2"
 */
TEST_F(InMemeoryStorageBackendFixture, erasePoliciesRecursiveLinkedBuckets) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAreArray;

    PolicyBucketId bucketId = "test-bucket";
    PolicyBucketId bucketId2 = "test-bucket2";
    PolicyKey linkKey = Helpers::generatePolicyKey("link");
    PolicyCollection linkPolicy = {Policy::bucketWithKey(linkKey, bucketId2)};
    const bool RECURSIVE = true;

    for (const auto &filter : filters()) {
        SCOPED_TRACE(filter.name());

        FakeInMemoryStorageBackend backend(m_fakeDbPath);
        EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));

        createBucket(bucketId);
        addToBucket(bucketId, fullPoliciesCollection());

        createBucket(bucketId2);
        addToBucket(bucketId2, fullPoliciesCollection());

        addToBucket(bucketId, linkPolicy);

        backend.erasePolicies(bucketId, RECURSIVE, filter.key());

        PolicyCollection expectedResult = expectedEraseResult(filter.name());
        if (!isLinkPolicyErased(filter.name())) {
            expectedResult.insert(expectedResult.end(), linkPolicy.begin(), linkPolicy.end());
        }
        ASSERT_THAT(m_buckets.at(bucketId),
                    UnorderedElementsAreArray(expectedResult));
        ASSERT_THAT(m_buckets.at(bucketId2),
                    UnorderedElementsAreArray(expectedEraseResult(filter.name())));
    }
}

/**
 * @brief   Verify if recursive erase doesn't propagate backwards to linked bucket
 * @test    Scenario:
 * - For different filter keys:
 *     - create "test-bucket"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket"
 *     - create "test-bucket2"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket2"
 *     - add policy linking test buckets
 *     - erase policies from "test-bucket2"
 *     - check if policies in "test-bucket" remain unaffected
 *     - check if received results match expected in case of "test-bucket2"
 */
TEST_F(InMemeoryStorageBackendFixture, erasePoliciesRecursiveLinkedBucketsNoBackwardPropagation) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAreArray;

    PolicyBucketId bucketId = "test-bucket";
    PolicyBucketId bucketId2 = "test-bucket2";
    PolicyKey linkKey = Helpers::generatePolicyKey("link");
    PolicyCollection linkPolicy = {Policy::bucketWithKey(linkKey, bucketId2)};
    const bool RECURSIVE = true;

    for (const auto &filter : filters()) {
        SCOPED_TRACE(filter.name());

        FakeInMemoryStorageBackend backend(m_fakeDbPath);
        EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));

        createBucket(bucketId);
        addToBucket(bucketId, fullPoliciesCollection());

        createBucket(bucketId2);
        addToBucket(bucketId2, fullPoliciesCollection());

        addToBucket(bucketId, linkPolicy);

        backend.erasePolicies(bucketId2, RECURSIVE, filter.key());

        PolicyCollection expectedResult = fullPoliciesCollection();
        expectedResult.insert(expectedResult.end(), linkPolicy.begin(), linkPolicy.end());

        ASSERT_THAT(m_buckets.at(bucketId),
                    UnorderedElementsAreArray(expectedResult));
        ASSERT_THAT(m_buckets.at(bucketId2),
                    UnorderedElementsAreArray(expectedEraseResult(filter.name())));
    }
}

/**
 * @brief   Verify if non-recursive erase doesn't propagate to linked bucket
 * @test    Scenario:
 * - For different filter keys:
 *     - create "test-bucket"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket"
 *     - create "test-bucket2"
 *     - add all types of policies (fullPoliciesCollection) into "test-bucket2"
 *     - add policy linking test buckets
 *     - erase policies from "test-bucket"
 *     - check if received results match expected in case of "test-bucket"
 *     - check if policies in "test-bucket2" remain unaffected
 */
TEST_F(InMemeoryStorageBackendFixture, erasePoliciesNonRecursiveLinkedBuckets) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAreArray;

    PolicyBucketId bucketId = "test-bucket";
    PolicyBucketId bucketId2 = "test-bucket2";
    PolicyKey linkKey = Helpers::generatePolicyKey("link");
    PolicyCollection linkPolicy = {Policy::bucketWithKey(linkKey, bucketId2)};
    const bool NON_RECURSIVE = false;

    for (const auto &filter : filters()) {
        SCOPED_TRACE(filter.name());

        FakeInMemoryStorageBackend backend(m_fakeDbPath);
        EXPECT_CALL(backend, buckets()).WillRepeatedly(ReturnRef(m_buckets));

        createBucket(bucketId);
        addToBucket(bucketId, fullPoliciesCollection());

        createBucket(bucketId2);
        addToBucket(bucketId2, fullPoliciesCollection());

        addToBucket(bucketId, linkPolicy);

        backend.erasePolicies(bucketId, NON_RECURSIVE, filter.key());

        PolicyCollection expectedResult = expectedEraseResult(filter.name());
        if (!isLinkPolicyErased(filter.name())) {
            expectedResult.insert(expectedResult.end(), linkPolicy.begin(), linkPolicy.end());
        }
        ASSERT_THAT(m_buckets.at(bucketId),
                    UnorderedElementsAreArray(expectedResult));
        ASSERT_THAT(m_buckets.at(bucketId2),
                    UnorderedElementsAreArray(fullPoliciesCollection()));
    }
}
