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
 * @file        test/cyad/CyadCommandlineDispatcherTest.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Test fixture for CyadCommandlineDispatcher
 */

#ifndef TEST_CYAD_CYADCOMMANDLINEDISPATCHERTEST_H_
#define TEST_CYAD_CYADCOMMANDLINEDISPATCHERTEST_H_

#include <new>
#include <stdexcept>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cynara-admin-types.h>
#include <cynara-error.h>

#include "FakeAdminApiWrapper.h"
#include "FakeDispatcherIO.h"
#include "FakeErrorApiWrapper.h"

#define MAX_POLICY_DESCRIPTIONS 15

class CyadCommandlineDispatcherTest : public ::testing::Test {
private:
    typedef cynara_admin_policy_descr DescrType;

public:
    void SetUp(void) {
        auto arrSize = sizeof(DescrType *) * (MAX_POLICY_DESCRIPTIONS + 1);
        m_policyDescs = static_cast<DescrType **>(malloc(arrSize));

        if (m_policyDescs == nullptr)
            throw std::bad_alloc();

        m_policyDescs[0] = nullptr;
        setupInitFinishExpectations();
        setupDescriptionsExpectation();
    }

    void TearDown(void) {
        if (m_policyDescs != nullptr)
            freeDescriptions();
    }

    void addDescriptions(std::vector<std::pair<int, std::string>> toAdd) {
        if (m_descCount + toAdd.size() > MAX_POLICY_DESCRIPTIONS) {
            throw std::length_error("Descriptions count would exceed "
                                    + std::to_string(MAX_POLICY_DESCRIPTIONS));
        }

        auto addDesc = [] (DescrType **desc, int result, const std::string &name) {
            (*desc) = static_cast<DescrType *>(malloc(sizeof(DescrType)));
            (*desc)->result = result;
            (*desc)->name = strdup(name.data());
        };

        for (const auto &it : toAdd) {
            addDesc(m_policyDescs + m_descCount, it.first, it.second);
            ++m_descCount;
        }

        m_policyDescs[m_descCount] = nullptr;
    }

protected:
    void setupInitFinishExpectations() {
        using ::testing::_;
        using ::testing::Return;

        EXPECT_CALL(m_adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
        EXPECT_CALL(m_adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    }

    void setupDescriptionsExpectation() {
        using ::testing::_;
        using ::testing::Return;
        using ::testing::NotNull;
        using ::testing::DoAll;
        using ::testing::SetArgPointee;
        using ::testing::Assign;

        addDescriptions({
            { 0, "DENY" },
            { 0xFFFF, "ALLOW" },
        });

        EXPECT_CALL(m_adminApi, cynara_admin_list_policies_descriptions(_, NotNull()))
            .WillOnce(DoAll(SetArgPointee<1>(m_policyDescs),
                            Assign(&m_policyDescs, nullptr),
                            Return(CYNARA_API_SUCCESS)));
    }

    FakeDispatcherIO m_io;
    FakeAdminApiWrapper m_adminApi;
    FakeErrorApiWrapper m_errorApi;

private:
    void freeDescriptions() {
        auto freePolicyDesc = [] (DescrType *pd) {
           free(pd->name);
           free(pd);
        };

        for (int i = 0; m_policyDescs[i] != nullptr; ++i) {
           freePolicyDesc(m_policyDescs[i]);
        }
        free(m_policyDescs);
    }

    DescrType **m_policyDescs = nullptr;
    std::size_t m_descCount = 0;
};

#endif /* TEST_CYAD_CYADCOMMANDLINEDISPATCHERTEST_H_ */
