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
/*
 * @file        load.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::BucketDeserializer
 */

#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <exceptions/BucketRecordCorruptedException.h>
#include <storage/BucketDeserializer.h>
#include <types/Policy.h>

#include "../../helpers.h"

using namespace Cynara;

MATCHER_P(PolicyAtPtrEq, policy, "") {
    return std::tie(policy->key(), policy->result())
        == std::tie(arg->key(), arg->result());
}

class BucketDeserializerFixture : public ::testing::Test {
public:
    virtual ~BucketDeserializerFixture() {};

    PolicyPtr createPolicy(const PolicyKey &pk, const PolicyResult &pr) {
        return std::make_shared<Policy>(pk, pr);
    }

    void checkCorruptedData(const std::string &data, const std::string &corruptedLine,
            size_t corruptedLineNumber) {
        auto bucketStream = std::make_shared<std::istringstream>(data);
        BucketDeserializer deserializer(bucketStream);
        EXPECT_THROW(deserializer.loadPolicies(), BucketRecordCorruptedException);

        bucketStream->seekg(0);
        try {
            deserializer.loadPolicies();
        } catch (const BucketRecordCorruptedException &ex) {
            ASSERT_EQ(corruptedLine, ex.line());
            ASSERT_EQ(corruptedLineNumber, ex.lineNumber());
        }
    }
};

TEST_F(BucketDeserializerFixture, load_empty) {
    using ::testing::IsEmpty;

    auto bucketStream = std::make_shared<std::istringstream>();
    BucketDeserializer deserializer(bucketStream);

    auto policies = deserializer.loadPolicies();
    ASSERT_THAT(policies, IsEmpty());
}

TEST_F(BucketDeserializerFixture, load_1) {
    using ::testing::UnorderedElementsAre;

    auto bucketStream = std::make_shared<std::istringstream>("c;u;p;0;meta");
    BucketDeserializer deserializer(bucketStream);

    auto policies = deserializer.loadPolicies();
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "meta" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyAtPtrEq(expectedPolicy)));
}

TEST_F(BucketDeserializerFixture, load_1_allow) {
    using ::testing::UnorderedElementsAre;

    auto bucketStream = std::make_shared<std::istringstream>("c;u;p;0xFFFF;meta");
    BucketDeserializer deserializer(bucketStream);

    auto policies = deserializer.loadPolicies();
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::ALLOW, "meta" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyAtPtrEq(expectedPolicy)));
}

TEST_F(BucketDeserializerFixture, load_1_no_meta_sep) {
    using ::testing::UnorderedElementsAre;

    auto bucketStream = std::make_shared<std::istringstream>("c;u;p;0;");
    BucketDeserializer deserializer(bucketStream);

    auto policies = deserializer.loadPolicies();
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyAtPtrEq(expectedPolicy)));
}

TEST_F(BucketDeserializerFixture, load_1_no_meta_no_sep) {
    using ::testing::UnorderedElementsAre;

    auto bucketStream = std::make_shared<std::istringstream>("c;u;p;0");
    BucketDeserializer deserializer(bucketStream);

    auto policies = deserializer.loadPolicies();
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyAtPtrEq(expectedPolicy)));
}

TEST_F(BucketDeserializerFixture, load_2) {
    using ::testing::UnorderedElementsAre;

    auto bucketStream = std::make_shared<std::istringstream>("c;u;p;0;meta\n"
                                                             "c;u;p;0;meta\n");
    BucketDeserializer deserializer(bucketStream);

    auto policies = deserializer.loadPolicies();
    auto expectedPolicy = createPolicy({ "c", "u", "p" }, { PredefinedPolicyType::DENY, "meta" });

    ASSERT_THAT(policies, UnorderedElementsAre(PolicyAtPtrEq(expectedPolicy),
            PolicyAtPtrEq(expectedPolicy)));
}

TEST_F(BucketDeserializerFixture, load_mixed) {
    using ::testing::UnorderedElementsAre;
    using ::testing::UnorderedElementsAreArray;

    auto bucketStream = std::make_shared<std::istringstream>("c1;u1;p1;0;meta\n"
                                                             "c2;u2;p2;0xFFFF;meta2\n"
                                                             "c2;u2;p2;0xFFFF;\n"
                                                             "c1;u1;p3;0xFFFE;bucket\n");
    BucketDeserializer deserializer(bucketStream);

    auto policies = deserializer.loadPolicies();
    PolicyCollection expectedPolices = {
        createPolicy({ "c1", "u1", "p1" }, { PredefinedPolicyType::DENY, "meta" }),
        createPolicy({ "c2", "u2", "p2" }, { PredefinedPolicyType::ALLOW, "meta2" }),
        createPolicy({ "c2", "u2", "p2" }, { PredefinedPolicyType::ALLOW, "" }),
        createPolicy({ "c1", "u1", "p3" }, { PredefinedPolicyType::BUCKET, "bucket" })
    };

    // How to do it more elegantly?
    ASSERT_THAT(policies, UnorderedElementsAre(
        PolicyAtPtrEq(expectedPolices.at(0)),
        PolicyAtPtrEq(expectedPolices.at(1)),
        PolicyAtPtrEq(expectedPolices.at(2)),
        PolicyAtPtrEq(expectedPolices.at(3))
    ));
}

TEST_F(BucketDeserializerFixture, load_no_client) {
    auto data = "u;p;0;meta";
    checkCorruptedData(data, data, 1);
}

TEST_F(BucketDeserializerFixture, load_no_type) {
    auto data = "c;u;p;meta";
    checkCorruptedData(data, data, 1);
}

TEST_F(BucketDeserializerFixture, load_all_missing) {
    auto data = ";;;";
    checkCorruptedData(data, data, 1);
}

TEST_F(BucketDeserializerFixture, load_invalid_type) {
    auto data = "c;u;p;X";
    checkCorruptedData(data, data, 1);
}

TEST_F(BucketDeserializerFixture, load_invalid_multiline) {
    auto data = "c1;u1;p1;0;meta\n"
                "c;u;p;X\n"
                "c1;u1;p1;0;meta\n";
    checkCorruptedData(data, "c;u;p;X", 2);
}
