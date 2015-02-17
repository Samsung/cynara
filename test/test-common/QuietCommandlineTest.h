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
 * @file        test/test-common/QuietCommandlineTest.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Fixture for tests of commandline parsers with no output to std::cout nor std::cerr
 */

#ifndef TEST_TEST_COMMON_QUIETCOMMANDLINETEST_H_
#define TEST_TEST_COMMON_QUIETCOMMANDLINETEST_H_

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "BaseCommandlineTest.h"

class QuietCommandlineTest : public BaseCommandlineTest {
public:
    void clearOutput(void) {
        m_cerr.str(std::string());
        m_cout.str(std::string());
    }

    void getOutput(std::string &out, std::string &err) const {
        err = m_cerr.str();
        out = m_cout.str();
    }

protected:
    virtual void SetUp(void) {
        m_cerrBackup.reset(std::cerr.rdbuf());
        std::cerr.rdbuf(m_cerr.rdbuf());

        m_coutBackup.reset(std::cout.rdbuf());
        std::cout.rdbuf(m_cout.rdbuf());
    }

    virtual void TearDown(void) {
        destroy_argv();
        std::cerr.rdbuf(m_cerrBackup.release());
        std::cout.rdbuf(m_coutBackup.release());
    }

    std::unique_ptr<std::streambuf> m_cerrBackup;
    std::unique_ptr<std::streambuf> m_coutBackup;
    std::stringstream m_cerr;
    std::stringstream m_cout;
};

#endif /* TEST_TEST_COMMONS_QUIETCOMMANDLINETEST_H_ */
