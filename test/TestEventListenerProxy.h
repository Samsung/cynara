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
 * @file        test/TestEventListenerProxy.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Proxy for ::testing::TestEventListener
 */

#ifndef TEST_TESTEVENTLISTENERPROXY_H_
#define TEST_TESTEVENTLISTENERPROXY_H_

#include <gtest/gtest.h>

namespace Cynara {

using namespace testing;

class TestEventListenerProxy : public TestEventListener
{
public:
    explicit TestEventListenerProxy(TestEventListener *originalListener);
    virtual ~TestEventListenerProxy();

    virtual void OnTestProgramStart(const UnitTest &unit_test);
    virtual void OnTestIterationStart(const UnitTest &unit_test, int iteration);
    virtual void OnEnvironmentsSetUpStart(const UnitTest &unit_test);
    virtual void OnEnvironmentsSetUpEnd(const UnitTest &unit_test);
    virtual void OnTestCaseStart(const TestCase &test_case);
    virtual void OnTestStart(const TestInfo &test_info);
    virtual void OnTestPartResult(const TestPartResult &result);
    virtual void OnTestEnd(const TestInfo &test_info);
    virtual void OnTestCaseEnd(const TestCase &test_case);
    virtual void OnEnvironmentsTearDownStart(const UnitTest &unit_test);
    virtual void OnEnvironmentsTearDownEnd(const UnitTest &unit_test);
    virtual void OnTestIterationEnd(const UnitTest &unit_test, int iteration);
    virtual void OnTestProgramEnd(const UnitTest &unit_test);

protected:
    TestEventListener *originalListener() {
        return m_originalListener;
    }

private:
    TestEventListener *m_originalListener;
};

} /* namespace Cynara */

#endif /* TEST_TESTEVENTLISTENERPROXY_H_ */
