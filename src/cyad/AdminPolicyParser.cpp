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
 * @file        src/cyad/AdminPolicyParser.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Parses policies from input stream
 */

#include <exceptions/BucketRecordCorruptedException.h>
#include <storage/StorageDeserializer.h>
#include <storage/StorageSerializer.h>

#include "AdminPolicyParser.h"

namespace Cynara {

namespace AdminPolicyParser {

CynaraAdminPolicies parse(const std::shared_ptr<std::istream> &input) {
    CynaraAdminPolicies policies;

    for (std::size_t lineNum = 1; !input->eof(); ++lineNum) {
        std::string line;
        std::getline(*input, line, StorageSerializer::recordSeparator());

        if (line.empty())
            break;

        try {
            std::size_t beginToken = 0;
            auto bucketId = StorageDeserializer::parseBucketId(line, beginToken);
            auto policyKey = BucketDeserializer::parseKey(line, beginToken);
            auto policyType = StorageDeserializer::parsePolicyType(line, beginToken);
            auto metadata = StorageDeserializer::parseMetadata(line, beginToken);

            policies.add(bucketId, PolicyResult(policyType, metadata), policyKey);
        } catch (const BucketRecordCorruptedException &ex) {
            throw ex.withLineNumber(lineNum);
        }
    }

    policies.seal();
    return policies;
}

} /* namespace AdminPolicyParser */

} /* namespace Cynara */
