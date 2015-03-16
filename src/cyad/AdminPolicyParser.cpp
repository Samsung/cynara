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
 * @file        src/cyad/AdminPolicyParser.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Parses policies from input stream
 */

#include <config/PathConfig.h>
#include <exceptions/BucketRecordCorruptedException.h>
#include <storage/StorageDeserializer.h>

#include "AdminPolicyParser.h"

namespace Cynara {

namespace AdminPolicyParser {

CynaraAdminPolicies parse(const std::shared_ptr<std::istream> &input,
                          std::function<PolicyType(const std::string &)> translatePolicy) {
    CynaraAdminPolicies policies;

    auto nextToken = [] (const std::string &line, std::size_t &beginToken) -> std::string  {
        auto endToken = line.find(PathConfig::StoragePath::fieldSeparator, beginToken);
        if (endToken != std::string::npos) {
            auto token = line.substr(beginToken, endToken - beginToken);
            beginToken = endToken + 1;
            return token;
        }

        throw BucketRecordCorruptedException(line);
    };

    auto lastToken = [] (const std::string &line, std::size_t &beginToken) -> std::string  {
        if (beginToken < line.size()) {
            auto ret = line.substr(beginToken);
            beginToken = line.size();
            return ret;
        }

        return std::string();
    };

    std::string line;
    std::size_t lineNum = 1;
    while (std::getline(*input, line, PathConfig::StoragePath::recordSeparator)) {
        if (line.empty())
            break;

        try {
            std::size_t beginToken = 0;
            auto bucketId = nextToken(line, beginToken);
            auto policyKey = BucketDeserializer::parseKey(line, beginToken);
            auto policyType = translatePolicy(nextToken(line, beginToken));
            auto metadata = lastToken(line, beginToken);

            policies.add(bucketId, PolicyResult(policyType, metadata), policyKey);
        } catch (const BucketRecordCorruptedException &ex) {
            throw ex.withLineNumber(lineNum);
        }
        lineNum++;
    }

    policies.seal();
    return policies;
}

} /* namespace AdminPolicyParser */

} /* namespace Cynara */
