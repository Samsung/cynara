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
 * @file        src/cyad/CommandlineParser/CyadCommandlineParser.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Commandline parser for Cyad
 */

#include <cstring>
#include <getopt.h>
#include <map>
#include <sstream>
#include <string>

#include <cyad/CommandlineParser/PolicyParsingException.h>

#include "CyadCommandlineParser.h"

namespace Cynara {

namespace CyadCmdlineArgs {
    const char HELP = 'h';
    const char * const HELP_LONG = "help";

    const char SET_BUCKET = 'b';
    const char * const SET_BUCKET_LONG = "set-bucket";

    const char DELETE_BUCKET = 'd';
    const char * const DELETE_BUCKET_LONG = "delete-bucket";

    const char POLICY = 'p';
    const char * const POLICY_LONG = "policy";

    const char METADATA = 'm';
    const char * const METADATA_LONG = "metadata";
}

namespace CyadCmdlineErrors {
    const char * const UNKNOWN_ERROR = "Unknown error";
    const char * const NO_OPTION = "No option specified";
    const char * const UNKNOWN_OPTION = "Unknown option";
    const char * const UNKNOWN_OPTION_SET_BUCKET = "Unknown option in --set-bucket";
    const char * const UNKNOWN_OPTION_DELETE_BUCKET = "Unknown option in --delete-bucket";
    const char * const NO_POLICY = "No --policy specified";
    const char * const NO_BUCKET = "No bucket specified";
    const char * const INVALID_POLICY = "Invalid --policy option";
}

CyadCommandlineParser::CyadCommandlineParser(int argc, char * const *argv)
    : m_argc(argc), m_argv(argv) {}

CyadCommandlineParser::~CyadCommandlineParser() {}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseMain(void) {
    namespace Args = CyadCmdlineArgs;

    const struct option long_options[] = {
        { Args::HELP_LONG,          no_argument,       nullptr, Args::HELP },
        { Args::SET_BUCKET_LONG,    required_argument, nullptr, Args::SET_BUCKET },
        { Args::DELETE_BUCKET_LONG, required_argument, nullptr, Args::DELETE_BUCKET },
        { nullptr, 0, nullptr, 0 }
    };

    optind = 0; // On entry to `getopt', zero means this is the first call; initialize.
    int opt;
    std::stringstream optstr;
    optstr << ":" << Args::HELP
           << Args::SET_BUCKET << ":"
           << Args::DELETE_BUCKET << ":";

    while ((opt = getopt_long(m_argc, m_argv, optstr.str().c_str(), long_options, nullptr)) != -1) {
        switch (opt) {
            case Args::HELP:
                return std::make_shared<HelpCyadCommand>();

            case Args::SET_BUCKET:
                return parseSetBucket(optarg);

            case Args::DELETE_BUCKET:
                return parseDeleteBucket(optarg);

            case '?': // Unknown option
                return std::make_shared<ErrorCyadCommand>(CyadCmdlineErrors::UNKNOWN_OPTION);

            case ':': // Missing argument
                switch (optopt) {
                    case Args::SET_BUCKET:
                    case Args::DELETE_BUCKET:
                        return std::make_shared<ErrorCyadCommand>(CyadCmdlineErrors::NO_BUCKET);
                }
                // Shall never happen, but let's just make compiler happy.
                return std::make_shared<ErrorCyadCommand>(CyadCmdlineErrors::UNKNOWN_ERROR);

            default:
                return std::make_shared<ErrorCyadCommand>(CyadCmdlineErrors::UNKNOWN_OPTION);
        }
    }

    return std::make_shared<ErrorCyadCommand>(CyadCmdlineErrors::NO_OPTION);
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseSetBucket(const std::string &bucketId) {
    namespace Args = CyadCmdlineArgs;
    namespace Errors = CyadCmdlineErrors;

    const struct option long_options[] = {
        { Args::POLICY_LONG, required_argument, nullptr, Args::POLICY },
        { Args::METADATA_LONG, required_argument, nullptr, Args::METADATA },
        { nullptr, 0, nullptr, 0 }
    };

    std::string policy;
    std::string metadata;

    int opt;
    std::stringstream optstr;
    optstr << ":"
           << Args::POLICY << ":"
           << Args::METADATA << ":";

    while ((opt = getopt_long(m_argc, m_argv, optstr.str().c_str(), long_options, nullptr)) != -1) {
        switch(opt) {
        case Args::POLICY:
            policy = optarg;
            break;
        case Args::METADATA:
            metadata = optarg;
            break;
        default:
            return std::make_shared<ErrorCyadCommand>(Errors::UNKNOWN_OPTION_SET_BUCKET);
        }
    }

    if (policy.empty())
        return std::make_shared<ErrorCyadCommand>(Errors::NO_POLICY);

    try {
        auto policyType = parsePolicyType(policy);
        return std::make_shared<SetBucketCyadCommand>(bucketId, PolicyResult(policyType, metadata));
    } catch (const PolicyParsingException &) {
        return std::make_shared<ErrorCyadCommand>(Errors::INVALID_POLICY);
    }
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseDeleteBucket(const std::string &bucketId) {
    namespace Errors = CyadCmdlineErrors;

    // Expect no additional options
    const struct option long_options[] = {
        { nullptr, 0, nullptr, 0 }
    };

    if (getopt_long(m_argc, m_argv, ":", long_options, nullptr) == -1) {
        return std::make_shared<DeleteBucketCyadCommand>(bucketId);
    } else {
        return std::make_shared<ErrorCyadCommand>(Errors::UNKNOWN_OPTION_DELETE_BUCKET);
    }
}

PolicyType CyadCommandlineParser::parsePolicyType(const std::string &rawPolicy) {
    if (rawPolicy == "DENY")
        return CYNARA_ADMIN_DENY;
    if (rawPolicy == "NONE")
        return CYNARA_ADMIN_NONE;
    if (rawPolicy == "BUCKET")
        return CYNARA_ADMIN_BUCKET;
    if (rawPolicy == "ALLOW")
        return CYNARA_ADMIN_ALLOW;

    // If base for std::stoi is set to 0, it detects base by itself from the format of the sequence
    try {
        return std::stoi(rawPolicy, nullptr, 0);
    } catch (const std::logic_error &) {
        throw PolicyParsingException();
    }
}

} /* namespace Cynara */
