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

#include <functional>
#include <getopt.h>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include <cynara-admin-types.h>

#include <cyad/CommandlineParser/CmdlineErrors.h>
#include <cyad/CommandlineParser/CmdlineOpts.h>
#include <cyad/CommandlineParser/HumanReadableParser.h>
#include <cyad/CommandlineParser/PolicyParsingException.h>

#include "CyadCommandlineParser.h"

namespace Cynara {

CyadCommandlineParser::CyadCommandlineParser(int argc, char * const *argv)
    : m_argc(argc), m_argv(argv) {}

CyadCommandlineParser::~CyadCommandlineParser() {}

static std::shared_ptr<ErrorCyadCommand> sharedError(const std::string &message) {
    return std::make_shared<ErrorCyadCommand>(message);
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseMain(void) {
    namespace Opts = CmdlineOpts;
    namespace Err = CmdlineErrors;
    using CmdlineOpts::CmdlineOpt;

    std::vector<CmdlineOpt> opts = {
        CmdlineOpt::Help,
        CmdlineOpt::SetBucket,
        CmdlineOpt::DeleteBucket,
        CmdlineOpt::SetPolicy,
        CmdlineOpt::Erase,
        CmdlineOpt::Check,
        CmdlineOpt::ListPolicies,
        CmdlineOpt::ListPoliciesDesc
    };

    const auto longOpts = Opts::makeLongOptions(opts);
    const auto shortOpts = Opts::makeShortOptions(opts);

    optind = 0; // On entry to `getopt', zero means this is the first call; initialize.
    int opt;
    while ((opt = getopt_long(m_argc, m_argv, shortOpts.data(), longOpts.data(), nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Help:
                return std::make_shared<HelpCyadCommand>();

            case CmdlineOpt::SetBucket:
                return parseSetBucket(optarg);

            case CmdlineOpt::DeleteBucket:
                return parseDeleteBucket(optarg);

            case CmdlineOpt::SetPolicy:
                return parseSetPolicy();

            case CmdlineOpt::Erase:
                return parseErase(optarg);

            case CmdlineOpt::Check:
                return parseCheck();

            case CmdlineOpt::ListPolicies:
                return parseListPolicies(optarg);

            case CmdlineOpt::ListPoliciesDesc:
                return std::make_shared<ListPoliciesDescCyadCommand>();

            case '?': // Unknown option
                return sharedError(Err::unknownOption());

            case ':': // Missing argument
                switch (optopt) {
                    case CmdlineOpt::SetBucket:
                    case CmdlineOpt::DeleteBucket:
                    case CmdlineOpt::Erase:
                    case CmdlineOpt::ListPolicies:
                        return sharedError(Err::noBucket());
                }
                // Shall never happen, but let's just make compiler happy.
                return sharedError(Err::unknownError());

            default:
                return sharedError(Err::unknownOption());
        }
    }

    return sharedError(Err::noOption());
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseSetBucket(const std::string &bucketId) {
    namespace Opts = CmdlineOpts;
    namespace Err = CmdlineErrors;
    using CmdlineOpts::CmdlineOpt;

    std::vector<CmdlineOpt> opts = {
        CmdlineOpt::Type,
        CmdlineOpt::Metadata
    };

    const auto longOpts = Opts::makeLongOptions(opts);
    const auto shortOpts = Opts::makeShortOptions(opts);

    std::string policy;
    std::string metadata;

    int opt;
    while ((opt = getopt_long(m_argc, m_argv, shortOpts.data(), longOpts.data(), nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Type:
                policy = optarg;
                break;
            case CmdlineOpt::Metadata:
                metadata = optarg;
                break;
            default:
                return sharedError(Err::unknownOption(CmdlineOpt::SetBucket));
        }
    }

    if (policy.empty())
        return sharedError(Err::noType());

    return std::make_shared<SetBucketCyadCommand>(bucketId, RawPolicyResult(policy, metadata));
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseDeleteBucket(const std::string &bucketId) {
    namespace Opts = CmdlineOpts;
    namespace Err = CmdlineErrors;
    using CmdlineOpts::CmdlineOpt;

    // Expect no additional options
    std::vector<CmdlineOpt> opts = {};
    const auto longOpts = Opts::makeLongOptions(opts);
    const auto shortOpts = Opts::makeShortOptions(opts);

    if (getopt_long(m_argc, m_argv, shortOpts.data(), longOpts.data(), nullptr) == -1) {
        return std::make_shared<DeleteBucketCyadCommand>(bucketId);
    } else {
        return sharedError(Err::unknownOption(CmdlineOpt::DeleteBucket));
    }
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseSetPolicy(void) {
    namespace Opts = CmdlineOpts;
    namespace Err = CmdlineErrors;
    using CmdlineOpts::CmdlineOpt;

    std::vector<CmdlineOpt> opts = {
        CmdlineOpt::Client,
        CmdlineOpt::User,
        CmdlineOpt::Privilege,
        CmdlineOpt::Bucket,
        CmdlineOpt::Type,
        CmdlineOpt::Metadata,
        CmdlineOpt::Bulk
    };

    const auto longOpts = Opts::makeLongOptions(opts);
    const auto shortOpts = Opts::makeShortOptions(opts);

    typedef std::unordered_map<char, std::string> OptionsValues;
    OptionsValues values = { { CmdlineOpt::Client,    std::string() },
                             { CmdlineOpt::User,      std::string() },
                             { CmdlineOpt::Privilege, std::string() },
                             { CmdlineOpt::Type,      std::string() } };

    std::string metadata;
    std::string bucket = CYNARA_ADMIN_DEFAULT_BUCKET;

    int opt;
    while ((opt = getopt_long(m_argc, m_argv, shortOpts.data(), longOpts.data(), nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Client:
            case CmdlineOpt::User:
            case CmdlineOpt::Privilege:
            case CmdlineOpt::Type:
                values[opt] = optarg;
                break;
            case CmdlineOpt::Bucket:
                bucket = optarg;
                break;
            case CmdlineOpt::Metadata:
                metadata = optarg;
                break;
            case CmdlineOpt::Bulk:
                return std::make_shared<SetPolicyBulkCyadCommand>(optarg);
            case ':': // Missing argument
                return sharedError(Err::argumentMissing(CmdlineOpt::SetPolicy));
            default:
                return sharedError(Err::unknownOption(CmdlineOpt::SetPolicy));
        }
    }

    for (const auto &val : values) {
        if (val.second.empty()) {
            // TODO: Identify actual option
            return sharedError(Err::optionMissing(CmdlineOpt::SetPolicy));
        }
    }

    auto policyResult = RawPolicyResult(values[CmdlineOpt::Type], metadata);
    return std::make_shared<SetPolicyCyadCommand>(bucket, policyResult,
                                                  PolicyKey(values[CmdlineOpt::Client],
                                                            values[CmdlineOpt::User],
                                                            values[CmdlineOpt::Privilege]));
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseErase(const std::string &bucketId) {
    namespace Opts = CmdlineOpts;
    namespace Err = CmdlineErrors;
    using CmdlineOpts::CmdlineOpt;

    std::vector<CmdlineOpt> opts = {
        CmdlineOpt::Recursive,
        CmdlineOpt::Client,
        CmdlineOpt::User,
        CmdlineOpt::Privilege
    };

    const auto longOpts = Opts::makeLongOptions(opts);
    const auto shortOpts = Opts::makeShortOptions(opts);

    typedef std::unordered_map<char, std::string> OptionsValues;
    OptionsValues values = { { CmdlineOpt::Recursive, std::string() },
                             { CmdlineOpt::Client,    std::string() },
                             { CmdlineOpt::User,      std::string() },
                             { CmdlineOpt::Privilege, std::string() } };

    int opt;
    while ((opt = getopt_long(m_argc, m_argv, shortOpts.data(), longOpts.data(), nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Recursive:
            case CmdlineOpt::Client:
            case CmdlineOpt::User:
            case CmdlineOpt::Privilege:
                values[opt] = optarg;
                break;
            case ':': // Missing argument
                return sharedError(Err::argumentMissing(CmdlineOpt::Erase));
            default:
                return sharedError(Err::unknownOption(CmdlineOpt::Erase));
        }
    }

    for (const auto &val : values) {
        if (val.second.empty()) {
            // TODO: Identify actual option
            return sharedError(Err::optionMissing(CmdlineOpt::Erase));
        }
    }

    auto recursive = HumanReadableParser::isYes(values[CmdlineOpt::Recursive]);


    return std::make_shared<EraseCyadCommand>(bucketId, recursive,
                                              PolicyKey(values[CmdlineOpt::Client],
                                                        values[CmdlineOpt::User],
                                                        values[CmdlineOpt::Privilege]));
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseCheck(void) {
    namespace Opts = CmdlineOpts;
    namespace Err = CmdlineErrors;
    using CmdlineOpts::CmdlineOpt;

    std::vector<CmdlineOpt> opts = {
        CmdlineOpt::Bucket,
        CmdlineOpt::Recursive,
        CmdlineOpt::Client,
        CmdlineOpt::User,
        CmdlineOpt::Privilege
    };

    const auto longOpts = Opts::makeLongOptions(opts);
    const auto shortOpts = Opts::makeShortOptions(opts);

    typedef std::unordered_map<char, std::string> OptionsValues;
    OptionsValues values = { { CmdlineOpt::Recursive, std::string() },
                             { CmdlineOpt::Client,    std::string() },
                             { CmdlineOpt::User,      std::string() },
                             { CmdlineOpt::Privilege, std::string() } };

    std::string bucketId = CYNARA_ADMIN_DEFAULT_BUCKET;

    int opt;
    while ((opt = getopt_long(m_argc, m_argv, shortOpts.data(), longOpts.data(), nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Recursive:
            case CmdlineOpt::Client:
            case CmdlineOpt::User:
            case CmdlineOpt::Privilege:
                values[opt] = optarg;
                break;
            case CmdlineOpt::Bucket:
                bucketId = optarg;
                break;
            case ':': // Missing argument
                return sharedError(Err::argumentMissing(CmdlineOpt::Check));
            default:
                return sharedError(Err::unknownOption(CmdlineOpt::Check));
        }
    }

    for (const auto &val : values) {
        if (val.second.empty()) {
            // TODO: Identify actual option
            return sharedError(Err::optionMissing(CmdlineOpt::Check));
        }
    }

    auto recursive = HumanReadableParser::isYes(values[CmdlineOpt::Recursive]);

    return std::make_shared<CheckCyadCommand>(bucketId, recursive,
                                              PolicyKey(values[CmdlineOpt::Client],
                                                        values[CmdlineOpt::User],
                                                        values[CmdlineOpt::Privilege]));
}

std::shared_ptr<CyadCommand> CyadCommandlineParser::parseListPolicies(const std::string &bucketId) {
    namespace Opts = CmdlineOpts;
    namespace Err = CmdlineErrors;
    using CmdlineOpts::CmdlineOpt;

    std::vector<CmdlineOpt> opts = {
        CmdlineOpt::Client,
        CmdlineOpt::User,
        CmdlineOpt::Privilege
    };

    const auto longOpts = Opts::makeLongOptions(opts);
    const auto shortOpts = Opts::makeShortOptions(opts);

    typedef std::unordered_map<char, std::string> OptionsValues;
    OptionsValues values = { { CmdlineOpt::Client,    std::string() },
                             { CmdlineOpt::User,      std::string() },
                             { CmdlineOpt::Privilege, std::string() } };

    int opt;
    while ((opt = getopt_long(m_argc, m_argv, shortOpts.data(), longOpts.data(), nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Client:
            case CmdlineOpt::User:
            case CmdlineOpt::Privilege:
                values[opt] = optarg;
                break;
            case ':': // Missing argument
                return sharedError(Err::argumentMissing(CmdlineOpt::ListPolicies));
            default:
                return sharedError(Err::unknownOption(CmdlineOpt::ListPolicies));
        }
    }

    for (const auto &val : values) {
        if (val.second.empty()) {
            // TODO: Identify actual option
            return sharedError(Err::optionMissing(CmdlineOpt::ListPolicies));
        }
    }

    return std::make_shared<ListPoliciesCyadCommand>(bucketId,
                                                     PolicyKey(values[CmdlineOpt::Client],
                                                               values[CmdlineOpt::User],
                                                               values[CmdlineOpt::Privilege]));
}

} /* namespace Cynara */
