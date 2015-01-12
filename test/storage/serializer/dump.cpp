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
 * @file        test/storage/serializer/dump.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for dumping feature of Cynara::StorageSerializer
 */


#include <sstream>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <storage/StorageSerializer.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyKey.h>
#include <types/PolicyType.h>

#include "../../helpers.h"

using namespace Cynara;

static std::string expectedPolicyKey(const PolicyKey &key) {
    return key.client().toString() + ";" + key.user().toString() + ";" + key.privilege().toString();
}

static std::string expectedPolicyType(const PolicyType &type) {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << "0x" << type;
    return oss.str();
}

TEST(serializer_dump, dump_empty_bucket) {
    auto oss = std::make_shared<std::ostringstream>();
    PolicyBucket bucket("empty");

    StorageSerializer<std::ostringstream> serializer(oss);
    serializer.dump(bucket);

    ASSERT_EQ("", oss->str());
}

TEST(serializer_dump, dump_bucket) {
    using ::testing::UnorderedElementsAreArray;
    using PredefinedPolicyType::ALLOW;
    using PredefinedPolicyType::DENY;

    PolicyKey pk1 = Helpers::generatePolicyKey("1");
    PolicyKey pk2 = Helpers::generatePolicyKey("2");

    PolicyBucket bucket("dump_bucket", PolicyCollection({ Policy::simpleWithKey(pk1, ALLOW),
                                                          Policy::simpleWithKey(pk2, DENY) }));

    auto outStream = std::make_shared<std::stringstream>();
    StorageSerializer<std::stringstream> serializer(outStream);
    serializer.dump(bucket);

    // Split stream into records
    auto actualRecords = std::vector<std::string>(std::istream_iterator<std::string>(*outStream),
                                                  std::istream_iterator<std::string>());

    std::vector<std::string> expectedRecords = {
        expectedPolicyKey(pk1) + ";" + expectedPolicyType(ALLOW) + ";",
        expectedPolicyKey(pk2) + ";" + expectedPolicyType(DENY) + ";"
    };

    ASSERT_THAT(actualRecords, UnorderedElementsAreArray(expectedRecords));
}

TEST(serializer_dump, dump_bucket_bucket) {
    using ::testing::UnorderedElementsAreArray;
    using PredefinedPolicyType::BUCKET;
    using PredefinedPolicyType::DENY;

    PolicyKey pk1 = Helpers::generatePolicyKey("1");
    PolicyKey pk2 = Helpers::generatePolicyKey("2");
    PolicyKey pk3 = Helpers::generatePolicyKey("3");
    PolicyBucketId bucketId = Helpers::generateBucketId();

    PolicyBucket bucket = {"dump_bucket_bucket", { Policy::bucketWithKey(pk1, bucketId),
                                                   Policy::simpleWithKey(pk2, DENY),
                                                   Policy::bucketWithKey(pk3, bucketId) }};

    auto outStream = std::make_shared<std::stringstream>();
    StorageSerializer<std::stringstream> serializer(outStream);
    serializer.dump(bucket);

    // Split stream into records
    auto actualRecords = std::vector<std::string>(std::istream_iterator<std::string>(*outStream),
                                                  std::istream_iterator<std::string>());

    std::vector<std::string> expectedRecords = {
        expectedPolicyKey(pk1) + ";" + expectedPolicyType(BUCKET) + ";" + bucketId,
        expectedPolicyKey(pk2) + ";" + expectedPolicyType(DENY) + ";",
        expectedPolicyKey(pk3) + ";" + expectedPolicyType(BUCKET) + ";" + bucketId
    };

    ASSERT_THAT(actualRecords, UnorderedElementsAreArray(expectedRecords));
}
