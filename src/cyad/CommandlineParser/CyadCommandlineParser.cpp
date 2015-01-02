/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <cyad/CommandlineParser/HumanReadableParser.h>
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

    const char TYPE = 't';
    const char * const TYPE_LONG = "type";

    const char METADATA = 'm';
    const char * const METADATA_LONG = "metadata";

    const char BUCKET = 'k';
    const char * const BUCKET_LONG = "bucket";

    const char SET_POLICY = 's';
    const char * const SET_POLICY_LONG = "set-policy";

    const char CLIENT = 'c';
    const char * const CLIENT_LONG = "client";

    const char USER = 'u';
    const char * const USER_LONG = "user";

    const char PRIVILEGE = 'p';
    const char * const PRIVILEGE_LONG = "privilege";

    const char BULK = 'f';
    const char * const BULK_LONG = "bulk";

    const char ERASE = 'e';
    const char * const ERASE_LONG = "erase";

    const char RECURSIVE = 'r';
    const char * const RECURSIVE_LONG = "recursive";
}

namespace CyadCmdlineErrors {
    const char * const UNKNOWN_ERROR = "Unknown error";
    const char * const NO_OPTION = "No option specified";
    const char * const UNKNOWN_OPTION = "Unknown option";
    const char * const UNKNOWN_OPTION_SET_BUCKET = "Unknown option in --set-bucket";
    const char * const UNKNOWN_OPTION_DELETE_BUCKET = "Unknown option in --delete-bucket";
    const char * const UNKNOWN_OPTION_SET_POLICY = "Unknown option in --set-policy";
    const char * const UNKNOWN_OPTION_ERASE = "Unknown option in --erase (-e)";
    const char * const NO_POLICY = "No --policy specified";
    const char * const NO_BUCKET = "No bucket specified";
    const char * const INVALID_POLICY = "Invalid --policy option";
    const char * const OPTION_MISSING_SET_POLICY = "One or more option missing in --set-policy";
    const char * const ARGUMENT_MISSING_SET_POLICY = "One or more argument missing in --set-policy";
    const char * const OPTION_MISSING_ERASE = "One or more option missing in --erase (-e)";
    const char * const ARGUMENT_MISSING_ERASE = "One or more argument missing in --erase (-e)";
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
        { Args::SET_POLICY_LONG,    no_argument,       nullptr, Args::SET_POLICY },
        { Args::ERASE_LONG,         required_argument, nullptr, Args::ERASE },
        { nullptr, 0, nullptr, 0 }
    };

    optind = 0; // On entry to `getopt', zero means this is the first call; initialize.
    int opt;
    std::stringstream optstr;
    optstr << ":" << Args::HELP
           << Args::SET_BUCKET << ":"
           << Args::DELETE_BUCKET << ":"
           << Args::SET_POLICY
           << Args::ERASE << ":";

    while ((opt = getopt_long(m_argc, m_argv, optstr.str().c_str(), long_options, nullptr)) != -1) {
        switch (opt) {
            case Args::HELP:
                return std::make_shared<HelpCyadCommand>();

            case Args::SET_BUCKET:
                return parseSetBucket(optarg);

            case Args::DELETE_BUCKET:
                return parseDeleteBucket(optarg);

            case Args::SET_POLICY:
                return parseSetPolicy();

            case Args::ERASE:
                return parseErase(optarg);

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
        { Args::TYPE_LONG,     required_argument, nullptr, Args::TYPE },
        { Args::METADATA_LONG, required_argument, nullptr, Args::METADATA },
        { nullptr, 0, nullptr, 0 }
    };

    std::string policy;
    std::string metadata;

    int opt;
    std::stringstream optstr;
    optstr << ":"
           << Args::TYPE << ":"
           << Args::METADATA << ":";

    while ((opt = getopt_long(m_argc, m_argv, optstr.str().c_str(), long_options, nullptr)) != -1) {
        switch(opt) {
        case Args::TYPE:
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
        auto policyType = HumanReadableParser::policyType(policy);
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

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseSetPolicy(void) {
    namespace Args = CyadCmdlineArgs;
    namespace Errors = CyadCmdlineErrors;

    const struct option long_options[] = {
        { Args::CLIENT_LONG,    required_argument, nullptr, Args::CLIENT },
        { Args::USER_LONG,      required_argument, nullptr, Args::USER },
        { Args::PRIVILEGE_LONG, required_argument, nullptr, Args::PRIVILEGE },
        { Args::BUCKET_LONG,    required_argument, nullptr, Args::BUCKET },
        { Args::TYPE_LONG,      required_argument, nullptr, Args::TYPE },
        { Args::METADATA_LONG,  required_argument, nullptr, Args::METADATA },
        { Args::BULK_LONG,      required_argument, nullptr, Args::BULK },
        { nullptr, 0, nullptr, 0 }
    };

    typedef std::map<char, std::string> OptionsValues;
    OptionsValues values = { { Args::CLIENT,    std::string() },
                             { Args::USER,      std::string() },
                             { Args::PRIVILEGE, std::string() },
                             { Args::TYPE,      std::string() } };

    std::string metadata;
    std::string bucket;

    int opt;
    std::stringstream optstr;
    optstr << ":"
           << Args::CLIENT << ":"
           << Args::USER << ":"
           << Args::PRIVILEGE << ":"
           << Args::BUCKET << ":"
           << Args::TYPE << ":"
           << Args::METADATA << ":"
           << Args::BULK << ":";

    while ((opt = getopt_long(m_argc, m_argv, optstr.str().c_str(), long_options, nullptr)) != -1) {
        switch(opt) {
        case Args::CLIENT:
        case Args::USER:
        case Args::PRIVILEGE:
        case Args::TYPE:
            values[opt] = optarg;
            break;
        case Args::BUCKET:
            bucket = optarg;
            break;
        case Args::METADATA:
            metadata = optarg;
            break;
        case Args::BULK:
            return std::make_shared<SetPolicyBulkCyadCommand>(optarg);
        case ':': // Missing argument
            return std::make_shared<ErrorCyadCommand>(Errors::ARGUMENT_MISSING_SET_POLICY);
        default:
            return std::make_shared<ErrorCyadCommand>(Errors::UNKNOWN_OPTION_SET_POLICY);
        }
    }

    for (const auto &val : values) {
        if (val.second.empty()) {
            // TODO: Identify actual option
            return std::make_shared<ErrorCyadCommand>(Errors::OPTION_MISSING_SET_POLICY);
        }
    }

    try {
        auto policyType = HumanReadableParser::policyType(values[Args::TYPE]);
        auto policyResult = PolicyResult(policyType, metadata);
        return std::make_shared<SetPolicyCyadCommand>(bucket, policyResult,
                                                      PolicyKey(values[Args::CLIENT],
                                                                values[Args::USER],
                                                                values[Args::PRIVILEGE]));
    } catch (const PolicyParsingException &) {
        return std::make_shared<ErrorCyadCommand>(Errors::INVALID_POLICY);
    }

    return std::make_shared<ErrorCyadCommand>(Errors::UNKNOWN_ERROR);
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseErase(const std::string &bucketId) {
    namespace Args = CyadCmdlineArgs;
    namespace Errors = CyadCmdlineErrors;

    const struct option long_options[] = {
        { Args::RECURSIVE_LONG, required_argument, nullptr, Args::RECURSIVE },
        { Args::CLIENT_LONG,    required_argument, nullptr, Args::CLIENT },
        { Args::USER_LONG,      required_argument, nullptr, Args::USER },
        { Args::PRIVILEGE_LONG, required_argument, nullptr, Args::PRIVILEGE },
        { nullptr, 0, nullptr, 0 }
    };

    typedef std::map<char, std::string> OptionsValues;
    OptionsValues values = { { Args::RECURSIVE, std::string() },
                             { Args::CLIENT,    std::string() },
                             { Args::USER,      std::string() },
                             { Args::PRIVILEGE, std::string() } };

    int opt;
    std::stringstream optstr;
    optstr << ":"
           << Args::RECURSIVE << ":"
           << Args::CLIENT << ":"
           << Args::USER << ":"
           << Args::PRIVILEGE << ":";

    while ((opt = getopt_long(m_argc, m_argv, optstr.str().c_str(), long_options, nullptr)) != -1) {
        switch (opt) {
        case Args::RECURSIVE:
        case Args::CLIENT:
        case Args::USER:
        case Args::PRIVILEGE:
            values[opt] = optarg;
            break;
        case ':': // Missing argument
            return std::make_shared<ErrorCyadCommand>(Errors::ARGUMENT_MISSING_ERASE);
        default:
            return std::make_shared<ErrorCyadCommand>(Errors::UNKNOWN_OPTION_ERASE);
        }
    }

    for (const auto &val : values) {
        if (val.second.empty()) {
            // TODO: Identify actual option
            return std::make_shared<ErrorCyadCommand>(Errors::OPTION_MISSING_ERASE);
        }
    }

    auto recursive = HumanReadableParser::isYes(values[Args::RECURSIVE]);


    return std::make_shared<EraseCyadCommand>(bucketId, recursive,
                                              PolicyKey(values[Args::CLIENT], values[Args::USER],
                                                        values[Args::PRIVILEGE]));
}

} /* namespace Cynara */
