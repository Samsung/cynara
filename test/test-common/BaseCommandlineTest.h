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
 * @file        test/test-common/BaseCommandlineTest.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Base fixture for tests of commandline parsers
 */

#ifndef TEST_TEST_COMMON_BASECOMMANDLINETEST_H_
#define TEST_TEST_COMMON_BASECOMMANDLINETEST_H_

#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class BaseCommandlineTest : public ::testing::Test {
public:
    typedef std::vector<std::string> Args;

    void prepare_argv(const Args &args) {
        destroy_argv();

        m_argc = args.size();
        m_argv = new char *[m_argc];

        for (auto i = 0; i < m_argc; ++i) {
            m_argv[i] = strdup(args.at(i).c_str());
            if (m_argv[i] == nullptr)
                throw std::bad_alloc();
        }
    }

    int argc(void) const {
        return m_argc;
    }

    char * const *argv(void) const {
        return m_argv;
    }

protected:
    virtual void TearDown(void) {
        destroy_argv();
    }

    void destroy_argv(void) {
        for (auto i = 0; i < m_argc; ++i) {
            free(m_argv[i]);
        }
        delete[] m_argv;

        m_argc = 0;
        m_argv = nullptr;
    }

    int m_argc = 0;
    char **m_argv = nullptr;
};

#endif /* TEST_TEST_COMMON_BASECOMMANDLINETEST_H_ */
