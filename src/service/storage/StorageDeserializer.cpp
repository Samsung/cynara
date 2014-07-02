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
 * @brief       Implementation for Cynara::StorageDeserializer
 */

#include <service/storage/StorageDeserializer.h>

#include <exceptions/BucketRecordCorruptedException.h>
#include <service/storage/BucketDeserializer.h>
#include <service/storage/StorageSerializer.h>

#include <iostream>
#include <string>

namespace Cynara {

StorageDeserializer::StorageDeserializer(std::istream &inStream,
                                         BucketStreamOpener bucketStreamOpener)
        : m_inStream(inStream), m_bucketStreamOpener(bucketStreamOpener) {}

void StorageDeserializer::initBuckets(InMemoryStorageBackend::Buckets &buckets) {
    buckets.clear();

    for(std::size_t lineNum = 1; !m_inStream.eof(); ++lineNum) {
        std::string line;
        std::getline(m_inStream, line, StorageSerializer::recordSeparator());

        if (line.empty())
            break;

        size_t beginToken = 0;
        auto bucketId = StorageDeserializer::parseBucketId(line, beginToken);
        auto policyType = BucketDeserializer::parsePolicyType(line, beginToken);
        auto metadata = BucketDeserializer::parseMetadata(line, beginToken);

        buckets[bucketId] = PolicyBucket(bucketId, PolicyResult(policyType, metadata));
    }
}

void StorageDeserializer::loadBuckets(InMemoryStorageBackend::Buckets &buckets) {
    for (auto &bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        auto &bucket = bucketIter.second;

        auto bucketDeserializer = m_bucketStreamOpener(bucketId);
        if (bucketDeserializer != nullptr) {
            bucket.setPolicyCollection(bucketDeserializer->loadPolicies());
        } else {
            // TODO: Throw?
        }
    }
}

PolicyBucketId StorageDeserializer::parseBucketId(const std::string &line,
                                                    std::size_t &beginToken) {
    auto bucketNameEndToken = line.find(StorageSerializer::fieldSeparator(), beginToken);
    if (bucketNameEndToken != std::string::npos) {
        auto bucketName = line.substr(beginToken, bucketNameEndToken - beginToken);
        beginToken = bucketNameEndToken + 1;
        return bucketName;
    }

    // TODO: Should throw other exception
    throw BucketRecordCorruptedException(line);
}

} /* namespace Cynara */
