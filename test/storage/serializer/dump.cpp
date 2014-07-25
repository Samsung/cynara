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
 * @file        dump.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
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
    PolicyBucket bucket;

    StorageSerializer serializer(oss);
    serializer.dump(bucket);

    ASSERT_EQ("", oss->str());
}

TEST(serializer_dump, dump_bucket) {
    PolicyKey pk1 = Helpers::generatePolicyKey("1");
    PolicyKey pk2 = Helpers::generatePolicyKey("2");

    PolicyBucket bucket = {{ Policy::simpleWithKey(pk1, PredefinedPolicyType::ALLOW),
                             Policy::simpleWithKey(pk2, PredefinedPolicyType::DENY) }};

    auto outputStream = std::make_shared<std::ostringstream>();
    StorageSerializer serializer(outputStream);
    serializer.dump(bucket);

    // TODO: Cynara::PolicyCollection is a vector, but in future version this may change
    // and so, we should not expect the exact order of records in serialized stream
    // See: StorageSerializerFixture::dump_buckets in serialize.cpp
    std::stringstream expected;
    expected
        << expectedPolicyKey(pk1) << ";" << expectedPolicyType(PredefinedPolicyType::ALLOW)<< ";"
        << std::endl
        << expectedPolicyKey(pk2) << ";" << expectedPolicyType(PredefinedPolicyType::DENY) << ";"
        << std::endl;

    ASSERT_EQ(expected.str(), outputStream->str());
}

TEST(serializer_dump, dump_bucket_bucket) {
    PolicyKey pk1 = Helpers::generatePolicyKey("1");
    PolicyKey pk2 = Helpers::generatePolicyKey("2");
    PolicyKey pk3 = Helpers::generatePolicyKey("3");
    PolicyBucketId bucketId = Helpers::generateBucketId();

    PolicyBucket bucket = {{ Policy::bucketWithKey(pk1, bucketId),
                             Policy::simpleWithKey(pk2, PredefinedPolicyType::DENY),
                             Policy::bucketWithKey(pk3, bucketId) }};

    auto outputStream = std::make_shared<std::ostringstream>();
    StorageSerializer serializer(outputStream);
    serializer.dump(bucket);

    // TODO: Cynara::PolicyCollection is a vector, but in future version this may change
    // and so, we should not expect the exact order of records in serialized stream
    // See: StorageSerializerFixture::dump_buckets in serialize.cpp
    std::stringstream expected;
    expected
        << expectedPolicyKey(pk1) << ";" << expectedPolicyType(PredefinedPolicyType::BUCKET) << ";"
        << bucketId << std::endl
        << expectedPolicyKey(pk2) << ";" << expectedPolicyType(PredefinedPolicyType::DENY) << ";"
        << std::endl
        << expectedPolicyKey(pk3) << ";" << expectedPolicyType(PredefinedPolicyType::BUCKET) << ";"
        << bucketId << std::endl;

    ASSERT_EQ(expected.str(), outputStream->str());
}
