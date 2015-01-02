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
 * @brief       Tests for CyadCommandlineParser
 */

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/types/PolicyKey.h>
#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandlineParser/CyadCommandlineParser.h>
#include <cyad/CommandlineParser/HumanReadableParser.h>

#include "CyadCommandlineTest.h"

TEST_F(CyadCommandlineTest, help) {
    prepare_argv({ "./cyad", "--help" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::HelpCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
}

TEST_F(CyadCommandlineTest, deleteBucket) {
    prepare_argv({ "./cyad", "--delete-bucket=bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::DeleteBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
}

TEST_F(CyadCommandlineTest, setBucket) {
    prepare_argv({ "./cyad", "--set-bucket=bucket", "--type=42" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
    ASSERT_EQ(42, result->policyResult().policyType());
    ASSERT_TRUE(result->policyResult().metadata().empty());
}

TEST_F(CyadCommandlineTest, setBucketWithMetadata) {
    const std::string ultimateAnswer = "Answer to The Ultimate Question of Life,"
                                       " the Universe, and Everything";

    prepare_argv({ "./cyad", "--set-bucket=adams", "--type=42", "--metadata=" + ultimateAnswer });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("adams", result->bucketId());
    ASSERT_EQ(42, result->policyResult().policyType());
    ASSERT_EQ(ultimateAnswer, result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, setPolicy) {
    prepare_argv({ "./cyad", "--set-policy", "--bucket=some-bucket",
                   "--client=client", "--user=user", "--privilege=privilege",
                   "--type=42" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("some-bucket", result->bucketId());

    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ(42, result->policyResult().policyType());
    ASSERT_TRUE(result->policyResult().metadata().empty());
}

TEST_F(CyadCommandlineTest, setPolicyWithMetadata) {
    prepare_argv({ "./cyad", "--set-policy", "--bucket=some-bucket",
                   "--client=client", "--user=user", "--privilege=privilege",
                   "--type=42", "--metadata=some-metadata" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("some-bucket", result->bucketId());

    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ(42, result->policyResult().policyType());
    ASSERT_EQ("some-metadata", result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, setPolicyInDefaultBucket) {
    prepare_argv({ "./cyad", "--set-policy", "--bucket=",
                   "--client=client", "--user=user", "--privilege=privilege",
                   "--type=ALLOW", "--metadata=some-metadata" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("", result->bucketId());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ(CYNARA_ADMIN_ALLOW, result->policyResult().policyType());
    ASSERT_EQ("some-metadata", result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, setPolicyInDefaultBucketNoOption) {
    prepare_argv({ "./cyad", "--set-policy",
                   "--client=client", "--user=user", "--privilege=privilege",
                   "--type=ALLOW", "--metadata=some-metadata" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("", result->bucketId());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ(CYNARA_ADMIN_ALLOW, result->policyResult().policyType());
    ASSERT_EQ("some-metadata", result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, parsePolicyTypeBase10) {
    auto parsePolicyType = [] (const char *rawPolicy) -> Cynara::PolicyType {
        return Cynara::HumanReadableParser::policyType(rawPolicy);
    };

    ASSERT_EQ(CYNARA_ADMIN_DENY,   parsePolicyType("0"));
    ASSERT_EQ(CYNARA_ADMIN_NONE,   parsePolicyType("1"));
    ASSERT_EQ(CYNARA_ADMIN_BUCKET, parsePolicyType("65534"));
    ASSERT_EQ(CYNARA_ADMIN_ALLOW,  parsePolicyType("65535"));
}

TEST_F(CyadCommandlineTest, parsePolicyTypeBase16) {
    auto parsePolicyType = [] (const char *rawPolicy) -> Cynara::PolicyType {
        return Cynara::HumanReadableParser::policyType(rawPolicy);
    };

    ASSERT_EQ(CYNARA_ADMIN_DENY,   parsePolicyType("0x0"));
    ASSERT_EQ(CYNARA_ADMIN_NONE,   parsePolicyType("0x1"));
    ASSERT_EQ(CYNARA_ADMIN_BUCKET, parsePolicyType("0xFFFE"));
    ASSERT_EQ(CYNARA_ADMIN_ALLOW,  parsePolicyType("0xFFFF"));
}

TEST_F(CyadCommandlineTest, parsePolicyTypeHuman) {
    auto parsePolicyType = [] (const char *rawPolicy) -> Cynara::PolicyType {
        return Cynara::HumanReadableParser::policyType(rawPolicy);
    };

    ASSERT_EQ(CYNARA_ADMIN_DENY,   parsePolicyType("DENY"));
    ASSERT_EQ(CYNARA_ADMIN_NONE,   parsePolicyType("NONE"));
    ASSERT_EQ(CYNARA_ADMIN_BUCKET, parsePolicyType("BUCKET"));
    ASSERT_EQ(CYNARA_ADMIN_ALLOW,  parsePolicyType("ALLOW"));
}

TEST_F(CyadCommandlineTest, setPoliciesBulkFilename) {
    prepare_argv({ "./cyad", "--set-policy", "--bulk=/tmp/input_file" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyBulkCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("/tmp/input_file", result->filename());
}

TEST_F(CyadCommandlineTest, setPoliciesBulkStdin) {
    prepare_argv({ "./cyad", "--set-policy", "--bulk=-" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyBulkCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("-", result->filename());
}
