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
 * @file        src/cyad/CommandlineParser/CmdlineOpts.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Oskar Świtalski <o.switalski@samsung.com>
 * @version     1.0
 * @brief       Command-line structs and helpers
 */

#include <cassert>
#include <sstream>

#include "CmdlineOpts.h"

namespace Cynara {

namespace CmdlineOpts {

const OptionsMap commandlineOptions = {
    { CmdlineOpt::SetBucket,
        { "set-bucket", CmdlineOpt::SetBucket, "set-bucket=<name>",
           "name of bucket to add or alter", OptHasArg::RequiredArgument, OptIsReq::NotRequired } },
    { CmdlineOpt::DeleteBucket,
        { "delete-bucket", CmdlineOpt::DeleteBucket, "delete-bucket=<name>",
          "name of bucket to delete", OptHasArg::RequiredArgument, OptIsReq::Required } },
    { CmdlineOpt::SetPolicy,
        { "set-policy", CmdlineOpt::SetPolicy, "",
          "", OptHasArg::NoArgument, OptIsReq::NotRequired } },
    { CmdlineOpt::Erase,
        { "erase", CmdlineOpt::Erase, "erase=<name>",
          "name of bucket to erase policies from", OptHasArg::RequiredArgument,
          OptIsReq::Required } },
    { CmdlineOpt::Check,
        { "check", CmdlineOpt::Check, "check",
          "", OptHasArg::NoArgument, OptIsReq::NotRequired } },
    { CmdlineOpt::ListPolicies,
        { "list-policies", CmdlineOpt::ListPolicies, "list-policies=<bucket>",
          "name of bucket to list policies from", OptHasArg::RequiredArgument,
          OptIsReq::Required } },
    { CmdlineOpt::ListPoliciesDesc,
        { "list-policies-descriptions", CmdlineOpt::ListPoliciesDesc, "list-policies-descriptions",
          "", OptHasArg::NoArgument, OptIsReq::NotRequired } },

    { CmdlineOpt::Type,
        { "type", CmdlineOpt::Type, "type=<type>",
          "policy type", OptHasArg::RequiredArgument, OptIsReq::Required } },
    { CmdlineOpt::Metadata,
        { "metadata", CmdlineOpt::Metadata, "metadata=<metadata>",
          "metadata for policy", OptHasArg::RequiredArgument, OptIsReq::NotRequired } },

    { CmdlineOpt::Client,
        { "client", CmdlineOpt::Client, "client=<client>",
          "client value", OptHasArg::RequiredArgument, OptIsReq::Required } },
    { CmdlineOpt::User,
        { "user", CmdlineOpt::User, "user=<user>",
          "user value", OptHasArg::RequiredArgument, OptIsReq::Required } },
    { CmdlineOpt::Privilege,
        { "privilege", CmdlineOpt::Privilege, "privilege=<privilege>",
          "privilege value", OptHasArg::RequiredArgument, OptIsReq::Required } },
    { CmdlineOpt::All,
        { "all", CmdlineOpt::All, "all", "list all - equivalent to -c \"#\" -u \"#\" -p \"#\"",
          OptHasArg::NoArgument, OptIsReq::NotRequired } },

    { CmdlineOpt::Bulk,
        { "bulk", CmdlineOpt::Bulk, "bulk=<filename>",
          "path or - for stdin", OptHasArg::RequiredArgument, OptIsReq::NotRequired } },

    { CmdlineOpt::Bucket,
        { "bucket", CmdlineOpt::Bucket, "bucket=<name>",
          "name of bucket; omit for default", OptHasArg::RequiredArgument,
          OptIsReq::NotRequired } },
    { CmdlineOpt::Recursive,
        { "recursive", CmdlineOpt::Recursive, "recursive=<yes|no>",
          "if linked buckets should be processed as well", OptHasArg::RequiredArgument,
          OptIsReq::Required } },

    { CmdlineOpt::Help,
        { "help", CmdlineOpt::Help, "help",
          "print help message", OptHasArg::NoArgument, OptIsReq::NotRequired } }
};

std::vector<option> makeLongOptions(const std::vector<CmdlineOpt> &opts) {
    std::vector<option> options;
    for (const auto &opt : opts) {
        const auto &o = commandlineOptions.at(opt);
        options.push_back({ o.longOption, o.hasArg, nullptr, static_cast<int>(opt) });
    }

    options.push_back({ nullptr, 0, nullptr, 0 });
    return options;
}

std::string makeShortOptions(const std::vector<CmdlineOpt> &opts) {
    std::stringstream optstr;
    optstr << ":";

    for (const auto &opt : opts) {
        const auto &o = commandlineOptions.at(opt);
        optstr << o.shortOption;

        if (o.hasArg == OptHasArg::RequiredArgument)
            optstr << ":";
    }

    return optstr.str();
}

std::string makeHelp(void) {

    std::size_t optWidth = 33;

    auto head = [] (const std::string &header, CmdlineOpt opt) -> std::string {
        return header + " (with -" + commandlineOptions.at(opt).shortOption
                      + " or --" + commandlineOptions.at(opt).longOption + ")";
    };

    auto opt = [&optWidth] (CmdlineOpt opt) -> std::string {
        auto prefix = std::string("  -") + commandlineOptions.at(opt).shortOption
                    + ", --" + commandlineOptions.at(opt).helpArgument;
        auto spaceLen = optWidth > prefix.length() ? optWidth - prefix.length() : 1;
        return prefix + std::string(spaceLen, ' ')
                      + commandlineOptions.at(opt).helpDescription
                      + (commandlineOptions.at(opt).isReq == Required ? " - required" : "");
    };

    std::stringstream helpStr;

    helpStr << "Usage: cyad [OPTIONS]" << std::endl << std::endl;

    helpStr << head("Bucket set options", CmdlineOpt::SetBucket) << std::endl;
    helpStr << opt(CmdlineOpt::SetBucket) << std::endl;
    helpStr << opt(CmdlineOpt::Type) << std::endl;
    helpStr << opt(CmdlineOpt::Metadata) << std::endl;
    helpStr << std::endl;

    helpStr << head("Bucket delete options", CmdlineOpt::DeleteBucket) << std::endl;
    helpStr << opt(CmdlineOpt::DeleteBucket) << std::endl;
    helpStr << std::endl;

    helpStr << head("Policy set options", CmdlineOpt::SetPolicy) << std::endl;
    helpStr << opt(CmdlineOpt::Bucket) << std::endl;
    helpStr << opt(CmdlineOpt::Client) << std::endl;
    helpStr << opt(CmdlineOpt::User) << std::endl;
    helpStr << opt(CmdlineOpt::Privilege) << std::endl;
    helpStr << opt(CmdlineOpt::Type) << std::endl;
    helpStr << opt(CmdlineOpt::Metadata) << std::endl;
    helpStr << opt(CmdlineOpt::Bulk) << std::endl;
    helpStr << std::endl;

    helpStr << "Bulk file" << std::endl;
    helpStr << "  For each policy a single line should be present with the folowing information:";
    helpStr << std::endl;
    helpStr << "    <bucket>;<client>;<user>;<privilege>;<type as decimal>;[metadata]" << std::endl;
    helpStr << "  In stdin input mode, double empty line ends this mode." << std::endl;
    helpStr << std::endl;

    helpStr << head("Policy erase options", CmdlineOpt::Erase) << std::endl;
    helpStr << opt(CmdlineOpt::Erase) << std::endl;
    helpStr << opt(CmdlineOpt::Recursive) << std::endl;
    helpStr << opt(CmdlineOpt::Client) << std::endl;
    helpStr << opt(CmdlineOpt::User) << std::endl;
    helpStr << opt(CmdlineOpt::Privilege) << std::endl;
    helpStr << std::endl;

    helpStr << head("Administrative policy check options", CmdlineOpt::Check) << std::endl;
    helpStr << opt(CmdlineOpt::Bucket) << std::endl;
    helpStr << opt(CmdlineOpt::Recursive) << std::endl;
    helpStr << opt(CmdlineOpt::Client) << std::endl;
    helpStr << opt(CmdlineOpt::User) << std::endl;
    helpStr << opt(CmdlineOpt::Privilege) << std::endl;
    helpStr << std::endl;

    helpStr << head("Policies list options", CmdlineOpt::ListPolicies) << std::endl;
    helpStr << opt(CmdlineOpt::ListPolicies) << std::endl;
    helpStr << opt(CmdlineOpt::Client) << std::endl;
    helpStr << opt(CmdlineOpt::User) << std::endl;
    helpStr << opt(CmdlineOpt::Privilege) << std::endl;
    helpStr << opt(CmdlineOpt::All) << std::endl;
    helpStr << std::endl;

    helpStr << head("Policies descriptions list options", CmdlineOpt::ListPoliciesDesc)
            << std::endl;
    helpStr << opt(CmdlineOpt::ListPoliciesDesc) << std::endl;
    helpStr << std::endl;

    helpStr << head("Help options", CmdlineOpt::Help) << std::endl;
    helpStr << opt(CmdlineOpt::Help);

    return helpStr.str();
}

} /* namespace CmdlineOpts */

} /* namespace Cynara */
