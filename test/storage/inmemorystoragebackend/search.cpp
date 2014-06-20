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
 * @file        search.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests of search in InMemeoryStorageBackend
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "inmemeorystoragebackendfixture.h"
#include "fakeinmemorystoragebackend.h"

#include "common/types/PolicyBucket.h"

#include "../../helpers.h"

#include <memory>

using namespace Cynara;

TEST_F(InMemeoryStorageBackendFixture, searchDefault) {
    using ::testing::ReturnRef;
    using ::testing::UnorderedElementsAre;
    using ::testing::IsEmpty;

    auto pk1 = Helpers::generatePolicyKey("1");
    auto pk2 = Helpers::generatePolicyKey("2");
    auto otherPk = Helpers::generatePolicyKey("-");

    const auto &defaultBucket = createBucket(defaultPolicyBucketId);

    addToBucket(defaultPolicyBucketId, {
       Policy::simpleWithKey(pk1, PredefinedPolicyType::ALLOW),
       Policy::simpleWithKey(pk2, PredefinedPolicyType::DENY),
       Policy::simpleWithKey(pk1, PredefinedPolicyType::DENY),
    });

    // Just override buckets() accessor
    FakeInMemoryStorageBackend backend;
    EXPECT_CALL(backend, buckets())
        .WillRepeatedly(ReturnRef(m_buckets));

    auto searchDefaultBucket = [&backend](const PolicyKey &key) -> PolicyCollection {
        return backend.searchDefaultBucket(key).policyCollection();
    };

    auto defaultPolicyAt = [&defaultBucket] (PolicyCollection::size_type idx) {
        return defaultBucket.policyCollection().at(idx);
    };

    ASSERT_THAT(searchDefaultBucket(pk1),
            UnorderedElementsAre(defaultPolicyAt(0),defaultPolicyAt(2)));
    ASSERT_THAT(searchDefaultBucket(pk2), UnorderedElementsAre(defaultPolicyAt(1)));
    ASSERT_THAT(searchDefaultBucket(otherPk), IsEmpty());
}
