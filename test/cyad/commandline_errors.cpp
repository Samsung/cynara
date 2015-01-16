/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <cyad/CommandlineParser/CmdlineErrors.h>
#include <cyad/CommandlineParser/CmdlineOpts.h>
#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandlineParser/CyadCommandlineParser.h>

#include "CyadCommandlineTest.h"

namespace Errors = Cynara::CmdlineErrors;
using Cynara::CmdlineOpts::CmdlineOpt;

#define ASSERT_ERROR_MSG(msg,rawResult) { \
    auto result = std::dynamic_pointer_cast<Cynara::ErrorCyadCommand>(rawResult); \
    ASSERT_NE(nullptr, result); \
    ASSERT_TRUE(result->isError()); \
    ASSERT_EQ(msg, result->message()); \
}

TEST_F(CyadCommandlineTest, noOption) {
    prepare_argv({ "./cyad" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::noOption(), parser.parseMain());
}

TEST_F(CyadCommandlineTest, unknownOption) {
    prepare_argv({ "./cyad", "--unknown-option" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::unknownOption(), parser.parseMain());
}

TEST_F(CyadCommandlineTest, deleteBucketNoBucket) {
    prepare_argv({ "./cyad", "--delete-bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::noBucket(), parser.parseMain());
}

TEST_F(CyadCommandlineTest, deleteBucketUnknownOption) {
    prepare_argv({ "./cyad", "--delete-bucket=bucket", "--unknown" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::unknownOption(CmdlineOpt::DeleteBucket), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketNoBucket) {
    prepare_argv({ "./cyad", "--set-bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::noBucket(), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketNoPolicy) {
    prepare_argv({ "./cyad", "--set-bucket=bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Cynara::CmdlineErrors::noType(), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketInvalidPolicy) {
    prepare_argv({ "./cyad", "--set-bucket=bucket", "--type=NaN" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::invalidType(), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketUnknownOption) {
    prepare_argv({ "./cyad", "--set-bucket=bucket", "--unknown", "--type=42" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::unknownOption(CmdlineOpt::SetBucket), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setBucketMetadataNoPolicy) {
    prepare_argv({ "./cyad", "--set-bucket=bucket", "--metadata=some-meta" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::noType(), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setPolicyNoOption) {
    prepare_argv({ "./cyad", "--set-policy" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::optionMissing(CmdlineOpt::SetPolicy), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setPolicyUnknownOption) {
    prepare_argv({ "./cyad", "--set-policy", "--unknown-option" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::unknownOption(CmdlineOpt::SetPolicy), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setPolicyArgumentMissing) {
    prepare_argv({ "./cyad", "--set-policy", "--bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::argumentMissing(CmdlineOpt::SetPolicy), parser.parseMain());
}

TEST_F(CyadCommandlineTest, setPolicyNoPolicy) {
    // TODO: In the future, we can identify actual options being missed
    prepare_argv({ "./cyad", "--set-policy", "--bucket=some-bucket",
                   "--client=client", "--user=user", "--privilege=privilege" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::optionMissing(CmdlineOpt::SetPolicy), parser.parseMain());
}

TEST_F(CyadCommandlineTest, eraseNoRecursive) {
    prepare_argv({ "./cyad", "--erase=",
                   "--client=client", "--user=user", "--privilege=privilege" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::optionMissing(CmdlineOpt::Erase), parser.parseMain());
}

// Error should be argument missing, but getopt acts weird
TEST_F(CyadCommandlineTest, listPoliciesNoBucket) {
    prepare_argv({ "./cyad", "--list-policies",
                   "--client=client", "--user=user", "--privilege=privilege" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());
    ASSERT_ERROR_MSG(Errors::optionMissing(CmdlineOpt::ListPolicies), parser.parseMain());
}
