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
 * @file        test/storage/serializer/serialize.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for dumping feature of Cynara::StorageSerializer
 */

#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <exceptions/BucketSerializationException.h>
#include <storage/Buckets.h>
#include <storage/InMemoryStorageBackend.h>
#include <storage/StorageSerializer.h>
#include <types/PolicyBucketId.h>

class FakeStreamForBucketId {
public:
    MOCK_METHOD1(streamForBucketId,
                 std::shared_ptr<Cynara::StorageSerializer>(const Cynara::PolicyBucketId &));

    Cynara::StorageSerializer::BucketStreamOpener streamOpener() {
        return std::bind(&FakeStreamForBucketId::streamForBucketId, this, std::placeholders::_1);
    }
};

// Fake StorageSerializer for Cynara::PolicyBucket
class FakeStorageSerializer : public Cynara::StorageSerializer {
public:
    FakeStorageSerializer() : Cynara::StorageSerializer(outStream),
                              outStream(new std::ostringstream()) {}
    MOCK_METHOD1(dump, void(const Cynara::PolicyBucket &bucket));
    std::shared_ptr<std::ostringstream> outStream;
};

class StorageSerializerFixture : public ::testing::Test {
public:
    virtual ~StorageSerializerFixture() {};

    Cynara::Buckets buckets;
    FakeStreamForBucketId fakeStreamOpener;
};

using namespace Cynara;

// Be sure no calls to streamForBucketId() are made
// and output stream is not touched
TEST_F(StorageSerializerFixture, dump_buckets_empty) {
    auto outStream = std::make_shared<std::ostringstream>();
    StorageSerializer serializer(outStream);
    serializer.dump(Buckets(), fakeStreamOpener.streamOpener());

    // Stream should be empty
    ASSERT_EQ(0, outStream->tellp());
}

TEST_F(StorageSerializerFixture, dump_buckets) {
    using ::testing::_;
    using ::testing::Property;
    using ::testing::Return;
    using ::testing::UnorderedElementsAreArray;

    // Will be returned as serializer for buckets
    auto fakeBucketSerializer = std::make_shared<FakeStorageSerializer>();

    buckets = {
        { "bucket1", PolicyBucket("bucket1", PredefinedPolicyType::DENY) },
        { "bucket2", PolicyBucket("bucket2", PredefinedPolicyType::DENY) },
        { "bucket3",
            PolicyBucket("bucket3", PolicyResult(PredefinedPolicyType::BUCKET, "bucket2")) }
    };

    auto outStream = std::make_shared<std::stringstream>();
    StorageSerializer dbSerializer(outStream);

    // Make sure stream was opened for each bucket
    EXPECT_CALL(fakeStreamOpener, streamForBucketId(_))
        .Times(buckets.size()).WillRepeatedly(Return(fakeBucketSerializer));

    // Make sure every bucket was dumped
    for (const auto &bucket : buckets) {
        EXPECT_CALL(*fakeBucketSerializer, dump(Property(&PolicyBucket::id, bucket.first)));
    }

    dbSerializer.dump(buckets, fakeStreamOpener.streamOpener());

    std::vector<std::string> expectedRecords = {
        "bucket1;0x0;",
        "bucket2;0x0;",
        "bucket3;0xFFFE;bucket2"
    };

    // Split stream into records
    auto actualRecords = std::vector<std::string>(std::istream_iterator<std::string>(*outStream),
                                                  std::istream_iterator<std::string>());

    ASSERT_THAT(actualRecords, UnorderedElementsAreArray(expectedRecords));
}

TEST_F(StorageSerializerFixture, dump_buckets_io_error) {
    using ::testing::_;
    using ::testing::Return;

    buckets = {
        { "bucket1", PolicyBucket("bucket1", PredefinedPolicyType::DENY) },
    };

    auto outStream = std::make_shared<std::stringstream>();
    StorageSerializer dbSerializer(outStream);

    // Make sure stream was opened for each bucket
    EXPECT_CALL(fakeStreamOpener, streamForBucketId(_))
        .Times(buckets.size()).WillRepeatedly(Return(nullptr));

    ASSERT_THROW(
        dbSerializer.dump(buckets, fakeStreamOpener.streamOpener()),
        BucketSerializationException
    );
}
