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
 * @file        src/storage/InMemoryStorageBackend.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Headers for InMemoryStorageBackend
 */

#ifndef SRC_STORAGE_INMEMORYSTORAGEBACKEND_H_
#define SRC_STORAGE_INMEMORYSTORAGEBACKEND_H_

#include <fstream>
#include <memory>
#include <string>

#include <types/pointers.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <storage/BucketDeserializer.h>
#include <storage/Buckets.h>
#include <storage/StorageBackend.h>
#include <storage/StorageSerializer.h>

namespace Cynara {

class InMemoryStorageBackend : public StorageBackend {
public:
    InMemoryStorageBackend(const std::string &path) : m_dbPath(path) {
    }
    virtual ~InMemoryStorageBackend() {};

    virtual void load(void);
    virtual void save(void);

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
    InMemoryStorageBackend() {}
    void openFileStream(std::shared_ptr<std::ifstream> stream, const std::string &filename);
    std::shared_ptr<BucketDeserializer> bucketStreamOpener(const PolicyBucketId &bucketId,
                                                           const std::string &fileNameSuffix);

    virtual void openDumpFileStream(std::shared_ptr<std::ofstream> stream,
                                    const std::string &filename);
    std::shared_ptr<StorageSerializer> bucketDumpStreamOpener(const PolicyBucketId &bucketId);

private:
    std::string m_dbPath;
    Buckets m_buckets;
    static const std::string m_indexFilename;
    static const std::string m_backupFilenameSuffix;
    static const std::string m_bucketFilenamePrefix;

protected:
    virtual Buckets &buckets(void) {
        return m_buckets;
    }
};

} /* namespace Cynara */

#endif /* SRC_STORAGE_INMEMORYSTORAGEBACKEND_H_ */
