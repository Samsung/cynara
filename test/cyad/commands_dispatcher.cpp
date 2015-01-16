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
 * @file        test/cyad/commands_dispatcher.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for CommandsDispatcher
 */

#include <ostream>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cynara-admin-types.h>
#include <cynara-error.h>
#include <cynara-policy-types.h>

#include <common/types/PolicyKey.h>
#include <common/types/PolicyResult.h>
#include <cyad/CynaraAdminPolicies.h>
#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandsDispatcher.h>

#include "CyadCommandlineDispatcherTest.h"
#include "FakeAdminApiWrapper.h"
#include "helpers.h"

/**
 * @brief   Dispatcher should not touch admin API on help or error
 * @test    Scenario:
 * - Prepare some parsing results not requiring API calls
 * - Check if no API calls were made
 */
TEST_F(CyadCommandlineDispatcherTest, noApi) {
    using ::testing::_;
    using ::testing::Return;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::CyadCommand result;
    Cynara::HelpCyadCommand helpResult;
    Cynara::ErrorCyadCommand errorResult("Fake error");

    dispatcher.execute(result);
    dispatcher.execute(helpResult);
    dispatcher.execute(errorResult);
}

TEST_F(CyadCommandlineDispatcherTest, deleteBucket) {
    using ::testing::_;
    using ::testing::Return;
    using ::testing::StrEq;
    using ::testing::IsNull;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);
    Cynara::DeleteBucketCyadCommand result("test-bucket");

    EXPECT_CALL(adminApi,
            cynara_admin_set_bucket(_, StrEq("test-bucket"), CYNARA_ADMIN_DELETE, IsNull()))
        .WillOnce(Return(CYNARA_API_SUCCESS));

    dispatcher.execute(result);
}

TEST_F(CyadCommandlineDispatcherTest, setBucket) {
    using ::testing::_;
    using ::testing::Return;
    using ::testing::StrEq;
    using ::testing::IsNull;
    using Cynara::PolicyBucketId;
    using Cynara::PolicyType;
    using Cynara::PolicyResult;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    typedef std::tuple<PolicyBucketId, PolicyResult> BucketData;
    typedef std::vector<BucketData> Buckets;
    const Buckets buckets = { BucketData("test-bucket-1", { CYNARA_ADMIN_ALLOW, "" }),
                              BucketData("test-bucket-2", { CYNARA_ADMIN_DENY, "" }),
                              BucketData("test-bucket-3", { CYNARA_ADMIN_BUCKET, "other-bucket" }),
                              BucketData("test-bucket-2", { CYNARA_ADMIN_NONE, "" }),
                              BucketData("test-bucket-4", { 42, "douglas-noel-adams" }) };

    for (const auto &bucket : buckets) {
        const auto &bucketId = std::get<0>(bucket);
        const auto &policyResult = std::get<1>(bucket);

        SCOPED_TRACE(bucketId);

        Cynara::SetBucketCyadCommand result(bucketId, policyResult);

        if (policyResult.metadata().empty() == false) {
            EXPECT_CALL(adminApi,
                    cynara_admin_set_bucket(_, StrEq(bucketId.c_str()), policyResult.policyType(),
                                            StrEq(policyResult.metadata().c_str())))
                .WillOnce(Return(CYNARA_API_SUCCESS));
        } else {
            EXPECT_CALL(adminApi,
                    cynara_admin_set_bucket(_, StrEq(bucketId.c_str()), policyResult.policyType(),
                                           IsNull()))
                .WillOnce(Return(CYNARA_API_SUCCESS));
        }

        dispatcher.execute(result);
    }
}

TEST_F(CyadCommandlineDispatcherTest, setPolicy) {
    using ::testing::_;
    using ::testing::Return;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);
    Cynara::SetPolicyCyadCommand result("test-bucket", { CYNARA_ADMIN_ALLOW, "" },
                                        { "client", "user", "privilege" });

    Cynara::CynaraAdminPolicies expectedPolicies;
    expectedPolicies.add("test-bucket", { CYNARA_ADMIN_ALLOW, "" },
                         { "client", "user", "privilege"} );
    expectedPolicies.seal();

    EXPECT_CALL(adminApi, cynara_admin_set_policies(_, AdmPolicyListEq(expectedPolicies.data())))
        .WillOnce(Return(CYNARA_API_SUCCESS));

    dispatcher.execute(result);
}

