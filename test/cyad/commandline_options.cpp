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
 * @file        test/cyad/commandline_options.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for command-line options
 */

#include <cyad/CommandlineParser/CmdlineOpts.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(CommandlineOptions, allOptionsPresent) {
    using Cynara::CmdlineOpts::CmdlineOpt;
    using Cynara::CmdlineOpts::commandlineOptions;

    // A cheap trick to make sure this test is updated, when new options are added
    ASSERT_EQ(14, commandlineOptions.size());

    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::SetBucket));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::DeleteBucket));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::SetPolicy));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Erase));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Check));

    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Type));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Metadata));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Client));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::User));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Privilege));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Bulk));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Bucket));
    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Recursive));

    ASSERT_NO_THROW(commandlineOptions.at(CmdlineOpt::Help));
}


void ASSERT_OPTS_END_WITH_NULL(const std::vector<option> &opts) {
    const auto &lastOpt = opts.end() - 1;
    ASSERT_EQ(nullptr, lastOpt->name);
    ASSERT_EQ(0, lastOpt->has_arg);
    ASSERT_EQ(nullptr, lastOpt->flag);
    ASSERT_EQ(0, lastOpt->val);
}

TEST(CommandlineOptions, makeOptionsNone) {
    const auto opts = Cynara::CmdlineOpts::makeLongOptions({});
    ASSERT_OPTS_END_WITH_NULL(opts);
}

TEST(CommandlineOptions, makeOptionsOne) {
    using Cynara::CmdlineOpts::CmdlineOpt;
    using Cynara::CmdlineOpts::commandlineOptions;

    const auto opts = Cynara::CmdlineOpts::makeLongOptions({ CmdlineOpt::Help });

    ASSERT_OPTS_END_WITH_NULL(opts);
    ASSERT_STREQ(commandlineOptions.at(CmdlineOpt::Help).longOption, opts.at(0).name);
}

TEST(CommandlineOptions, makeOptionsMore) {
    using Cynara::CmdlineOpts::CmdlineOpt;
    using Cynara::CmdlineOpts::commandlineOptions;

    const auto opts = Cynara::CmdlineOpts::makeLongOptions({ CmdlineOpt::Help,
                                                             CmdlineOpt::SetPolicy });

    ASSERT_OPTS_END_WITH_NULL(opts);
    ASSERT_STREQ(commandlineOptions.at(CmdlineOpt::Help).longOption, opts.at(0).name);
    ASSERT_STREQ(commandlineOptions.at(CmdlineOpt::SetPolicy).longOption, opts.at(1).name);
}
