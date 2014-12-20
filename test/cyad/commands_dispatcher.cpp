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
 * @file        test/cyad/commands_dispatcher.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for CommandsDispatcher
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cynara-error.h>

#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandsDispatcher.h>

#include "CyadCommandlineDispatcherTest.h"
#include "FakeAdminApiWrapper.h"

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