TEST_F(CyadCommandlineDispatcherTest, setPolicyWithMetadata) {
    using ::testing::_;
    using ::testing::Return;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);
    Cynara::SetPolicyCyadCommand result("test-bucket", { CYNARA_ADMIN_ALLOW, "metadata" },
                                        Cynara::PolicyKey("client", "user", "privilege"));

    Cynara::CynaraAdminPolicies expectedPolicies;
    expectedPolicies.add("test-bucket", { CYNARA_ADMIN_ALLOW, "metadata" },
                         { "client", "user", "privilege"} );
    expectedPolicies.seal();

    EXPECT_CALL(adminApi, cynara_admin_set_policies(_, AdmPolicyListEq(expectedPolicies.data())))
        .WillOnce(Return(CYNARA_API_SUCCESS));

    dispatcher.execute(result);
}

TEST_F(CyadCommandlineDispatcherTest, setPoliciesBulk) {
    using ::testing::_;
    using ::testing::Return;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);
    Cynara::SetPolicyBulkCyadCommand result("-");

    // fake stdin ;)
    m_io.file() << "bucket;cli;usr;privilege;0;metadata" << std::endl;
    m_io.file() << "bucket-2;cli;usr;privilege;0xFFFF";

    Cynara::CynaraAdminPolicies expectedPolicies;
    expectedPolicies.add("bucket", { CYNARA_ADMIN_DENY, "metadata" }, {"cli", "usr", "privilege"} );
    expectedPolicies.add("bucket-2", { CYNARA_ADMIN_ALLOW, "" }, {"cli", "usr", "privilege"} );
    expectedPolicies.seal();

    EXPECT_CALL(adminApi, cynara_admin_set_policies(_, AdmPolicyListEq(expectedPolicies.data())))
        .WillOnce(Return(CYNARA_API_SUCCESS));

    dispatcher.execute(result);
}

TEST_F(CyadCommandlineDispatcherTest, setPoliciesBulkInputError) {
    using ::testing::_;
    using ::testing::Return;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);
    Cynara::SetPolicyBulkCyadCommand result("-");

    // fake stdin ;)
    m_io.file() << "invalid input" << std::endl;

    dispatcher.execute(result);

    ASSERT_FALSE(m_io.cerrRaw().str().empty());
}

TEST_F(CyadCommandlineDispatcherTest, erase) {
    using ::testing::_;
    using ::testing::Return;
    using ::testing::StrEq;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::EraseCyadCommand command("", true, { "client", "user", "privilege" });

    EXPECT_CALL(adminApi, cynara_admin_erase(_, StrEq(""), true, StrEq("client"), StrEq("user"),
                                             StrEq("privilege")))
        .WillOnce(Return(CYNARA_API_SUCCESS));

    dispatcher.execute(command);
}

TEST_F(CyadCommandlineDispatcherTest, check) {
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::NotNull;
    using ::testing::Return;
    using ::testing::SetArgPointee;
    using ::testing::StrEq;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::CheckCyadCommand command("", true, { "client", "user", "privilege" });
    int result = 42;

    EXPECT_CALL(adminApi, cynara_admin_check(_, StrEq(""), true, StrEq("client"), StrEq("user"),
                                             StrEq("privilege"), NotNull(), NotNull()))
        .WillOnce(DoAll(SetArgPointee<6>(result), SetArgPointee<7>(nullptr),
                        Return(CYNARA_API_SUCCESS)));

    dispatcher.execute(command);

    ASSERT_EQ("42;\n", m_io.coutRaw().str());
}

TEST_F(CyadCommandlineDispatcherTest, checkWithMetadata) {
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::NotNull;
    using ::testing::Return;
    using ::testing::SetArgPointee;
    using ::testing::StrEq;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::CheckCyadCommand command("", true, { "client", "user", "privilege" });
    int result = 42;
    char *resultExtra = strdup("adams");

    EXPECT_CALL(adminApi, cynara_admin_check(_, StrEq(""), true, StrEq("client"), StrEq("user"),
                                             StrEq("privilege"), NotNull(), NotNull()))
        .WillOnce(DoAll(SetArgPointee<6>(result), SetArgPointee<7>(resultExtra),
                        Return(CYNARA_API_SUCCESS)));

    dispatcher.execute(command);

    ASSERT_EQ("42;adams\n", m_io.coutRaw().str());
}

