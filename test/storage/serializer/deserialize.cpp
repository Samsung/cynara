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
 * @file        deserialize.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::StorageDeserializer
 */


#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/types/PolicyBucket.h>
#include <service/storage/StorageDeserializer.h>

#include <tuple>
#include <memory>
#include <istream>


// TODO: Move to .h, because it's used also in bucket_load.cpp
MATCHER_P(PolicyPtrEq, policy, "") {
    return std::tie(policy->key(), policy->result())
        == std::tie(arg->key(), arg->result());
}

MATCHER_P(PolicyBucketIdPolicyEq, expected, "") {
    auto bucket1 = expected.second;
    auto bucket2 = arg.second;
    return std::tie(expected.first, bucket1.id(), bucket1.defaultPolicy())
        == std::tie(arg.first, bucket2.id(), bucket2.defaultPolicy());
}

#define COMPARE_BUCKETS(id, bucket) \
        PolicyBucketIdPolicyEq(std::pair<PolicyBucketId, PolicyBucket>((id), (bucket)))


class FakeStreamForBucketId {
public:
    MOCK_METHOD1(streamForBucketId, std::shared_ptr<std::istream>(const std::string &));
};

class StorageDeserializerFixture : public ::testing::Test {
public:
    Cynara::StorageDeserializer::BucketStreamOpener nullStreamOpener =
            [] (const std::string &) -> std::shared_ptr<std::istream> {
                return nullptr;
            };
};

using namespace Cynara;

TEST_F(StorageDeserializerFixture, init_default_only) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss(";0");
    StorageDeserializer deserializer(ss, nullStreamOpener);

    InMemoryStorageBackend::Buckets buckets;
    deserializer.initBuckets(buckets);

    ASSERT_THAT(buckets, UnorderedElementsAre(
        COMPARE_BUCKETS("", PolicyBucket("", PredefinedPolicyType::DENY))
    ));
}

TEST_F(StorageDeserializerFixture, init_more) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss(";0\n"
                          "bucket2;0\n"
                          "bucket3;0xFFFE;bucket2\n");

    StorageDeserializer deserializer(ss, nullStreamOpener);
    InMemoryStorageBackend::Buckets buckets;
    deserializer.initBuckets(buckets);

    ASSERT_THAT(buckets, UnorderedElementsAre(
        COMPARE_BUCKETS("", PolicyBucket("", PredefinedPolicyType::DENY)),
        COMPARE_BUCKETS("bucket2", PolicyBucket("bucket2", PredefinedPolicyType::DENY)),
        COMPARE_BUCKETS("bucket3", PolicyBucket("bucket3",
                                        PolicyResult(PredefinedPolicyType::BUCKET, "bucket2")))
    ));
}

// Add bucket and check, if it's overwritten if exists in file
TEST_F(StorageDeserializerFixture, init_overwrite) {
    using ::testing::UnorderedElementsAre;

    std::istringstream ss(";0");
    StorageDeserializer deserializer(ss, nullStreamOpener);

    InMemoryStorageBackend::Buckets buckets;
    buckets.insert({ "", PolicyBucket("fakeId", PredefinedPolicyType::ALLOW) });
    deserializer.initBuckets(buckets);

    ASSERT_THAT(buckets, UnorderedElementsAre(
        COMPARE_BUCKETS("", PolicyBucket("", PredefinedPolicyType::DENY))
    ));
}

TEST_F(StorageDeserializerFixture, load_buckets_plus_policies) {
    using ::testing::_;
    using ::testing::Return;
    using ::testing::UnorderedElementsAre;

    InMemoryStorageBackend::Buckets buckets;
    buckets.insert({ "", PolicyBucket("", PredefinedPolicyType::DENY) });

    std::istringstream bucketsStream; // Won't be used; buckets are pre-inserted above
    FakeStreamForBucketId streamOpener;
    auto streamOpenerFunc = std::bind(&FakeStreamForBucketId::streamForBucketId, &streamOpener,
                                      std::placeholders::_1);
    StorageDeserializer deserializer(bucketsStream, streamOpenerFunc);

    auto defaultBucketStream = std::make_shared<std::istringstream>("c;u;p;0;meta");
    EXPECT_CALL(streamOpener, streamForBucketId(""))
        .WillOnce(Return(defaultBucketStream));

    deserializer.loadBuckets(buckets);

    // Check if our bucket is still there
    ASSERT_THAT(buckets, UnorderedElementsAre(
        COMPARE_BUCKETS("", PolicyBucket("", PredefinedPolicyType::DENY))
    ));

    auto expectedPolicy = std::make_shared<Policy>(PolicyKey("c", "u", "p"),
                                      PolicyResult(PredefinedPolicyType::DENY, "meta"));

    // Check policy was inserted into bucket
    ASSERT_THAT(buckets.at("").policyCollection(), UnorderedElementsAre(
       PolicyPtrEq(expectedPolicy)
   ));
}

TEST_F(StorageDeserializerFixture, load_buckets) {
    using ::testing::_;
    using ::testing::Return;
    using ::testing::UnorderedElementsAre;

    // Pre-insert some buckets
    InMemoryStorageBackend::Buckets buckets;
    buckets.insert({ "", PolicyBucket("", PredefinedPolicyType::DENY) });
    buckets.insert({ "bucket1", PolicyBucket("bucket1", PredefinedPolicyType::DENY) });
    buckets.insert({ "bucket2", PolicyBucket("bucket2", PredefinedPolicyType::DENY) });

    std::istringstream bucketsStream; // Won't be used; buckets are pre-inserted above
    FakeStreamForBucketId streamOpener;
    auto streamOpenerFunc = std::bind(&FakeStreamForBucketId::streamForBucketId, &streamOpener,
                                      std::placeholders::_1);
    StorageDeserializer deserializer(bucketsStream, streamOpenerFunc);

    // Check, if streamOpener was called for each bucket
    EXPECT_CALL(streamOpener, streamForBucketId(""))
        .WillOnce(Return(nullptr));

    EXPECT_CALL(streamOpener, streamForBucketId("bucket1"))
        .WillOnce(Return(nullptr));

    EXPECT_CALL(streamOpener, streamForBucketId("bucket2"))
        .WillOnce(Return(nullptr));

    deserializer.loadBuckets(buckets);
}
