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
 * @file        test/common/protocols/admin/descriptionlistresponse.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::DescriptionListResponse usage in Cynara::ProtocolAdmin
 */

#include <vector>

#include <gtest/gtest.h>

#include <protocol/ProtocolAdmin.h>
#include <response/DescriptionListResponse.h>
#include <types/PolicyDescription.h>

#include <ResponseTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::DescriptionListResponse &resp1,
             const Cynara::DescriptionListResponse &resp2) {
    ASSERT_EQ(resp1.descriptions().size(), resp2.descriptions().size());
    for (size_t i = 0U; i < resp1.descriptions().size(); ++i) {
        SCOPED_TRACE(std::to_string(i));
        EXPECT_EQ(resp1.descriptions()[i].name, resp2.descriptions()[i].name);
        EXPECT_EQ(resp1.descriptions()[i].type, resp2.descriptions()[i].type);
    }
}

} /* namespace anonymous */

using namespace Cynara;
using namespace ResponseTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

TEST(ProtocolAdmin, DescriptionListResponse01) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::allow, "allow"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponse02) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::bucket, "bucket"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponse03) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::deny, "deny"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponse04) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::none, "none"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponse05) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::plugin_type, "plugin"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseMultipleDescriptions) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::allow, "allow"),
        PolicyDescription(Types::bucket, "bucket"),
        PolicyDescription(Types::deny, "deny"),
        PolicyDescription(Types::none, "none"),
        PolicyDescription(Types::plugin_type, ""),
        PolicyDescription(Types::plugin_type, "plugin"),
        PolicyDescription(Types::plugin_type, "plugin"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseEmptyDescriptions) {
    std::vector<PolicyDescription> descriptions;

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    testResponse(response, protocol);
}

/* *** compare by serialized data test cases *** */

TEST(ProtocolAdmin, DescriptionListResponseBinary01) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::allow, "allow"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::min);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseBinary02) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::bucket, "bucket"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::min_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseBinary03) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::deny, "deny"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::max);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseBinary04) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::none, "none"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::max_1);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseBinary05) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::plugin_type, "plugin"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::mid);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseBinaryMultipleDescriptions) {
    std::vector<PolicyDescription> descriptions = {
        PolicyDescription(Types::allow, "allow"),
        PolicyDescription(Types::bucket, "bucket"),
        PolicyDescription(Types::deny, "deny"),
        PolicyDescription(Types::none, "none"),
        PolicyDescription(Types::plugin_type, ""),
        PolicyDescription(Types::plugin_type, "plugin"),
        PolicyDescription(Types::plugin_type, "plugin"),
    };

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::max_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}

TEST(ProtocolAdmin, DescriptionListResponseBinaryEmptyDescriptions) {
    std::vector<PolicyDescription> descriptions;

    auto response = std::make_shared<DescriptionListResponse>(descriptions, SN::min_2);
    auto protocol = std::make_shared<ProtocolAdmin>();
    binaryTestResponse(response, protocol);
}
