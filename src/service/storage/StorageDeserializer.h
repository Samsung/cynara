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
 * @file        StorageDeserializer.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Headers for Cynara::StorageDeserializer
 */
#ifndef SRC_SERVICE_STORAGE_STORAGEDESERIALIZER_H_
#define SRC_SERVICE_STORAGE_STORAGEDESERIALIZER_H_

#include <storage/BucketDeserializer.h>
#include <storage/InMemoryStorageBackend.h>

#include <istream>
#include <memory>
#include <functional>

namespace Cynara {

class StorageDeserializer {
public:
    typedef std::function<std::shared_ptr<BucketDeserializer>(const std::string &)> BucketStreamOpener;
    StorageDeserializer(std::istream &inStream, BucketStreamOpener m_bucketStreamOpener);
    void initBuckets(InMemoryStorageBackend::Buckets &buckets);
    void loadBuckets(InMemoryStorageBackend::Buckets &buckets);

    static PolicyBucketId parseBucketId(const std::string &line, std::size_t &beginToken);
    static PolicyType parsePolicyType(const std::string &line, std::size_t &beginToken);
    static PolicyResult::PolicyMetadata parseMetadata(const std::string &line,
                                                      std::size_t &beginToken);

private:
    std::istream &m_inStream;
    BucketStreamOpener m_bucketStreamOpener;
};

} /* namespace Cynara */

#endif /* SRC_SERVICE_STORAGE_STORAGEDESERIALIZER_H_ */
