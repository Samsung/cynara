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
 * @file        test/cyad/commandline_short.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for CyadCommandlineParser (short opts)
 */

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cynara-admin-types.h>

#include <common/types/PolicyKey.h>
#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandlineParser/CyadCommandlineParser.h>
#include <cyad/CommandlineParser/HumanReadableParser.h>

#include "CyadCommandlineTest.h"

TEST_F(CyadCommandlineTest, help_short) {
    prepare_argv({ "./cyad", "-h" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::HelpCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
}

TEST_F(CyadCommandlineTest, deleteBucket_short) {
    prepare_argv({ "./cyad", "-d", "bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::DeleteBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
}

TEST_F(CyadCommandlineTest, setBucket_short) {
    prepare_argv({ "./cyad", "-b", "bucket", "-t", "42" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
    ASSERT_EQ("42", result->policyResult().policyType());
    ASSERT_TRUE(result->policyResult().metadata().empty());
}

TEST_F(CyadCommandlineTest, setBucketWithMetadata_short) {
    const std::string ultimateAnswer = "Answer to The Ultimate Question of Life,"
                                       " the Universe, and Everything";

    prepare_argv({ "./cyad", "-b", "adams", "-t", "42", "-m", ultimateAnswer });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("adams", result->bucketId());
    ASSERT_EQ("42", result->policyResult().policyType());
    ASSERT_EQ(ultimateAnswer, result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, setPolicy_short) {
    prepare_argv({ "./cyad", "-s", "-k", "some-bucket",
                   "-c", "client", "-u", "user", "-p", "privilege",
                   "-t", "42" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("some-bucket", result->bucketId());

    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ("42", result->policyResult().policyType());
    ASSERT_TRUE(result->policyResult().metadata().empty());
}

TEST_F(CyadCommandlineTest, setPolicyWithMetadata_short) {
    prepare_argv({ "./cyad", "-s", "-k", "some-bucket",
                   "-c", "client", "-u", "user", "-p", "privilege",
                   "-t", "42", "-m", "some-metadata" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("some-bucket", result->bucketId());

    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ("42", result->policyResult().policyType());
    ASSERT_EQ("some-metadata", result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, setPolicyInDefaultBucket_short) {
    prepare_argv({ "./cyad", "-s", "-k", "",
                   "-c", "client", "-u", "user", "-p", "privilege",
                   "-t", "ALLOW", "-m", "some-metadata" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("", result->bucketId());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ("ALLOW", result->policyResult().policyType());
    ASSERT_EQ("some-metadata", result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, setPolicyInDefaultBucketNoOption_short) {
    prepare_argv({ "./cyad", "-s",
                   "-c", "client", "-u", "user", "-p", "privilege",
                   "-t", "ALLOW", "-m", "some-metadata" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("", result->bucketId());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
    ASSERT_EQ("ALLOW", result->policyResult().policyType());
    ASSERT_EQ("some-metadata", result->policyResult().metadata());
}

TEST_F(CyadCommandlineTest, parsePolicyTypeBase10_short) {
    auto parsePolicyType = [] (const char *rawPolicy) -> Cynara::PolicyType {
        return Cynara::HumanReadableParser::policyType(rawPolicy);
    };

    ASSERT_EQ(CYNARA_ADMIN_DENY,   parsePolicyType("0"));
    ASSERT_EQ(CYNARA_ADMIN_NONE,   parsePolicyType("1"));
    ASSERT_EQ(CYNARA_ADMIN_BUCKET, parsePolicyType("65534"));
    ASSERT_EQ(CYNARA_ADMIN_ALLOW,  parsePolicyType("65535"));
}

TEST_F(CyadCommandlineTest, parsePolicyTypeBase16_short) {
    auto parsePolicyType = [] (const char *rawPolicy) -> Cynara::PolicyType {
        return Cynara::HumanReadableParser::policyType(rawPolicy);
    };

    ASSERT_EQ(CYNARA_ADMIN_DENY,   parsePolicyType("0x0"));
    ASSERT_EQ(CYNARA_ADMIN_NONE,   parsePolicyType("0x1"));
    ASSERT_EQ(CYNARA_ADMIN_BUCKET, parsePolicyType("0xFFFE"));
    ASSERT_EQ(CYNARA_ADMIN_ALLOW,  parsePolicyType("0xFFFF"));
}

TEST_F(CyadCommandlineTest, parsePolicyTypeHuman_short) {
    auto parsePolicyType = [] (const char *rawPolicy) -> Cynara::PolicyType {
        return Cynara::HumanReadableParser::policyType(rawPolicy);
    };

    ASSERT_EQ(CYNARA_ADMIN_DENY,   parsePolicyType("DENY"));
    ASSERT_EQ(CYNARA_ADMIN_NONE,   parsePolicyType("NONE"));
    ASSERT_EQ(CYNARA_ADMIN_BUCKET, parsePolicyType("BUCKET"));
    ASSERT_EQ(CYNARA_ADMIN_ALLOW,  parsePolicyType("ALLOW"));
}

TEST_F(CyadCommandlineTest, setPoliciesBulkFilename_short) {
    prepare_argv({ "./cyad", "-s", "-f", "/tmp/input_file" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyBulkCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("/tmp/input_file", result->filename());
}

TEST_F(CyadCommandlineTest, setPoliciesBulkStdin_short) {
    prepare_argv({ "./cyad", "-s", "-f", "-" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::SetPolicyBulkCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("-", result->filename());
}

TEST_F(CyadCommandlineTest, eraseRecursive_short) {
    prepare_argv({ "./cyad", "-e", "bucket", "-r", "yes",
                   "-c", "client", "-u", "user", "-p", "privilege" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::EraseCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
    ASSERT_TRUE(result->recursive());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
}

TEST_F(CyadCommandlineTest, eraseNonrecursive_short) {
    prepare_argv({ "./cyad", "-e", "bucket", "-r", "no",
                   "-c", "client", "-u", "user", "-p", "privilege" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::EraseCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
    ASSERT_FALSE(result->recursive());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
}

TEST_F(CyadCommandlineTest, checkDefaultRecursive_short) {
    prepare_argv({ "./cyad", "-a", "-r", "no",
                   "-c", "client", "-u", "user", "-p", "privilege" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::CheckCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ(CYNARA_ADMIN_DEFAULT_BUCKET, result->bucketId());
    ASSERT_FALSE(result->recursive());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
}

TEST_F(CyadCommandlineTest, listPoliciesDefault_short) {
    prepare_argv({ "./cyad", "-l", "",
                   "-c", "client", "-u", "user", "-p", "privilege" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::ListPoliciesCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ(CYNARA_ADMIN_DEFAULT_BUCKET, result->bucketId());
    ASSERT_EQ(Cynara::PolicyKey("client", "user", "privilege"), result->policyKey());
}

TEST_F(CyadCommandlineTest, listPoliciesOtherBucket_short) {
    prepare_argv({ "./cyad", "-l", "some-bucket",
                   "-c", "c", "-u", "u", "-p", "p" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::ListPoliciesCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("some-bucket", result->bucketId());
    ASSERT_EQ(Cynara::PolicyKey("c", "u", "p"), result->policyKey());
}

TEST_F(CyadCommandlineTest, listPoliciesDesc_short) {
    prepare_argv({ "./cyad", "-g" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::ListPoliciesDescCyadCommand>(
                                                                                parser.parseMain());
    ASSERT_NE(nullptr, result);
}
