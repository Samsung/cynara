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
 * @file        InMemoryStorageBackend.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Headers for InMemoryStorageBackend
 */

#ifndef INMEMORYSTORAGEBACKEND_H_
#define INMEMORYSTORAGEBACKEND_H_

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

#include <exceptions/BucketNotExistsException.h>
#include <exceptions/NotImplementedException.h>
#include <storage/BucketDeserializer.h>
#include <types/Policy.h>

#include "StorageBackend.h"

namespace Cynara {

class InMemoryStorageBackend : public StorageBackend {
public:
    typedef std::unordered_map<PolicyBucketId, PolicyBucket> Buckets;

    InMemoryStorageBackend(const std::string &path) : m_dbPath(path) {
    }

    virtual ~InMemoryStorageBackend() = default;

    virtual void load(void);

    virtual PolicyBucket searchDefaultBucket(const PolicyKey &key);
    virtual PolicyBucket searchBucket(const PolicyBucketId &bucketId, const PolicyKey &key);
    virtual void insertPolicy(const PolicyBucketId &bucketId, PolicyPtr policy);
    virtual void createBucket(const PolicyBucketId &bucketId, const PolicyResult &defaultPolicy);
    virtual void updateBucket(const PolicyBucketId &bucketId, const PolicyResult &defaultPolicy);
    virtual void deleteBucket(const PolicyBucketId &bucketId);
    virtual bool hasBucket(const PolicyBucketId &bucketId);
    virtual void deletePolicy(const PolicyBucketId &bucketId, const PolicyKey &key);
    virtual void deleteLinking(const PolicyBucketId &bucketId);

protected:
    void openFileStream(std::ifstream &stream, const std::string &filename);
    std::shared_ptr<BucketDeserializer> bucketStreamOpener(const PolicyBucketId &bucketId);

private:
    std::string m_dbPath;
    Buckets m_buckets;

protected:
    virtual Buckets &buckets() {
        return m_buckets;
    }
};

} /* namespace Cynara */
#endif /* INMEMORYSTORAGEBACKEND_H_ */
