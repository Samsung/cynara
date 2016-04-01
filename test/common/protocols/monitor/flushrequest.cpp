/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/common/protocols/monitor/flushrequest.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::MonitorGetFlushRequest usage in
 *              Cynara::ProtocolMonitorGet
 */

#include <vector>

#include <gtest/gtest.h>

#include <protocol/ProtocolMonitorGet.h>
#include <request/MonitorGetFlushRequest.h>
#include <types/Policy.h>

#include <RequestTestHelper.h>
#include <TestDataCollection.h>

namespace {

template<>
void compare(const Cynara::MonitorGetFlushRequest &req1,
             const Cynara::MonitorGetFlushRequest &req2) {
    EXPECT_EQ(req1.sequenceNumber(), req2.sequenceNumber());
}

} /* namespace anonymous */

using namespace Cynara;
using namespace RequestTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with minimal sequence number
 * @test    Expected result:
 * - sequence number is 0
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest01) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with minimal + 1 sequence
            number
 * @test    Expected result:
 * - sequence number is minimal + 1
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest02) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::min_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with minimal + 2 sequence
            number
 * @test    Expected result:
 * - sequence number is min + 2
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest03) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::min_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with medium sequence number
 * @test    Expected result:
 * - sequence number is medium
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest04) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::mid);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with max - 1 sequence
            number
 * @test    Expected result:
 * - sequence number is max - 1
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest05) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::max_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with max - 2 sequence
            number
 * @test    Expected result:
 * - sequence number is max - 2
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest06) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::max_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with maximum sequence
            number
 * @test    Expected result:
 * - sequence number is maximum
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest07) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::max);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/* *** compare by serialized data test cases *** */

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with minimal sequence
            number
 * @test    Expected result:
 * - sequence number is minimal
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest08) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with minimal + 1 sequence
            number
 * @test    Expected result:
 * - sequence number is minimal + 1
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest09) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::min_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with minimal + 2 sequence
            number
 * @test    Expected result:
 * - sequence number is minimal + 2
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest10) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::min_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with medium sequence number
 * @test    Expected result:
 * - sequence number is medium
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest11) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::mid);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with maximum - 1 sequence
            number
 * @test    Expected result:
 * - sequence number is maximum -1
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest12) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::max_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with maximum - 2 sequence
            number
 * @test    Expected result:
 * - sequence number is maximum - 2
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest13) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::max_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if MonitorGetFlushRequest is properly (de)serialized with maximum sequence
            number
 * @test    Expected result:
 * - sequence number is max
 */
TEST(ProtocolMonitorGet, MonitorGetFlushRequest14) {
    auto request = std::make_shared<MonitorGetFlushRequest>(SN::max);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}
