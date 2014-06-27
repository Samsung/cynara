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
/*
 * @file        StorageDeserializer.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Methods implementation of Cynara::StorageDeserializer
 */

#include <exceptions/BucketRecordCorruptedException.h>
#include <storage/StorageDeserializer.h>
#include <storage/StorageSerializer.h>
#include <types/Policy.h>

#include <iostream>
#include <memory>
#include <string>

namespace Cynara {

PolicyCollection StorageDeserializer::loadPolicies(std::istream &is) {
    PolicyCollection policies;

    // TODO: Get someone smart to do error checking on stream
    for(std::size_t lineNum = 1; !is.eof(); ++lineNum) {
        std::string line;
        std::getline(is, line, StorageSerializer::recordSeparator());

        if (line.empty())
            break;

        try {
            std::size_t beginToken = 0;
            auto policyKey = parseKey(line, beginToken);
            auto policyType = parsePolicyType(line, beginToken);
            auto metadata = parseMetadata(line, beginToken);
            PolicyResult policyResult(policyType, metadata);
            policies.push_back(std::make_shared<Policy>(policyKey, policyResult));
        } catch (const BucketRecordCorruptedException &ex) {
            throw ex.withLineNumber(lineNum);
        }
    }

    return policies;
}

PolicyKey StorageDeserializer::parseKey(const std::string &line, std::size_t &beginToken) {
    std::array<std::string, 3> keyFeatures;

    for(std::size_t tokenNum = 0; tokenNum < keyFeatures.size(); ++tokenNum) {
        auto endToken = line.find(StorageSerializer::fieldSeparator(), beginToken);
        if (endToken != std::string::npos) {
            keyFeatures[tokenNum] = line.substr(beginToken, endToken - beginToken);
            beginToken = endToken + 1;
        } else {
            throw BucketRecordCorruptedException(line);
        }
    }

    return PolicyKey(keyFeatures[0], keyFeatures[1], keyFeatures[2]);
}

PolicyType StorageDeserializer::parsePolicyType(const std::string &line, std::size_t &beginToken) {
    PolicyType policyType;
    try {
        size_t newBegin = 0;
        policyType = std::stoi(line.substr(beginToken), &newBegin, 16);
        beginToken += newBegin;
    } catch(...) {
        throw BucketRecordCorruptedException(line);
    }

    return policyType;
}

PolicyResult::PolicyMetadata StorageDeserializer::parseMetadata(const std::string &line,
                                                                std::size_t &beginToken) {
    if (beginToken < line.size()) {
        auto ret = line.substr(beginToken + 1);
        beginToken = line.size();
        return ret;
    }

    return std::string();
}

} /* namespace Cynara */
