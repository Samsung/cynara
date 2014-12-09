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
 * @file        test/cyad/policy_collection.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for CynaraAdminPolicies
 */

#include <exception>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cynara-admin-types.h>
#include <cynara-policy-types.h>

#include <cyad/CynaraAdminPolicies.h>

#include "helpers.h"

TEST(CynaraAdminPolicies, notSealed) {
    Cynara::CynaraAdminPolicies policies;
    ASSERT_THROW(policies.data(), std::logic_error);
}

TEST(CynaraAdminPolicies, sealEmpty) {
    Cynara::CynaraAdminPolicies policies;
    policies.seal();
    ASSERT_EQ(nullptr, policies.data()[0]);
}

TEST(CynaraAdminPolicies, addToSealed) {
    Cynara::CynaraAdminPolicies policies;
    policies.seal();
    ASSERT_THROW(policies.add("", { CYNARA_ADMIN_ALLOW, "" }, { "", "", ""} ), std::logic_error);
}

TEST(CynaraAdminPolicies, addOne) {
    using ::testing::ElementsAreArray;

    Cynara::CynaraAdminPolicies policies;
    policies.add("test-bucket", { CYNARA_ADMIN_ALLOW, "" }, { "client", "user", "privilege"} );
    policies.seal();
    ASSERT_NO_THROW(policies.data());

    cynara_admin_policy policy = { strdup("test-bucket"), strdup("client"), strdup("user"),
                                   strdup("privilege"), CYNARA_ADMIN_ALLOW, nullptr };

    ASSERT_EQ(policy, *policies.data()[0]);
    ASSERT_EQ(nullptr, policies.data()[1]);

    Cynara::Helpers::freeAdminPolicyMembers(&policy);
}
