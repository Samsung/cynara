/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        test/service/main/cmdlineparser.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Tests of CmdlineParser
 */

#include <string>

#include <service/main/CmdlineParser.h>

#include "CynaraCommandlineTest.h"

namespace {

const std::string execName("./cynara");
const std::string helpMessage("Usage: " + execName + " [OPTIONS]\n\n"
        "  -V, --version                  print version of ./cynara and exit\n"
        "  -h, --help                     print this help message and exit\n");

} // namespace

namespace Parser = Cynara::CmdlineParser;

/**
 * @brief   Verify if passing "help" option to commandline handler returns help message
 * @test    Expected result:
 * - call handler indicates success
 * - help message in output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, help) {
    std::string err;
    std::string out;

    for (const auto &opt : { "-h", "--help" }) {
        clearOutput();
        prepare_argv({ execName, opt });

        SCOPED_TRACE(opt);
        const auto handlingSuccess = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(handlingSuccess);
        ASSERT_EQ(helpMessage, out);
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing "version" option to commandline handler returns version message
 * @test    Expected result:
 * - call handler indicates success
 * - version message in output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, version) {
    std::string err;
    std::string out;

    for (const auto &opt : { "-V", "--version" }) {
        clearOutput();
        prepare_argv({ execName, opt });

        SCOPED_TRACE(opt);
        const auto handlingSuccess = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(handlingSuccess);
        ASSERT_EQ(std::string(CYNARA_VERSION) + "\n", out);
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing unknown option to commandline handler returns error message
 * @test    Expected result:
 * - call handler indicates failure
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, unknownOption) {
    std::string err;
    std::string out;

    for (const auto &badOpt : { "-b", "--badOption" }) {
        clearOutput();
        prepare_argv({ execName, badOpt });

        SCOPED_TRACE(badOpt);
        const auto handlingSuccess = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(handlingSuccess);
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ("Unknown option\n", err);
    }
}

/**
 * @brief   Verify if passing no options to commandline handler returns error message
 * @test    Expected result:
 * - call handler indicates failure
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, noOption) {
    std::string err;
    std::string out;

    clearOutput();
    prepare_argv({ execName });

    const auto handlingSuccess = Parser::handleCmdlineOptions(this->argc(), this->argv());
    getOutput(out, err);

    ASSERT_FALSE(handlingSuccess);
    ASSERT_EQ(helpMessage, out);
    ASSERT_EQ("No options given\n", err);
}
