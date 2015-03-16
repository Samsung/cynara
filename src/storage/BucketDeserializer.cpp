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
 * @file        src/storage/BucketDeserializer.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Methods implementation of Cynara::BucketDeserializer
 */

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <config/PathConfig.h>
#include <exceptions/BucketRecordCorruptedException.h>
#include <types/PolicyCollection.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <storage/StorageDeserializer.h>

#include "BucketDeserializer.h"

namespace Cynara {

PolicyCollection BucketDeserializer::loadPolicies(void) {
    PolicyCollection policies;

    // TODO: Get someone smart to do error checking on stream
    std::string line;
    std::size_t lineNum = 1;
    while(std::getline(*m_inStream, line, PathConfig::StoragePath::recordSeparator)) {
        if (line.empty())
            break;

        try {
            std::size_t beginToken = 0;
            auto policyKey = parseKey(line, beginToken);
            auto policyType = StorageDeserializer::parsePolicyType(line, beginToken);
            auto metadata = StorageDeserializer::parseMetadata(line, beginToken);
            PolicyResult policyResult(policyType, metadata);
            policies.push_back(std::make_shared<Policy>(policyKey, policyResult));
        } catch (const BucketRecordCorruptedException &ex) {
            throw ex.withLineNumber(lineNum);
        }
        ++lineNum;
    }

    return policies;
}

PolicyKey BucketDeserializer::parseKey(const std::string &line, std::size_t &beginToken) {
    std::array<std::string, 3> keyFeatures;

    for (std::size_t tokenNum = 0; tokenNum < keyFeatures.size(); ++tokenNum) {
        auto endToken = line.find(PathConfig::StoragePath::fieldSeparator, beginToken);
        if (endToken != std::string::npos) {
            keyFeatures[tokenNum] = line.substr(beginToken, endToken - beginToken);
            beginToken = endToken + 1;
        } else {
            throw BucketRecordCorruptedException(line);
        }
    }

    return PolicyKey(keyFeatures[0], keyFeatures[1], keyFeatures[2]);
}

} /* namespace Cynara */
