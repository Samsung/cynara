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
 * @file        test/types/policykey.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::PolicyKey
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../helpers.h"

#include <types/PolicyKey.h>
#include <cynara-admin-types.h>

using namespace Cynara;

TEST(PolicyKey, to_string) {
    typedef Cynara::PolicyKeyFeature PKF;

    PolicyKey pk1 = Helpers::generatePolicyKey();
    ASSERT_EQ("c\tu\tp", pk1.toString());

    PolicyKey pk2(PKF::createWildcard(), PKF::createWildcard(), PKF::createWildcard());
    ASSERT_EQ(CYNARA_ADMIN_WILDCARD "\t" CYNARA_ADMIN_WILDCARD "\t" CYNARA_ADMIN_WILDCARD,
              pk2.toString());

    PolicyKey pk3(PKF::createWildcard(), PKF::create("u"), PKF::createWildcard());
    ASSERT_EQ(CYNARA_ADMIN_WILDCARD "\tu\t" CYNARA_ADMIN_WILDCARD, pk3.toString());

    PolicyKey pk4(PKF::createAny(), PKF::createAny(), PKF::createAny());
    ASSERT_EQ(CYNARA_ADMIN_ANY "\t" CYNARA_ADMIN_ANY "\t" CYNARA_ADMIN_ANY, pk4.toString());

    PolicyKey pk5(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    ASSERT_EQ(CYNARA_ADMIN_WILDCARD "\tu\t" CYNARA_ADMIN_ANY, pk5.toString());
}