TEST_F(CyadCommandlineDispatcherTest, checkWithError) {
    using ::testing::_;
    using ::testing::NotNull;
    using ::testing::Return;
    using ::testing::StrEq;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::CheckCyadCommand command("", true, { "client", "user", "privilege" });

    EXPECT_CALL(adminApi, cynara_admin_check(_, StrEq(""), true, StrEq("client"), StrEq("user"),
                                             StrEq("privilege"), NotNull(), NotNull()))
        .WillOnce(Return(CYNARA_API_UNKNOWN_ERROR));

    dispatcher.execute(command);

    ASSERT_TRUE(m_io.coutRaw().str().empty());
}

TEST_F(CyadCommandlineDispatcherTest, listPoliciesNone) {
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::NotNull;
    using ::testing::Return;
    using ::testing::SetArgPointee;
    using ::testing::StrEq;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::ListPoliciesCyadCommand command("", { "client", "user", "privilege" });

    Cynara::CynaraAdminPolicies resultPolicies;
    resultPolicies.seal();
    auto policies = resultPolicies.duplicate();

    EXPECT_CALL(adminApi, cynara_admin_list_policies(_, StrEq(""), StrEq("client"), StrEq("user"),
                                                     StrEq("privilege"), NotNull()))
        .WillOnce(DoAll(SetArgPointee<5>(policies), Return(CYNARA_API_SUCCESS)));

    dispatcher.execute(command);

    ASSERT_EQ("", m_io.coutRaw().str());
}

TEST_F(CyadCommandlineDispatcherTest, listPoliciesTwo) {
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::NotNull;
    using ::testing::Return;
    using ::testing::SetArgPointee;
    using ::testing::StrEq;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::ListPoliciesCyadCommand command("", { "client", "user", "privilege" });

    Cynara::CynaraAdminPolicies resultPolicies;
    resultPolicies.add("bucket", { CYNARA_ADMIN_DENY, "metadata" }, {"cli", "usr", "privilege"} );
    resultPolicies.add("bucket-2", { CYNARA_ADMIN_ALLOW, "" }, {"cli", "usr", "privilege"} );
    resultPolicies.seal();
    auto policies = resultPolicies.duplicate();

    EXPECT_CALL(adminApi, cynara_admin_list_policies(_, StrEq(""), StrEq("client"), StrEq("user"),
                                                     StrEq("privilege"), NotNull()))
        .WillOnce(DoAll(SetArgPointee<5>(policies), Return(CYNARA_API_SUCCESS)));

    dispatcher.execute(command);

    ASSERT_EQ("bucket;cli;usr;privilege;0;metadata\nbucket-2;cli;usr;privilege;65535;\n",
              m_io.coutRaw().str());
}

TEST_F(CyadCommandlineDispatcherTest, listPoliciesDescNone) {
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::NotNull;
    using ::testing::Return;
    using ::testing::SetArgPointee;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::ListPoliciesDescCyadCommand command;

    auto descs = static_cast<cynara_admin_policy_descr **>(
                        calloc(1, sizeof(cynara_admin_policy_descr *)));
    descs[0] = nullptr;

    EXPECT_CALL(adminApi, cynara_admin_list_policies_descriptions(_, NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(descs), Return(CYNARA_API_SUCCESS)));

    dispatcher.execute(command);

    ASSERT_EQ("", m_io.coutRaw().str());
}

TEST_F(CyadCommandlineDispatcherTest, listPoliciesDescOne) {
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::NotNull;
    using ::testing::Return;
    using ::testing::SetArgPointee;

    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(m_io, adminApi);

    Cynara::ListPoliciesDescCyadCommand command;

    auto descs = static_cast<cynara_admin_policy_descr **>(
                        calloc(2, sizeof(cynara_admin_policy_descr *)));

    descs[0] = static_cast<cynara_admin_policy_descr *>(malloc(sizeof(cynara_admin_policy_descr)));
    descs[0]->result = 42;
    descs[0]->name = strdup("adams");
    descs[1] = nullptr;

    EXPECT_CALL(adminApi, cynara_admin_list_policies_descriptions(_, NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(descs), Return(CYNARA_API_SUCCESS)));

    dispatcher.execute(command);

    ASSERT_EQ("42;adams\n", m_io.coutRaw().str());
}
