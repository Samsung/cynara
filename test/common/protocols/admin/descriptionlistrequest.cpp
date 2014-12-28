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
 * @file        test/common/protocols/admin/descriptionlistrequest.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::DescriptionListRequest usage in Cynara::ProtocolAdmin
 */

#include <gtest/gtest.h>

#include <protocol/ProtocolAdmin.h>
#include <request/DescriptionListRequest.h>

#include <RequestTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::DescriptionListRequest &req1, const Cynara::DescriptionListRequest &req2)
{
    EXPECT_EQ(req1.sequenceNumber(), req2.sequenceNumber());
}

} /* namespace anonymous */

using namespace Cynara;
using namespace RequestTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

TEST(ProtocolAdmin, DescriptionListRequest01) {
    auto request = std::make_shared<DescriptionListRequest>(SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testRequest(request, protocol);
}

/* *** compare by serialized data test cases *** */

TEST(ProtocolAdmin, DescriptionListRequestBinary01) {
    auto request = std::make_shared<DescriptionListRequest>(SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestRequest(request, protocol);
}
