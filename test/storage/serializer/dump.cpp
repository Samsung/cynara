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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "storage/StorageSerializer.h"
#include "types/PolicyBucket.h"
#include "types/PolicyType.h"
#include "types/PolicyKey.h"
#include "types/Policy.h"

#include "../../helpers.h"

#include <sstream>

using namespace Cynara;

TEST(serializer_dump, dump_empty_bucket) {
    std::ostringstream oss;
    PolicyBucket bucket;

    StorageSerializer serializer(oss);
    serializer.dump(bucket);

    ASSERT_EQ("", oss.str());
}

TEST(serializer_dump, dump_bucket) {
    PolicyKey pk1 = Helpers::generatePolicyKey("1");
    PolicyKey pk2 = Helpers::generatePolicyKey("2");

    PolicyBucket bucket = {{ Policy::simpleWithKey(pk1, PredefinedPolicyType::ALLOW),
                             Policy::simpleWithKey(pk2, PredefinedPolicyType::DENY) }};

    std::ostringstream outputStream;
    StorageSerializer serializer(outputStream);
    serializer.dump(bucket);

    // TODO: Cynara::PolicyCollection is a vector, but in future version this may change
    // and so, we should not expect the exact order of records in serialized stream
    // See: StorageSerializerFixture::dump_buckets in serialize.cpp
    std::stringstream expected;
    expected
        << std::hex << std::uppercase
        << pk1.toString() << ";" << "0x" << PredefinedPolicyType::ALLOW << ";" << "\n"
        << pk2.toString() << ";" << "0x" << PredefinedPolicyType::DENY << ";" << "\n";

    ASSERT_EQ(expected.str(), outputStream.str());
}

TEST(serializer_dump, dump_bucket_bucket) {
    PolicyKey pk1 = Helpers::generatePolicyKey("1");
    PolicyKey pk2 = Helpers::generatePolicyKey("2");
    PolicyKey pk3 = Helpers::generatePolicyKey("3");
    PolicyBucketId bucketId = Helpers::generateBucketId();

    PolicyBucket bucket = {{ Policy::bucketWithKey(pk1, bucketId),
                             Policy::simpleWithKey(pk2, PredefinedPolicyType::DENY),
                             Policy::bucketWithKey(pk3, bucketId) }};

    std::ostringstream outputStream;
    StorageSerializer serializer(outputStream);
    serializer.dump(bucket);

    // TODO: Cynara::PolicyCollection is a vector, but in future version this may change
    // and so, we should not expect the exact order of records in serialized stream
    // See: StorageSerializerFixture::dump_buckets in serialize.cpp
    std::stringstream expected;
    expected
        << std::hex << std::uppercase
        << pk1.toString() << ";" << "0x" << PredefinedPolicyType::BUCKET << ";" << bucketId << "\n"
        << pk2.toString() << ";" << "0x" << PredefinedPolicyType::DENY << ";" << "\n"
        << pk3.toString() << ";" << "0x" << PredefinedPolicyType::BUCKET << ";" << bucketId << "\n";

    ASSERT_EQ(expected.str(), outputStream.str());
}
