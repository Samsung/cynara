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
 * @file        test/common/protocols/admin/admincheckrequest.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::AdminCheckRequest usage in Cynara::ProtocolAdmin
 */

#include <gtest/gtest.h>

#include <protocol/ProtocolAdmin.h>
#include <request/AdminCheckRequest.h>

#include <RequestTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::AdminCheckRequest &req1, const Cynara::AdminCheckRequest &req2) {
    EXPECT_EQ(req1.key(), req2.key());
    EXPECT_EQ(req1.startBucket(), req2.startBucket());
    EXPECT_EQ(req1.recursive(), req2.recursive());
}

static const bool RECURSIVE = true;
static const bool NON_RECURSIVE = false;

} /* anonymous namespace */

using namespace Cynara;
using namespace RequestTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

TEST(ProtocolAdmin, AdminCheckRequest01) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_nun, Buckets::empty, RECURSIVE,
                                                       SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequest02) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_cup, Buckets::not_empty, RECURSIVE,
                                                       SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequest03) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_www, Buckets::empty, NON_RECURSIVE,
                                                       SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequest04) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_wuw, Buckets::not_empty,
                                                       NON_RECURSIVE, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequest05) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_aaa, Buckets::empty, RECURSIVE,
                                                       SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequest06) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_wua, Buckets::not_empty, RECURSIVE,
                                                       SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequest07) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_nua, Buckets::empty, NON_RECURSIVE,
                                                       SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

/* *** compare by serialized data test cases *** */

TEST(ProtocolAdmin, AdminCheckRequestBinary01) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_nun, Buckets::empty, RECURSIVE,
                                                       SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary02) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_cup, Buckets::not_empty, RECURSIVE,
                                                       SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary03) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_www, Buckets::empty, NON_RECURSIVE,
                                                       SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary04) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_wuw, Buckets::not_empty,
                                                       NON_RECURSIVE, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary05) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_aaa, Buckets::empty, RECURSIVE,
                                                       SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary06) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_wua, Buckets::not_empty, RECURSIVE,
                                                       SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary07) {
    auto request = std::make_shared<AdminCheckRequest>(Keys::k_nua, Buckets::empty, NON_RECURSIVE,
                                                       SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}
