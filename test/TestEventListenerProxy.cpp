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
 * @file        TestEventListenerProxy.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Proxy for ::testing::TestEventListener
 */

#include "TestEventListenerProxy.h"

namespace Cynara {

TestEventListenerProxy::TestEventListenerProxy(TestEventListener *originalListener)
    : m_originalListener(originalListener) {};

TestEventListenerProxy::~TestEventListenerProxy() {
    delete m_originalListener;
};

void TestEventListenerProxy::OnTestProgramStart(const UnitTest &unit_test) {
    m_originalListener->OnTestProgramStart(unit_test);
}

void TestEventListenerProxy::OnTestIterationStart(const UnitTest &unit_test, int iteration) {
    m_originalListener->OnTestIterationStart(unit_test, iteration);
}

void TestEventListenerProxy::OnEnvironmentsSetUpStart(const UnitTest &unit_test) {
    m_originalListener->OnEnvironmentsSetUpStart(unit_test);
}

void TestEventListenerProxy::OnEnvironmentsSetUpEnd(const UnitTest &unit_test) {
    m_originalListener->OnEnvironmentsSetUpEnd(unit_test);
}

void TestEventListenerProxy::OnTestCaseStart(const TestCase &test_case) {
    m_originalListener->OnTestCaseStart(test_case);
}

void TestEventListenerProxy::OnTestStart(const TestInfo &test_info) {
    m_originalListener->OnTestStart(test_info);
}

void TestEventListenerProxy::OnTestPartResult(const TestPartResult &result) {
    m_originalListener->OnTestPartResult(result);
}

void TestEventListenerProxy::OnTestEnd(const TestInfo &test_info) {
    m_originalListener->OnTestEnd(test_info);
}

void TestEventListenerProxy::OnTestCaseEnd(const TestCase &test_case) {
    m_originalListener->OnTestCaseEnd(test_case);
}

void TestEventListenerProxy::OnEnvironmentsTearDownStart(const UnitTest &unit_test) {
    m_originalListener->OnEnvironmentsTearDownStart(unit_test);
}

void TestEventListenerProxy::OnEnvironmentsTearDownEnd(const UnitTest &unit_test) {
    m_originalListener->OnEnvironmentsTearDownEnd(unit_test);
}

void TestEventListenerProxy::OnTestIterationEnd(const UnitTest &unit_test, int iteration) {
    m_originalListener->OnTestIterationEnd(unit_test, iteration);
}

void TestEventListenerProxy::OnTestProgramEnd(const UnitTest &unit_test) {
    m_originalListener->OnTestProgramEnd(unit_test);
}

} /* namespace Cynara */
