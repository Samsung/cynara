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
/**
 * @file        test/storage/serializer/deserialize.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::StorageDeserializer
 */

#include <istream>
#include <memory>
#include <tuple>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <exceptions/BucketDeserializationException.h>
#include <storage/Buckets.h>
#include <storage/StorageDeserializer.h>
#include <types/PolicyBucket.h>

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
    MOCK_METHOD1(streamForBucketId,
                 std::shared_ptr<Cynara::BucketDeserializer>(const std::string &));
};

class EmptyBucketDeserializer : public Cynara::BucketDeserializer {
public:
    EmptyBucketDeserializer()
        : Cynara::BucketDeserializer(std::make_shared<std::istringstream>("")) {}
};

class StorageDeserializerFixture : public ::testing::Test {
public:
    std::shared_ptr<Cynara::BucketDeserializer> emptyBucketStream() const {
        return std::make_shared<EmptyBucketDeserializer>();
    }

    Cynara::StorageDeserializer::BucketStreamOpener nullStreamOpener =
        [this] (const std::string &) -> std::shared_ptr<Cynara::BucketDeserializer> {
            return emptyBucketStream();
        };
};

using namespace Cynara;

TEST_F(StorageDeserializerFixture, init_default_only) {
    using ::testing::UnorderedElementsAre;

    auto ss = std::make_shared<std::istringstream>(";0");
    StorageDeserializer deserializer(ss, nullStreamOpener);

    Buckets buckets;
    deserializer.initBuckets(buckets);

    ASSERT_THAT(buckets, UnorderedElementsAre(
        COMPARE_BUCKETS("", PolicyBucket("", PredefinedPolicyType::DENY))
    ));
}

TEST_F(StorageDeserializerFixture, init_more) {
    using ::testing::UnorderedElementsAre;

    auto ss = std::make_shared<std::istringstream>(";0\n"
                                                   "bucket2;0\n"
                                                   "bucket3;0xFFFE;bucket2\n");

    StorageDeserializer deserializer(ss, nullStreamOpener);
    Buckets buckets;
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

    auto ss = std::make_shared<std::istringstream>(";0x0");
    StorageDeserializer deserializer(ss, nullStreamOpener);

    Buckets buckets;
    // Default bucket has ALLOW policy as default
    buckets.insert({ "", PolicyBucket("fakeId", PredefinedPolicyType::ALLOW) });

    deserializer.initBuckets(buckets);

    // Check, if default bucket has now DENY as default policy, which would be read from stream
    ASSERT_THAT(buckets, UnorderedElementsAre(
        COMPARE_BUCKETS("", PolicyBucket("", PredefinedPolicyType::DENY))
    ));
}

TEST_F(StorageDeserializerFixture, load_buckets_plus_policies) {
    using ::testing::Pointee;
    using ::testing::Return;
    using ::testing::UnorderedElementsAre;

    Buckets buckets;
    buckets.insert({ "", PolicyBucket("", PredefinedPolicyType::DENY) });

    FakeStreamForBucketId streamOpener;
    auto streamOpenerFunc = std::bind(&FakeStreamForBucketId::streamForBucketId, &streamOpener,
                                      std::placeholders::_1);
    StorageDeserializer deserializer(nullptr, streamOpenerFunc);

    auto defaultBucketStream = std::make_shared<std::istringstream>("c;u;p;0;meta");
    auto bucketDeserializer = std::make_shared<BucketDeserializer>(defaultBucketStream);
    EXPECT_CALL(streamOpener, streamForBucketId(""))
        .WillOnce(Return(bucketDeserializer));

    deserializer.loadBuckets(buckets);

    // Check if pre-inserted bucket is still there
    ASSERT_THAT(buckets, UnorderedElementsAre(
        COMPARE_BUCKETS("", PolicyBucket("", PredefinedPolicyType::DENY))
    ));

    // Check policy was inserted into bucket
    ASSERT_THAT(buckets.at(""), UnorderedElementsAre(
       Pointee(Policy(PolicyKey("c", "u", "p"), PolicyResult(PredefinedPolicyType::DENY, "meta")))
   ));
}

TEST_F(StorageDeserializerFixture, load_buckets) {
    using ::testing::Return;

    // Pre-insert some buckets
    Buckets buckets;
    buckets.insert({ "", PolicyBucket("", PredefinedPolicyType::DENY) });
    buckets.insert({ "bucket1", PolicyBucket("bucket1", PredefinedPolicyType::DENY) });
    buckets.insert({ "bucket2", PolicyBucket("bucket2", PredefinedPolicyType::DENY) });

    FakeStreamForBucketId streamOpener;
    auto streamOpenerFunc = std::bind(&FakeStreamForBucketId::streamForBucketId, &streamOpener,
                                      std::placeholders::_1);
    StorageDeserializer deserializer(nullptr, streamOpenerFunc);

    // Check, if streamOpener was called for each bucket
    EXPECT_CALL(streamOpener, streamForBucketId(""))
        .WillOnce(Return(emptyBucketStream()));

    EXPECT_CALL(streamOpener, streamForBucketId("bucket1"))
        .WillOnce(Return(emptyBucketStream()));

    EXPECT_CALL(streamOpener, streamForBucketId("bucket2"))
        .WillOnce(Return(emptyBucketStream()));

    deserializer.loadBuckets(buckets);
}

TEST_F(StorageDeserializerFixture, load_buckets_io_error) {
    using ::testing::Return;

    // Pre-insert some buckets
    Buckets buckets;
    buckets.insert({ "", PolicyBucket("", PredefinedPolicyType::DENY) });

    FakeStreamForBucketId streamOpener;
    auto streamOpenerFunc = std::bind(&FakeStreamForBucketId::streamForBucketId, &streamOpener,
                                      std::placeholders::_1);
    StorageDeserializer deserializer(nullptr, streamOpenerFunc);

    // Check, if streamOpener was called for each bucket
    EXPECT_CALL(streamOpener, streamForBucketId(""))
        .WillOnce(Return(nullptr));

    ASSERT_THROW(deserializer.loadBuckets(buckets), BucketDeserializationException);
}
