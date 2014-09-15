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
 * @file        test/tests.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Unit-tests setup
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestEventListenerProxy.h"

class PropertyAwarePrettyUnitTestResultPrinter : public Cynara::TestEventListenerProxy {
    using Cynara::TestEventListenerProxy::TestEventListenerProxy;

    void OnTestEnd(const ::testing::TestInfo &testInfo) {
        auto result = testInfo.result();
        for (int i = 0; i < result->test_property_count(); ++i) {
            const auto &property = result->GetTestProperty(i);
            std::cout << "             - "
                      << property.key() << " = " << property.value()
                      << std::endl;
        }
        originalListener()->OnTestEnd(testInfo);
    }
};

int main(int argc, char** argv) {
    // Disables elapsed time by default.
    ::testing::GTEST_FLAG(print_time) = false;

    // This allows the user to override the flag on the command line.
    ::testing::InitGoogleTest(&argc, argv);

    // Gets hold of the event listener list.
    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();

    // Remove original listener (PrettyUnitTestResultPrinter)
    // and wrap it with our proxy (PropertyAwarePrettyUnitTestResultPrinter)
    auto originalListener = listeners.Release(listeners.default_result_printer());
    listeners.Append(new PropertyAwarePrettyUnitTestResultPrinter(originalListener));

    return RUN_ALL_TESTS();
}
