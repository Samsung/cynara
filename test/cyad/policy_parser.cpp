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
 * @file        test/cyad/commandline.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for AdminPolicyParser
 */

#include <memory>
#include <sstream>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/exceptions/BucketRecordCorruptedException.h>
#include <common/types/PolicyKey.h>
#include <common/types/PolicyType.h>
#include <cyad/AdminPolicyParser.h>

#include "helpers.h"

static Cynara::PolicyType translatePolicy(const std::string &rawPolicy) {
    return std::stoi(rawPolicy);
}

TEST(AdminPolicyParser, parseInvalid) {
    auto input = std::make_shared<std::stringstream>();

    *input << "invalid input" << std::endl;

    ASSERT_THROW(Cynara::AdminPolicyParser::parse(input, translatePolicy),
                 Cynara::BucketRecordCorruptedException);
}

TEST(AdminPolicyParser, parse0) {
    auto input = std::make_shared<std::stringstream>();

    Cynara::CynaraAdminPolicies expectedPolicies;
    expectedPolicies.seal();

    auto policies = Cynara::AdminPolicyParser::parse(input, translatePolicy);

    ASSERT_TRUE(policies.sealed());
    ASSERT_THAT(policies.data(), AdmPolicyListEq(expectedPolicies.data()));
}

TEST(AdminPolicyParser, parse1) {
    auto input = std::make_shared<std::stringstream>();

    *input << "b;c;u;p;0;m" << std::endl;

    Cynara::CynaraAdminPolicies expectedPolicies;
    expectedPolicies.add("b", { 0, "m" }, { "c", "u", "p" });
    expectedPolicies.seal();

    auto policies = Cynara::AdminPolicyParser::parse(input, translatePolicy);

    ASSERT_TRUE(policies.sealed());
    ASSERT_THAT(policies.data(), AdmPolicyListEq(expectedPolicies.data()));
}

TEST(AdminPolicyParser, parse2) {
    auto input = std::make_shared<std::stringstream>();

    *input << "b1;c1;u1;p1;0;m1" << std::endl;
    *input << "b2;c2;u2;p2;0;m2" << std::endl;

    Cynara::CynaraAdminPolicies expectedPolicies;
    expectedPolicies.add("b1", { 0, "m1" }, { "c1", "u1", "p1" });
    expectedPolicies.add("b2", { 0, "m2" }, { "c2", "u2", "p2" });
    expectedPolicies.seal();

    auto policies = Cynara::AdminPolicyParser::parse(input, translatePolicy);

    ASSERT_TRUE(policies.sealed());
    ASSERT_THAT(policies.data(), AdmPolicyListEq(expectedPolicies.data()));
}
