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
 * @file        test/common/protocols/admin/listresponse.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::ListResponse usage in Cynara::ProtocolAdmin
 */

#include <vector>

#include <gtest/gtest.h>

#include <protocol/ProtocolAdmin.h>
#include <response/ListResponse.h>
#include <types/Policy.h>

#include <ResponseTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::ListResponse &resp1, const Cynara::ListResponse &resp2) {
    EXPECT_EQ(resp1.policies(), resp2.policies());
    EXPECT_EQ(resp1.isBucketValid(), resp2.isBucketValid());
    EXPECT_EQ(resp1.isDbCorrupted(), resp2.isDbCorrupted());
}

static const bool VALID_BUCKET = true;
static const bool NO_BUCKET = false;
static const bool DB_OK = false;
static const bool DB_CORRUPTED = true;

} /* namespace anonymous */

using namespace Cynara;
using namespace ResponseTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

TEST(ProtocolAdmin, ListResponse01) {
    std::vector<Policy> policies = {
        Policy(Keys::k_nun, Results::allow),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponse02) {
    std::vector<Policy> policies = {
        Policy(Keys::k_cup, Results::deny),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponse03) {
    std::vector<Policy> policies = {
        Policy(Keys::k_www, Results::bucket_empty),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponse04) {
    std::vector<Policy> policies = {
        Policy(Keys::k_wuw, Results::bucket_not_empty),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponse05) {
    std::vector<Policy> policies = {
        Policy(Keys::k_aaa, Results::none),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponse06) {
    std::vector<Policy> policies = {
        Policy(Keys::k_wua, Results::plugin_1),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponse07) {
    std::vector<Policy> policies = {
        Policy(Keys::k_nua, Results::plugin_2),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseMultiplePolicies) {
    std::vector<Policy> policies = {
        Policy(Keys::k_nun, Results::allow),
        Policy(Keys::k_cup, Results::deny),
        Policy(Keys::k_www, Results::bucket_empty),
        Policy(Keys::k_wuw, Results::bucket_not_empty),
        Policy(Keys::k_aaa, Results::none),
        Policy(Keys::k_wua, Results::plugin_1),
        Policy(Keys::k_nua, Results::plugin_2),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseEmptyPolicies) {
    std::vector<Policy> policies;

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseNoBucket) {
    std::vector<Policy> policies;

    auto response = std::make_shared<ListResponse>(policies, NO_BUCKET, DB_OK, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized while database is corrupted
 * @test    Expected result:
 * - policies vector is empty
 * - bucketValid flag set to false (NO_BUCKET)
 * - dbCorrupted flag set to true (DB_CORRUPTED)
 */
TEST(ProtocolAdmin, ListResponseDatabaseCorrupted) {
    std::vector<Policy> policies;

    auto response = std::make_shared<ListResponse>(policies, NO_BUCKET, DB_CORRUPTED, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

/* *** compare by serialized data test cases *** */

TEST(ProtocolAdmin, ListResponseBinary01) {
    std::vector<Policy> policies = {
        Policy(Keys::k_nun, Results::allow),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinary02) {
    std::vector<Policy> policies = {
        Policy(Keys::k_cup, Results::deny),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinary03) {
    std::vector<Policy> policies = {
        Policy(Keys::k_www, Results::bucket_empty),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinary04) {
    std::vector<Policy> policies = {
        Policy(Keys::k_wuw, Results::bucket_not_empty),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinary05) {
    std::vector<Policy> policies = {
        Policy(Keys::k_aaa, Results::none),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinary06) {
    std::vector<Policy> policies = {
        Policy(Keys::k_wua, Results::plugin_1),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinary07) {
    std::vector<Policy> policies = {
        Policy(Keys::k_nua, Results::plugin_2),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinaryMultiplePolicies) {
    std::vector<Policy> policies = {
        Policy(Keys::k_nun, Results::allow),
        Policy(Keys::k_cup, Results::deny),
        Policy(Keys::k_www, Results::bucket_empty),
        Policy(Keys::k_wuw, Results::bucket_not_empty),
        Policy(Keys::k_aaa, Results::none),
        Policy(Keys::k_wua, Results::plugin_1),
        Policy(Keys::k_nua, Results::plugin_2),
    };

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinaryEmptyPolicies) {
    std::vector<Policy> policies;

    auto response = std::make_shared<ListResponse>(policies, VALID_BUCKET, DB_OK, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, ListResponseBinaryNoBucket) {
    std::vector<Policy> policies;

    auto response = std::make_shared<ListResponse>(policies, NO_BUCKET, DB_OK, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized while database is corrupted
 * @test    Expected result:
 * - policies vector is empty
 * - bucketValid flag set to false (NO_BUCKET)
 * - dbCorrupted flag set to true (DB_CORRUPTED)
 */
TEST(ProtocolAdmin, ListResponseBinaryDatabaseCorrupted) {
    std::vector<Policy> policies;

    auto response = std::make_shared<ListResponse>(policies, NO_BUCKET, DB_CORRUPTED, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}
