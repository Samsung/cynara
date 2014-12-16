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
 * @file        test/common/protocols/admin/eraserequest.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::EraseRequest usage in Cynara::ProtocolAdmin
 */

#include <gtest/gtest.h>

#include <protocol/ProtocolAdmin.h>
#include <request/EraseRequest.h>

#include <RequestTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::EraseRequest &req1, const Cynara::EraseRequest &req2) {
    EXPECT_EQ(req1.startBucket(), req2.startBucket());
    EXPECT_EQ(req1.recursive(), req2.recursive());
    EXPECT_EQ(req1.filter(), req2.filter());
}

static const bool RECURSIVE = true;
static const bool NON_RECURSIVE = false;

} /* namespace anonymous */

using namespace Cynara;
using namespace RequestTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

TEST(ProtocolAdmin, EraseRequest01) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, RECURSIVE, Keys::k_nun, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequest02) {
    auto request = std::make_shared<EraseRequest>(Buckets::not_empty, NON_RECURSIVE, Keys::k_cup,
                                                  SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequest03) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, NON_RECURSIVE, Keys::k_www,
                                                  SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequest04) {
    auto request = std::make_shared<EraseRequest>(Buckets::not_empty, RECURSIVE, Keys::k_wuw,
                                                  SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequest05) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, RECURSIVE, Keys::k_aaa,
                                                  SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequest06) {
    auto request = std::make_shared<EraseRequest>(Buckets::not_empty, NON_RECURSIVE, Keys::k_wua,
                                                  SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequest07) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, NON_RECURSIVE, Keys::k_nua,
                                                  SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

/* *** compare by serialized data test cases *** */

TEST(ProtocolAdmin, EraseRequestBinary01) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, RECURSIVE, Keys::k_nun, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequestBinary02) {
    auto request = std::make_shared<EraseRequest>(Buckets::not_empty, NON_RECURSIVE, Keys::k_cup,
                                                  SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequestBinary03) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, NON_RECURSIVE, Keys::k_www,
                                                  SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequestBinary04) {
    auto request = std::make_shared<EraseRequest>(Buckets::not_empty, RECURSIVE, Keys::k_wuw,
                                                  SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequestBinary05) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, RECURSIVE, Keys::k_aaa,
                                                  SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequestBinary06) {
    auto request = std::make_shared<EraseRequest>(Buckets::not_empty, NON_RECURSIVE, Keys::k_wua,
                                                  SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, EraseRequestBinary07) {
    auto request = std::make_shared<EraseRequest>(Buckets::empty, NON_RECURSIVE, Keys::k_nua,
                                                  SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}
