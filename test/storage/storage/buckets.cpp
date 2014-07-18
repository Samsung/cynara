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
 * @brief       Tests of buckets in Storage
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

TEST(storage, addBucket) {
    using ::testing::Return;

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId bucketId = "test-bucket";
    PolicyResult defaultPolicy(PredefinedPolicyType::DENY);

    EXPECT_CALL(backend, hasBucket(bucketId)).WillOnce(Return(false));
    EXPECT_CALL(backend, createBucket(bucketId, defaultPolicy));

    storage.addOrUpdateBucket(bucketId, defaultPolicy);
}

TEST(storage, updateBucket) {
    using ::testing::Return;

    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId bucketId = "test-bucket";
    PolicyResult defaultPolicy(PredefinedPolicyType::DENY);

    EXPECT_CALL(backend, hasBucket(bucketId)).WillOnce(Return(true));
    EXPECT_CALL(backend, updateBucket(bucketId, defaultPolicy));

    storage.addOrUpdateBucket(bucketId, defaultPolicy);
}

// Cannot delete default bucket
TEST(storage, deleteDefaultBucket) {
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    ASSERT_THROW(
        storage.deleteBucket(defaultPolicyBucketId),
        DefaultBucketDeletionException
    );
}

// Delete bucket and all policies linking to it
TEST(storage, deleteBucket) {
    FakeStorageBackend backend;
    Cynara::Storage storage(backend);

    PolicyBucketId bucketId = "test-bucket";

    EXPECT_CALL(backend, deleteLinking(bucketId));
    EXPECT_CALL(backend, deleteBucket(bucketId));

    storage.deleteBucket(bucketId);
}
