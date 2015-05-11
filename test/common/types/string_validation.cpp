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
 * @file        test/common/types/string_validation.cpp
 * @author      Oskar Świtalski <o.switalski@samsung.com>
 * @version     1.0
 * @brief       Tests for string validation functions
 */

#include <cstring>

#include <gtest/gtest.h>

#include <common/types/string_validation.h>
#include <cynara-limits.h>

TEST(StringValidation, nullptrString) {
    EXPECT_FALSE(isStringValid(nullptr));
    EXPECT_TRUE(isExtraStringValid(nullptr));
}

TEST(StringValidation, emptyString) {
    const char *emptyString = "";

    EXPECT_TRUE(isStringValid(emptyString));
    EXPECT_TRUE(isExtraStringValid(emptyString));
}

TEST(StringValidation, maxString) {
    char maxString[CYNARA_MAX_ID_LENGTH];
    memset(maxString,' ', CYNARA_MAX_ID_LENGTH - 1);

    EXPECT_TRUE(isStringValid(maxString));
    EXPECT_TRUE(isExtraStringValid(maxString));
}

TEST(StringValidation, overMaxString) {
    char maxString[CYNARA_MAX_ID_LENGTH + 1];
    memset(maxString,' ', CYNARA_MAX_ID_LENGTH);

    EXPECT_FALSE(isStringValid(maxString));
    EXPECT_FALSE(isExtraStringValid(maxString));
}
