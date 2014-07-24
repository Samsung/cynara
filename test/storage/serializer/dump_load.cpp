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
 * @file        dump_load.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for dump => load routine
 */

#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <storage/BucketDeserializer.h>
#include <storage/StorageSerializer.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>

#include "../../helpers.h"

using namespace Cynara;

// TODO: Move to helpers or other .h
MATCHER_P(PolicyAtPtrEq, policy, "") {
    return std::tie(policy->key(), policy->result())
        == std::tie(arg->key(), arg->result());
}

// Test if dumping and loading yields the same result
TEST(dump_load, bucket) {
    using ::testing::UnorderedElementsAre;

    PolicyKey pk1 = Helpers::generatePolicyKey("1");
    PolicyKey pk2 = Helpers::generatePolicyKey("2");
    PolicyKey pk3 = Helpers::generatePolicyKey("3");
    PolicyBucketId bucketId = Helpers::generateBucketId();
    PolicyBucket bucket = {{ Policy::bucketWithKey(pk1, bucketId),
                             Policy::simpleWithKey(pk2, PredefinedPolicyType::DENY),
                             Policy::bucketWithKey(pk3, bucketId) }};

    auto ioStream = std::make_shared<std::stringstream>();

    StorageSerializer serializer(ioStream);
    serializer.dump(bucket);

    BucketDeserializer deserializer(ioStream);
    const auto loadedPolicies = deserializer.loadPolicies();
    const auto &expectedPolicies = bucket.policyCollection();

    ASSERT_THAT(loadedPolicies, UnorderedElementsAre(
        PolicyAtPtrEq(expectedPolicies.at(0)),
        PolicyAtPtrEq(expectedPolicies.at(1)),
        PolicyAtPtrEq(expectedPolicies.at(2))
    ));
}
