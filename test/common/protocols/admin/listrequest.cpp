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
 * @file        test/common/protocols/admin/listrequest.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::ListRequest usage in Cynara::ProtocolAdmin
 */

#include <gtest/gtest.h>

#include <protocol/ProtocolAdmin.h>
#include <request/ListRequest.h>

#include <RequestTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::ListRequest &req1, const Cynara::ListRequest &req2) {
    EXPECT_EQ(req1.bucket(), req2.bucket());
    EXPECT_EQ(req1.filter(), req2.filter());
}

} /* namespace anonymous */

using namespace Cynara;
using namespace RequestTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

TEST(ProtocolAdmin, ListRequest01) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_nun, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequest02) {
    auto request = std::make_shared<ListRequest>(Buckets::not_empty, Keys::k_cup, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequest03) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_www, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequest04) {
    auto request = std::make_shared<ListRequest>(Buckets::not_empty, Keys::k_wuw, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequest05) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_aaa, SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequest06) {
    auto request = std::make_shared<ListRequest>(Buckets::not_empty, Keys::k_wua, SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequest07) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_nua, SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

/* *** compare by serialized data test cases *** */

TEST(ProtocolAdmin, ListRequestBinary01) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_nun, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequestBinary02) {
    auto request = std::make_shared<ListRequest>(Buckets::not_empty, Keys::k_cup, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequestBinary03) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_www, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequestBinary04) {
    auto request = std::make_shared<ListRequest>(Buckets::not_empty, Keys::k_wuw, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequestBinary05) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_aaa, SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequestBinary06) {
    auto request = std::make_shared<ListRequest>(Buckets::not_empty, Keys::k_wua, SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}

TEST(ProtocolAdmin, ListRequestBinary07) {
    auto request = std::make_shared<ListRequest>(Buckets::empty, Keys::k_nua, SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}
