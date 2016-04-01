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
 * @file        test/common/protocols/monitor/getentriesresponse.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::MonitorGetEntriesResponse usage in
 *              Cynara::ProtocolMonitorGet
 */

#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include <cynara-limits.h>
#include <protocol/ProtocolMonitorGet.h>
#include <response/MonitorGetEntriesResponse.h>
#include <types/Policy.h>
#include <cynara-error.h>

#include "../../helpers.h"
#include <ResponseTestHelper.h>
#include <TestDataCollection.h>

namespace {

template <class T, class M> M get_member_type(M T:: *);

const int64_t TV_SEC_MIN = 0;
const int64_t TV_SEC_1 = 1;
const int64_t TV_SEC_MAX = std::numeric_limits<decltype(get_member_type(&timespec::tv_sec))>::max();
const int64_t TV_SEC_HALF = (TV_SEC_MAX - TV_SEC_MIN) / 2;
const int64_t TV_NSEC_MIN = 0;
const int64_t TV_NSEC_1 = 1;
const int64_t TV_NSEC_MAX =
    std::numeric_limits<decltype(get_member_type(&timespec::tv_nsec))>::max();
const int64_t TV_NSEC_HALF = (TV_SEC_MAX - TV_SEC_MIN) / 2;

template<>
void compare(const Cynara::MonitorGetEntriesResponse &req1,
             const Cynara::MonitorGetEntriesResponse &req2) {
    EXPECT_EQ(req1.entries(), req2.entries());
}

} /* namespace anonymous */

using namespace Cynara;
using namespace ResponseTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of minimum values
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is minimal, nsec is minimal
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse01) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_MIN, TV_NSEC_MIN})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is denied and timespec consist of medium sec and minimal nsec
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is denied
 *          - sec is medium, nsec is minimal
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse02) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_DENIED, {TV_SEC_HALF, TV_NSEC_MIN})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of medium values
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is medium, nsec is medium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse03) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_HALF, TV_NSEC_HALF})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is denied and timespec consist of maxium sec and medium nsec
 * @test    Expected result:
 *          - policy key contains {amanda, to, troll}
 *          - result is denied
 *          - sec is maximum, nsec is medium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse04) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup2, CYNARA_API_ACCESS_DENIED, {TV_SEC_MAX, TV_NSEC_HALF})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::mid);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of maxium values
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is maxium, nsec is maxium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse05) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_MAX, TV_NSEC_MAX})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is denied and timespec consist of maxium sec and minimum nsec
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is denied
 *          - sec is maxium, nsec is minimum
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse06) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_DENIED, {TV_SEC_MAX, TV_NSEC_MIN})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of minimum sec and maximum nsec
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is minimum, nsec is maxium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse07) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_MIN, TV_SEC_MAX})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while
 *          entries vector is empty
 * @test    Expected result:
 *          - entries are empty
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponseEmptyEntries) {
    std::vector<MonitorEntry> entries;

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while
 *          entries vector has maximum allowed elements
 * @test    Expected result:
 *          - entries contain max value of elements
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponseMultipleEntries) {
    std::vector<MonitorEntry> entries;
    entries.reserve(CYNARA_MAX_VECTOR_SIZE);
    for (uint16_t i = 0; i < CYNARA_MAX_VECTOR_SIZE; i++) {
        entries.emplace_back(MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED,
                                          {TV_SEC_MIN, TV_SEC_MAX}));
    }

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testResponse(response, protocol);
}

/* *** compare by serialized data test cases *** */

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of minimum values
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is minimal + 1, nsec is minimal
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse08) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_1, TV_NSEC_MIN})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is denied and timespec consist of medium sec and minimal nsec
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is denied
 *          - sec is medium, nsec is minimal
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse09) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_DENIED, {TV_SEC_HALF, TV_NSEC_MIN})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of medium values
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is medium, nsec is medium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse10) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_HALF, TV_NSEC_HALF})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is denied and timespec consist of maxium sec and medium nsec
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is denied
 *          - sec is maximum, nsec is medium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse11) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_DENIED, {TV_SEC_MAX, TV_NSEC_HALF})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::mid);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of maxium values
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is maxium, nsec is maxium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse12) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_MAX, TV_NSEC_MAX})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max_1);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is denied and timespec consist of maxium sec and minimum + 1 nsec
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is denied
 *          - sec is maxium, nsec is minimum + 1
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse13) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_DENIED, {TV_SEC_MAX, TV_NSEC_1})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is allowed and timespec consist of minimum sec and maximum nsec
 * @test    Expected result:
 *          - policy key contains {c, u, p}
 *          - result is allowed
 *          - sec is minimum, nsec is maxium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse14) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED, {TV_SEC_MIN, TV_SEC_MAX})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while using standard
 *          PolicyKey, result is denied and timespec consist of maxium sec and medium nsec
 * @test    Expected result:
 *          - policy key contains {amanda, to, troll}
 *          - result is denied
 *          - sec is maximum, nsec is medium
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponse15) {
    std::vector<MonitorEntry> entries = {
            MonitorEntry(Keys::k_cup2, CYNARA_API_ACCESS_DENIED, {TV_SEC_MAX, TV_NSEC_HALF})
    };

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::mid);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while
 *          entries vector is empty
 * @test    Expected result:
 *          - entries are empty
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponseEmptyEntries2) {
    std::vector<MonitorEntry> entries;

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::max);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}

/**
 * @brief   Verify if MonitorGetEntriesResponse is properly (de)serialized while
 *          entries vector has maximum allowed elements
 * @test    Expected result:
 *          - entries contain max value of elements
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesResponseMultipleEntries2) {
    std::vector<MonitorEntry> entries;
    entries.reserve(std::numeric_limits<uint16_t>::max());
    for (uint16_t i = 0; i < std::numeric_limits<uint16_t>::max(); i++) {
        entries.emplace_back(MonitorEntry(Keys::k_cup, CYNARA_API_ACCESS_ALLOWED,
                                          {TV_SEC_MIN, TV_SEC_MAX}));
    }

    auto response = std::make_shared<MonitorGetEntriesResponse>(entries, SN::min_2);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestResponse(response, protocol);
}
