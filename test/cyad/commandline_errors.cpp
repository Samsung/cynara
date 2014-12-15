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
 * @file        test/cyad/commandline.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for CyadCommandlineParser (errors)
 */

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandlineParser/CyadCommandlineParser.h>

#include "CyadCommandlineTest.h"

#define ASSERT_ERROR_MSG(msg,rawResult) { \
    auto result = std::dynamic_pointer_cast<Cynara::ErrorCyadCommand>(rawResult); \
    ASSERT_NE(nullptr, result); \
    ASSERT_TRUE(result->isError()); \
    ASSERT_EQ(msg, result->message()); \
}

TEST_F(CyadCommandlineTest, noOption) {
    prepare_argv({ "./cyad" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::NO_OPTION, parser.parseMain());
}

TEST_F(CyadCommandlineTest, unknownOption) {
    prepare_argv({ "./cyad", "--unknown-option" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::UNKNOWN_OPTION, parser.parseMain());
}

TEST_F(CyadCommandlineTest, deleteBucketNoBucket) {
    prepare_argv({ "./cyad", "--delete-bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::NO_BUCKET, parser.parseMain());
}

TEST_F(CyadCommandlineTest, deleteBucketUnknownOption) {
    prepare_argv({ "./cyad", "--delete-bucket=bucket", "--unknown" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::UNKNOWN_OPTION_DELETE_BUCKET, parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketNoBucket) {
    prepare_argv({ "./cyad", "--set-bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::NO_BUCKET, parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketNoPolicy) {
    prepare_argv({ "./cyad", "--set-bucket=bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::NO_POLICY, parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketInvalidPolicy) {
    prepare_argv({ "./cyad", "--set-bucket=bucket", "--policy=NaN" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::INVALID_POLICY, parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketUnknownOption) {
    prepare_argv({ "./cyad", "--set-bucket=bucket", "--unknown", "--policy=42" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::UNKNOWN_OPTION_SET_BUCKET, parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketMetadataNoPolicy) {
    prepare_argv({ "./cyad", "--set-bucket=bucket", "--metadata=some-meta" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CyadCmdlineErrors::NO_POLICY, parser.parseMain());
}
