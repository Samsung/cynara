/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/client-async/sequence/sequencecontainer.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::SequenceContainer
 */


#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <client-async/sequence/SequenceContainer.h>

using namespace Cynara;

/**
 * @brief   Verify if getting and releasing numbers from SequenceContainer works.
 *          There are 100 sequence number got and all of them are released in the same order,
 *          they were acquired.
 * @test    Expected result:
 * - getting all sequence number succeeds (returns true)
 * - every got number is the lowest available non-negative integer number
 *   (so sequently 0, 1, 2, 3, ... should be received)
 * - all numbers are successfully (true returned) released
 */
TEST(SequenceContainer, get_linear) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.release(seq[i]);
        SCOPED_TRACE(std::string("release : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
    }
}

/**
 * @brief   Verify if getting and releasing numbers from SequenceContainer works
 *          in case when numbers are reused. There are 100 iterations (j) of zig-zag.
 *          Every iteration gets 100 sequential numbers (i) and releases them in reverse order.
 * @test    Expected result:
 * - getting all sequence number succeeds (returns true)
 * - every got number is the lowest available non-negative integer number
 *   (so sequently 0, 1, 2, 3, ... should be received in every zig-zag iteration)
 * - all numbers are successfully (true returned) released in all zig-zag iterations
 */
TEST(SequenceContainer, get_zig_zag) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    constexpr size_t zigzags = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t j = 0; j < zigzags; ++j) {
        for (size_t i = 0; i < seqMax; ++i) {
            bool ret = seqBox.get(seq[i]);
            SCOPED_TRACE(std::string("get : ") + std::to_string(i) + std::string(", zigzag : ")
                + std::to_string(j));
            ASSERT_EQ(true, ret);
            ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
        }

        for (size_t i = seqMax - 1; i != static_cast<size_t>(-1); --i) {
            bool ret = seqBox.release(seq[i]);
            SCOPED_TRACE(std::string("release : ") + std::to_string(i) + std::string(", zigzag : ")
                + std::to_string(j));
            ASSERT_EQ(true, ret);
        }
    }
}

/**
 * @brief   Verify if getting and releasing numbers from SequenceContainer works
 *          in case when numbers are got and released insequently.
 *          There are 100 numbers got at the start of the test case,
 *          then all odd numbers are released and reacquired.
 * @test    Expected result:
 * - getting all sequence number succeeds (returns true)
 * - every got number is the lowest available non-negative integer number
 *   (so sequently 0, 1, 2, 3, ... should be received during iteration on first loop
 *   and 1, 3, 5, ... should be received in 3rd loop)
 * - all odd numbers are successfully (true returned) released in 2nd loop
 */
TEST(SequenceContainer, get_odd) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    for (size_t i = 1; i < seqMax; i += 2) {
        bool ret = seqBox.release(seq[i]);
        SCOPED_TRACE(std::string("release : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
    }

    for (size_t i = 1; i < seqMax; i += 2) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get odd : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }
}

/**
 * @brief   Verify if clearing SequenceContainer works.
 *          There are 100 sequence number got from SequenceContainer. Then a clear() is called.
 *          After clear() 100 numbers are reacquired and released
 * @test    Expected result:
 * - getting all sequence number succeeds (returns true) both before anf after calling clear()
 * - every got number is the lowest available non-negative integer number
 *   (so sequently 0, 1, 2, 3, ... should be received both before and after clear())
 * - all numbers are successfully (true returned) released after they were reacquired.
 */
TEST(SequenceContainer, clear) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    seqBox.clear();

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.release(seq[i]);
        SCOPED_TRACE(std::string("release : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
    }
}
