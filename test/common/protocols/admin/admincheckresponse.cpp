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
 * @file        test/common/protocols/admin/admincheckresponse.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::AdminCheckResponse usage in Cynara::ProtocolAdmin
 */

#include <gtest/gtest.h>

#include <protocol/ProtocolAdmin.h>
#include <response/AdminCheckResponse.h>

#include <ResponseTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::AdminCheckResponse &resp1, const Cynara::AdminCheckResponse &resp2) {
    EXPECT_EQ(resp1.result(), resp2.result());
    EXPECT_EQ(resp1.isBucketValid(), resp2.isBucketValid());
    EXPECT_EQ(resp1.isDbCorrupted(), resp2.isDbCorrupted());
}

static const bool VALID_BUCKET = true;
static const bool NO_BUCKET = false;
static const bool DB_OK = false;
static const bool DB_CORRUPTED = true;

} /* anonymous namespace */

using namespace Cynara;
using namespace ResponseTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

TEST(ProtocolAdmin, AdminCheckResponse01) {
    auto response = std::make_shared<AdminCheckResponse>(Results::allow, VALID_BUCKET, DB_OK,
                                                         SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponse02) {
    auto response = std::make_shared<AdminCheckResponse>(Results::deny, NO_BUCKET, DB_OK,
                                                         SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponse03) {
    auto response = std::make_shared<AdminCheckResponse>(Results::bucket_empty, VALID_BUCKET, DB_OK,
                                                         SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponse04) {
    auto response = std::make_shared<AdminCheckResponse>(Results::bucket_not_empty, NO_BUCKET,
                                                         DB_OK, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponse05) {
    auto response = std::make_shared<AdminCheckResponse>(Results::none, VALID_BUCKET, DB_OK,
                                                         SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponse06) {
    auto response = std::make_shared<AdminCheckResponse>(Results::plugin_1, NO_BUCKET, DB_OK,
                                                         SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponse07) {
    auto response = std::make_shared<AdminCheckResponse>(Results::plugin_2, VALID_BUCKET, DB_OK,
                                                         SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if AdminCheckResponse is properly (de)serialized while database is corrupted
 * @test    Expected result:
 * - PolicyResult set to DENY
 * - bucketValid flag set to false (NO_BUCKET)
 * - dbCorrupted flag set to true (DB_CORRUPTED)
 */
TEST(ProtocolAdmin, AdminCheckResponse08) {
    auto response = std::make_shared<AdminCheckResponse>(Results::deny, NO_BUCKET, DB_CORRUPTED,
                                                         SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

/* *** compare by serialized data test cases *** */

TEST(ProtocolAdmin, AdminCheckResponseBinary01) {
    auto response = std::make_shared<AdminCheckResponse>(Results::allow, VALID_BUCKET, DB_OK,
                                                         SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary02) {
    auto response = std::make_shared<AdminCheckResponse>(Results::deny, NO_BUCKET, DB_OK,
                                                         SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary03) {
    auto response = std::make_shared<AdminCheckResponse>(Results::bucket_empty, VALID_BUCKET, DB_OK,
                                                         SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary04) {
    auto response = std::make_shared<AdminCheckResponse>(Results::bucket_not_empty, NO_BUCKET,
                                                         DB_OK, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary05) {
    auto response = std::make_shared<AdminCheckResponse>(Results::none, VALID_BUCKET, DB_OK,
                                                         SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary06) {
    auto response = std::make_shared<AdminCheckResponse>(Results::plugin_1, NO_BUCKET, DB_OK,
                                                         SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary07) {
    auto response = std::make_shared<AdminCheckResponse>(Results::plugin_2, VALID_BUCKET, DB_OK,
                                                         SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if AdminCheckResponse is properly (de)serialized while database is corrupted
 * @test    Expected result:
 * - PolicyResult set to DENY
 * - bucketValid flag set to false (NO_BUCKET)
 * - dbCorrupted flag set to true (DB_CORRUPTED)
 */
TEST(ProtocolAdmin, AdminCheckResponseBinary08) {
    auto response = std::make_shared<AdminCheckResponse>(Results::deny, NO_BUCKET, DB_CORRUPTED,
                                                         SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}
