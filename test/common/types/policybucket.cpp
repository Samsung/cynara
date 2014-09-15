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
 * @file        test/common/types/policybucket.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::PolicyBucket
 */

#include <algorithm>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "types/PolicyBucket.h"
#include "types/PolicyCollection.h"
#include "types/PolicyKey.h"

#include "../../helpers.h"

using namespace Cynara;

class PolicyBucketFixture : public ::testing::Test {
public:
    virtual ~PolicyBucketFixture() {}

protected:
    const PolicyKey pk1 = Helpers::generatePolicyKey("1");
    const PolicyKey pk2 = Helpers::generatePolicyKey("2");
    const PolicyKey pk3 = Helpers::generatePolicyKey("3");
    const PolicyKey otherPk = Helpers::generatePolicyKey("_");

    const PolicyCollection pkPolicies = {
        Policy::simpleWithKey(pk1, PredefinedPolicyType::ALLOW),
        Policy::simpleWithKey(pk2, PredefinedPolicyType::ALLOW),
        Policy::simpleWithKey(pk3, PredefinedPolicyType::ALLOW)
    };

    const PolicyCollection wildcardPolicies = {
        Policy::simpleWithKey(PolicyKey("c1", "u1", "*"), PredefinedPolicyType::ALLOW),
        Policy::simpleWithKey(PolicyKey("*", "u1", "p2"), PredefinedPolicyType::ALLOW),
        Policy::simpleWithKey(PolicyKey("*", "*", "p1"), PredefinedPolicyType::ALLOW),
        Policy::simpleWithKey(PolicyKey("*", "*", "*"), PredefinedPolicyType::ALLOW)
    };
};

TEST_F(PolicyBucketFixture, filtered) {
    using ::testing::UnorderedElementsAre;

    PolicyBucket bucket(pkPolicies);
    bucket.setDefaultPolicy(PredefinedPolicyType::DENY);
    auto filtered = bucket.filtered(pk1);

    // Elements match
    ASSERT_THAT(filtered, UnorderedElementsAre(pkPolicies.at(0)));

    // default policy matches
    ASSERT_EQ(PredefinedPolicyType::DENY, filtered.defaultPolicy());
}

TEST_F(PolicyBucketFixture, filtered_other) {
    using ::testing::IsEmpty;

    PolicyBucket bucket(pkPolicies);
    bucket.setDefaultPolicy(PredefinedPolicyType::DENY);
    auto filtered = bucket.filtered(otherPk);

    // No policies should be found
    ASSERT_THAT(filtered, IsEmpty());

    // default policy should be preserved
    ASSERT_EQ(PredefinedPolicyType::DENY, filtered.defaultPolicy());
}

TEST_F(PolicyBucketFixture, filtered_wildcard_1) {
    using ::testing::UnorderedElementsAreArray;

    // Leave policies with given client, given user and any privilege
    auto policiesToStay = Helpers::pickFromCollection(wildcardPolicies, { 0, 1, 3 });

    PolicyBucket bucket(wildcardPolicies);
    auto filtered = bucket.filtered(PolicyKey("c1", "u1", "p2"));
    ASSERT_THAT(filtered, UnorderedElementsAreArray(policiesToStay));
}

TEST_F(PolicyBucketFixture, filtered_wildcard_2) {
    using ::testing::UnorderedElementsAreArray;

    // Leave policies with given client, given user and any privilege
    auto policiesToStay = Helpers::pickFromCollection(wildcardPolicies, { 2, 3 });

    PolicyBucket bucket(wildcardPolicies);
    auto filtered = bucket.filtered(PolicyKey("cccc", "u1", "p1"));

    ASSERT_THAT(filtered, UnorderedElementsAreArray(policiesToStay));
}

TEST_F(PolicyBucketFixture, filtered_wildcard_3) {
    using ::testing::UnorderedElementsAreArray;

    // Leave policies with given client, given user and any privilege
    auto policiesToStay = Helpers::pickFromCollection(wildcardPolicies, { 0, 3 });

    PolicyBucket bucket(wildcardPolicies);
    auto filtered = bucket.filtered(PolicyKey("c1", "u1", "pppp"));
    ASSERT_THAT(filtered, UnorderedElementsAreArray(policiesToStay));
}

TEST_F(PolicyBucketFixture, filtered_wildcard_4) {
    using ::testing::UnorderedElementsAreArray;

    // Leave policies with given client, given user and any privilege
    auto policiesToStay = Helpers::pickFromCollection(wildcardPolicies, { 3 });

    PolicyBucket bucket(wildcardPolicies);
    auto filtered = bucket.filtered(PolicyKey("cccc", "uuuu", "pppp"));
    ASSERT_THAT(filtered, UnorderedElementsAreArray(policiesToStay));
}

TEST_F(PolicyBucketFixture, filtered_wildcard_none) {
    using ::testing::IsEmpty;

    PolicyBucket bucket({ wildcardPolicies.begin(), wildcardPolicies.begin() + 3 });
    auto filtered = bucket.filtered(PolicyKey("cccc", "uuuu", "pppp"));
    ASSERT_THAT(filtered, IsEmpty());
}
