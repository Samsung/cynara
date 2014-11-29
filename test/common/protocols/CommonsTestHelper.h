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
 * @file        test/common/protocols/CommonsTestHelper.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Common helper functions for tests of serialization by protocols
 */

#ifndef TEST_COMMON_PROTOCOLS_COMMONSTESTHELPER_H_
#define TEST_COMMON_PROTOCOLS_COMMONSTESTHELPER_H_

#include <gtest/gtest.h>

namespace {

template <typename R>
void compare(const R &r1, const R &r2) {
    (void) r1;
    (void) r2;
    ASSERT_FALSE("Specialization of compare function must be implemented!");
}

} /* namespace anonymous */

#endif /* TEST_COMMON_PROTOCOLS_COMMONSTESTHELPER_H_ */
