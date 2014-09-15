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
 * @file        search.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests of search in InMemeoryStorageBackend
 */

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "types/PolicyBucket.h"

#include "../../helpers.h"
#include "fakeinmemorystoragebackend.h"
#include "inmemeorystoragebackendfixture.h"

using namespace Cynara;

TEST_F(InMemeoryStorageBackendFixture, searchDefault) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAreArray;
    using ::testing::IsEmpty;

    auto pk1 = Helpers::generatePolicyKey("1");
    auto pk2 = Helpers::generatePolicyKey("2");
    auto pk3 = PolicyKey(PolicyKeyFeature::createWildcard(), pk1.user(), pk1.privilege());
    auto otherPk = Helpers::generatePolicyKey("-");

    PolicyCollection policies = {
       Policy::simpleWithKey(pk1, PredefinedPolicyType::ALLOW),
       Policy::simpleWithKey(pk2, PredefinedPolicyType::DENY),
       Policy::simpleWithKey(pk3, PredefinedPolicyType::DENY),
    };

    createBucket(defaultPolicyBucketId, policies);

    // Just override buckets() accessor
    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    auto policiesToStay1 = Helpers::pickFromCollection(policies, { 0, 2 });
    auto policiesToStay2 = Helpers::pickFromCollection(policies, { 1 });

    ASSERT_THAT(backend.searchDefaultBucket(pk1), UnorderedElementsAreArray(policiesToStay1));
    ASSERT_THAT(backend.searchDefaultBucket(pk2), UnorderedElementsAreArray(policiesToStay2));
    ASSERT_THAT(backend.searchDefaultBucket(otherPk), IsEmpty());
}
