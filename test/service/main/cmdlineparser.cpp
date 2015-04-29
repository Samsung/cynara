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
const std::string helpMessage(
    "Usage: " + execName + " [OPTIONS]\n\n"
    "Information mode options [program exits after printing information]:\n"
    "  -V, --version                print version of " + execName + " and exit\n"
    "  -h, --help                   print this help message and exit\n"
    "Normal work mode options:\n"
    "  -d, --daemon                 daemonize "
    "[by default " + execName + " does not daemonize]\n"
    "  -m, --mask=MASK              set umask to MASK "
    "[by default no umask is set]\n"
    "  -u, --user=USER              change user to USER "
                 "[by default uid is not changed]\n"
    "  -g, --group=GROUP            change group to GROUP "
                 "[by default gid is not changed]\n");

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
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
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
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
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
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ(std::string("Unknown option: ") + badOpt + "\n", err);
    }
}

/**
 * @brief   Verify if passing no options to commandline handler succeeds
 * @test    Expected result:
 * - call handler indicates success
 * - empty output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, noOption) {
    std::string err;
    std::string out;

    clearOutput();
    prepare_argv({ execName });

    const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
    getOutput(out, err);

    ASSERT_FALSE(options.m_error);
    ASSERT_FALSE(options.m_exit);
    ASSERT_FALSE(options.m_daemon);
    ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
    ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
    ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
    ASSERT_TRUE(out.empty());
    ASSERT_TRUE(err.empty());
}

/**
 * @brief   Verify if passing daemon option to commandline succeeds
 * @test    Expected result:
 * - call handler indicates success
 * - empty output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, daemonOption) {
    std::string err;
    std::string out;

    for (const auto &daemonOpt : { "-d", "--daemon" }) {
        clearOutput();
        prepare_argv({ execName, daemonOpt });

        SCOPED_TRACE(daemonOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_FALSE(options.m_exit);
        ASSERT_TRUE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_TRUE(out.empty());
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing mask option to commandline succeeds
 * @test    Expected result:
 * - call handler indicates success
 * - empty output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, maskOption) {
    std::string err;
    std::string out;

    std::string maskParam("0666");

    for (const auto &maskOpt : { "-m", "--mask" }) {
        clearOutput();
        prepare_argv({ execName, maskOpt, maskParam});

        SCOPED_TRACE(maskOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_FALSE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(0666));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_TRUE(out.empty());
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing invalid mask option to commandline fails
 * @test    Expected result:
 * - call handler indicates failure
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, maskOptionInvalid) {
    std::string err;
    std::string out;

    std::string maskParam("MASK");

    for (const auto &maskOpt : { "-m", "--mask" }) {
        clearOutput();
        prepare_argv({ execName, maskOpt, maskParam});

        SCOPED_TRACE(maskOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ("Invalid param: MASK\n", err);
    }
}

/**
 * @brief   Verify if passing no mask option to commandline fails
 * @test    Expected result:
 * - call handler indicates failure
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, maskOptionNoParam) {
    std::string err;
    std::string out;

    for (const auto &maskOpt : { "-m", "--mask" }) {
        clearOutput();
        prepare_argv({ execName, maskOpt});

        SCOPED_TRACE(maskOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ(std::string("Missing argument for option: ") + maskOpt + "\n", err);
    }
}

/**
 * @brief   Verify if passing user option to commandline succeeds
 * @test    Expected result:
 * - call handler indicates success
 * - empty output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, userOptionName) {
    std::string err;
    std::string out;

    std::string userParam("root");

    for (const auto &userOpt : { "-u", "--user" }) {
        clearOutput();
        prepare_argv({ execName, userOpt, userParam});

        SCOPED_TRACE(userOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_FALSE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(0));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_TRUE(out.empty());
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing user option to commandline succeeds
 * @test    Expected result:
 * - call handler indicates success
 * - empty output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, userOptionUid) {
    std::string err;
    std::string out;

    std::string userParam("1234");

    for (const auto &userOpt : { "-u", "--user" }) {
        clearOutput();
        prepare_argv({ execName, userOpt, userParam});

        SCOPED_TRACE(userOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_FALSE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(1234));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_TRUE(out.empty());
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing invalid user option to commandline fails
 * @test    Expected result:
 * - call handler indicates success
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, userOptionInvalid) {
    std::string err;
    std::string out;

    std::string userParam("UserThatDoNotExist");

    for (const auto &userOpt : { "-u", "--user" }) {
        clearOutput();
        prepare_argv({ execName, userOpt, userParam});

        SCOPED_TRACE(userOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ("Invalid param: UserThatDoNotExist\n", err);
    }
}

/**
 * @brief   Verify if passing no user option to commandline fails
 * @test    Expected result:
 * - call handler indicates failure
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, userOptionNoParam) {
    std::string err;
    std::string out;

    for (const auto &userOpt : { "-u", "--user" }) {
        clearOutput();
        prepare_argv({ execName, userOpt});

        SCOPED_TRACE(userOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ(std::string("Missing argument for option: ") + userOpt + "\n", err);
    }
}

/**
 * @brief   Verify if passing group option to commandline succeeds
 * @test    Expected result:
 * - call handler indicates success
 * - empty output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, groupOptionName) {
    std::string err;
    std::string out;

    std::string groupParam("root");

    for (const auto &groupOpt : { "-g", "--group" }) {
        clearOutput();
        prepare_argv({ execName, groupOpt, groupParam});

        SCOPED_TRACE(groupOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_FALSE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(0));
        ASSERT_TRUE(out.empty());
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing group option to commandline succeeds
 * @test    Expected result:
 * - call handler indicates success
 * - empty output stream
 * - empty error stream
 */
TEST_F(CynaraCommandlineTest, groupOptionGid) {
    std::string err;
    std::string out;

    std::string groupParam("1234");

    for (const auto &groupOpt : { "-g", "--group" }) {
        clearOutput();
        prepare_argv({ execName, groupOpt, groupParam});

        SCOPED_TRACE(groupOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_FALSE(options.m_error);
        ASSERT_FALSE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(1234));
        ASSERT_TRUE(out.empty());
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if passing invalid group option to commandline fails
 * @test    Expected result:
 * - call handler indicates success
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, groupOptionInvalid) {
    std::string err;
    std::string out;

    std::string groupParam("GroupThatDoNotExist");

    for (const auto &groupOpt : { "-g", "--group" }) {
        clearOutput();
        prepare_argv({ execName, groupOpt, groupParam});

        SCOPED_TRACE(groupOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ("Invalid param: GroupThatDoNotExist\n", err);
    }
}

/**
 * @brief   Verify if passing no group option to commandline fails
 * @test    Expected result:
 * - call handler indicates failure
 * - help message in output stream
 * - error message in error stream
 */
TEST_F(CynaraCommandlineTest, groupOptionNoParam) {
    std::string err;
    std::string out;

    for (const auto &groupOpt : { "-g", "--group" }) {
        clearOutput();
        prepare_argv({ execName, groupOpt});

        SCOPED_TRACE(groupOpt);
        const auto options = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(out, err);

        ASSERT_TRUE(options.m_error);
        ASSERT_TRUE(options.m_exit);
        ASSERT_FALSE(options.m_daemon);
        ASSERT_EQ(options.m_mask, static_cast<mode_t>(-1));
        ASSERT_EQ(options.m_uid, static_cast<uid_t>(-1));
        ASSERT_EQ(options.m_gid, static_cast<gid_t>(-1));
        ASSERT_EQ(helpMessage, out);
        ASSERT_EQ(std::string("Missing argument for option: ") + groupOpt + "\n", err);
    }
}
