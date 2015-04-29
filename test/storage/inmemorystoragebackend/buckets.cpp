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
 * @file        test/storage/inmemorystoragebackend/buckets.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests of buckets in InMemoryStorageBackend
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <exceptions/BucketNotExistsException.h>
#include <types/PolicyBucket.h>
#include <types/PolicyResult.h>

#include "fakeinmemorystoragebackend.h"
#include "inmemorystoragebackendfixture.h"

using namespace Cynara;

TEST_F(InMemoryStorageBackendFixture, createBucket) {
    using ::testing::ReturnRef;
    using ::testing::IsEmpty;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    PolicyResult defaultPolicy(PredefinedPolicyType::ALLOW);
    PolicyBucketId bucketId = "new-bucket";
    backend.createBucket(bucketId, defaultPolicy);

    ASSERT_EQ(1, m_buckets.size());
    ASSERT_NE(m_buckets.end(), m_buckets.find(bucketId));
    ASSERT_EQ(defaultPolicy, m_buckets.at(bucketId).defaultPolicy());
    ASSERT_THAT(m_buckets.at(bucketId), IsEmpty());
}

TEST_F(InMemoryStorageBackendFixture, updateBucket) {
    using ::testing::ReturnRef;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    PolicyBucketId bucketId = "bucket";
    auto &bucket = this->createBucket(bucketId);
    bucket.setDefaultPolicy(PredefinedPolicyType::ALLOW);

    backend.updateBucket(bucketId, PredefinedPolicyType::DENY);

    ASSERT_EQ(PredefinedPolicyType::DENY, bucket.defaultPolicy());
}

TEST_F(InMemoryStorageBackendFixture, updateNonexistentBucket) {
    using ::testing::ReturnRef;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    EXPECT_THROW(backend.updateBucket("non-existent", PredefinedPolicyType::ALLOW),
                 BucketNotExistsException);
}

TEST_F(InMemoryStorageBackendFixture, deleteBucket) {
    using ::testing::ReturnRef;
    using ::testing::IsEmpty;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    PolicyBucketId bucketId = "delete-bucket";
    m_buckets.insert({ bucketId, PolicyBucket(bucketId) });

    backend.deleteBucket(bucketId);

    ASSERT_THAT(m_buckets, IsEmpty());
}

TEST_F(InMemoryStorageBackendFixture, hasBucket) {
    using ::testing::ReturnRef;
    using ::testing::IsEmpty;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    PolicyBucketId bucketId = "bucket";
    m_buckets.insert({ bucketId, PolicyBucket(bucketId) });

    ASSERT_TRUE(backend.hasBucket(bucketId));
    ASSERT_FALSE(backend.hasBucket("non-existent"));
}

TEST_F(InMemoryStorageBackendFixture, deleteNonexistentBucket) {
    using ::testing::ReturnRef;

    FakeInMemoryStorageBackend backend(m_fakeDbPath);
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    EXPECT_THROW(backend.deleteBucket("non-existent"), BucketNotExistsException);
}
