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
 * @file        src/storage/InMemoryStorageBackend.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of InMemoryStorageBackend
 */

#include <errno.h>
#include <fstream>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>

#include <log/log.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/CannotCreateFileException.h>
#include <exceptions/DatabaseException.h>
#include <exceptions/FileNotFoundException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <types/PolicyBucket.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <storage/BucketDeserializer.h>
#include <storage/Integrity.h>
#include <storage/StorageDeserializer.h>
#include <storage/StorageSerializer.h>

#include "InMemoryStorageBackend.h"

namespace Cynara {

const std::string InMemoryStorageBackend::m_indexFilename = "buckets";
const std::string InMemoryStorageBackend::m_backupFilenameSuffix = "~";
const std::string InMemoryStorageBackend::m_bucketFilenamePrefix = "_";

void InMemoryStorageBackend::load(void) {
    Integrity integrity(m_dbPath, m_indexFilename, m_backupFilenameSuffix, m_bucketFilenamePrefix);
    bool isBackupValid = integrity.backupGuardExists();
    std::string bucketSuffix = "";
    std::string indexFilename = m_dbPath + m_indexFilename;

    if (isBackupValid) {
        bucketSuffix += m_backupFilenameSuffix;
        indexFilename += m_backupFilenameSuffix;
    }

    try {
        auto indexStream = std::make_shared<std::ifstream>();
        openFileStream(indexStream, indexFilename);

        StorageDeserializer storageDeserializer(indexStream,
            std::bind(&InMemoryStorageBackend::bucketStreamOpener, this,
                      std::placeholders::_1, bucketSuffix));

        storageDeserializer.initBuckets(buckets());
        storageDeserializer.loadBuckets(buckets());
    } catch (const DatabaseException &) {
        LOGC("Reading cynara database failed.");
        buckets().clear();
        // TODO: Implement emergency mode toggle
    }

    if (!hasBucket(defaultPolicyBucketId)) {
        LOGN("Creating defaultBucket.");
        this->buckets().insert({ defaultPolicyBucketId, PolicyBucket(defaultPolicyBucketId) });
    }

    if (isBackupValid) {
        integrity.revalidatePrimaryDatabase(buckets());
    }
    //in case there were unnecessary files in db directory
    integrity.deleteNonIndexedFiles(std::bind(&InMemoryStorageBackend::hasBucket, this,
                                    std::placeholders::_1));
}

void InMemoryStorageBackend::save(void) {

    //create directory
    int ret = mkdir(m_dbPath.c_str(), S_IRWXU);
    if (ret < 0) {
        int err = errno;
        if (err != EEXIST) {
            LOGE("Cannot create directory <%s>. Error [%d] : <%s>.",
                 m_dbPath.c_str(), err, strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        }
    }

    auto indexStream = std::make_shared<std::ofstream>();
    std::string indexFilename = m_dbPath + m_indexFilename;
    openDumpFileStream(indexStream, indexFilename + m_backupFilenameSuffix);

    StorageSerializer storageSerializer(indexStream);
    storageSerializer.dump(buckets(), std::bind(&InMemoryStorageBackend::bucketDumpStreamOpener,
                           this, std::placeholders::_1));

    Integrity integrity(m_dbPath, m_indexFilename, m_backupFilenameSuffix, m_bucketFilenamePrefix);

    integrity.syncDatabase(buckets(), true);
    integrity.createBackupGuard();
    integrity.revalidatePrimaryDatabase(buckets());
    //guard is removed during revalidation
}

PolicyBucket InMemoryStorageBackend::searchDefaultBucket(const PolicyKey &key) {
    return searchBucket(defaultPolicyBucketId, key);
}

PolicyBucket InMemoryStorageBackend::searchBucket(const PolicyBucketId &bucketId,
                                                  const PolicyKey &key) {
    try {
        const auto &bucket = this->buckets().at(bucketId);
        return bucket.filtered(key);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::insertPolicy(const PolicyBucketId &bucketId, PolicyPtr policy) {
    try {
        auto &bucket = buckets().at(bucketId);
        bucket.insertPolicy(policy);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::createBucket(const PolicyBucketId &bucketId,
                                          const PolicyResult &defaultPolicy) {
    PolicyBucket newBucket(bucketId, defaultPolicy);
    buckets().insert({ bucketId, newBucket });
}

void InMemoryStorageBackend::updateBucket(const PolicyBucketId &bucketId,
                                          const PolicyResult &defaultPolicy) {
    try {
        auto &bucket = buckets().at(bucketId);
        bucket.setDefaultPolicy(defaultPolicy);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::deleteBucket(const PolicyBucketId &bucketId) {
    auto bucketErased = buckets().erase(bucketId);
    if (bucketErased == 0) {
        throw BucketNotExistsException(bucketId);
    }
}

bool InMemoryStorageBackend::hasBucket(const PolicyBucketId &bucketId) {
    return buckets().find(bucketId) != buckets().end();
}

void InMemoryStorageBackend::deletePolicy(const PolicyBucketId &bucketId, const PolicyKey &key) {
    try {
        // TODO: Move the erase code to PolicyCollection maybe?
        auto &bucket = buckets().at(bucketId);
        bucket.deletePolicy(key);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::deleteLinking(const PolicyBucketId &bucketId) {
    auto bucketIdMatches = [&bucketId] (PolicyPtr policy) -> bool {
        auto policyResult = policy->result();

        // Check bucket id only if policy is a bucket policy
        // TODO: Maybe move the test to PolicyResult
        if (policyResult.policyType() == PredefinedPolicyType::BUCKET) {
            return policyResult.metadata() == bucketId;
        }
        return false;
    };

    for (auto &bucketIter : buckets()) {
        auto &bucket = bucketIter.second;
        bucket.deletePolicy(bucketIdMatches);
    }
}

PolicyBucket::Policies InMemoryStorageBackend::listPolicies(const PolicyBucketId &bucketId,
                                                            const PolicyKey &filter) const {
    try {
        auto &bucket = buckets().at(bucketId);
        return bucket.listPolicies(filter);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::openFileStream(std::shared_ptr<std::ifstream> stream,
                                            const std::string &filename) {
    // TODO: Consider adding exceptions to streams and handling them:
    // stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    stream->open(filename);

    if (!stream->is_open()) {
        throw FileNotFoundException(filename);
    }
}

void InMemoryStorageBackend::openDumpFileStream(std::shared_ptr<std::ofstream> stream,
                                                const std::string &filename) {
    stream->open(filename, std::ofstream::out | std::ofstream::trunc);

    if (!stream->is_open()) {
        throw CannotCreateFileException(filename);
    }
}

std::shared_ptr<BucketDeserializer> InMemoryStorageBackend::bucketStreamOpener(
        const PolicyBucketId &bucketId, const std::string &filenameSuffix) {
    std::string bucketFilename = m_dbPath + m_bucketFilenamePrefix + bucketId + filenameSuffix;
    auto bucketStream = std::make_shared<std::ifstream>();
    try {
        openFileStream(bucketStream, bucketFilename);
        return std::make_shared<BucketDeserializer>(bucketStream);
    } catch (const FileNotFoundException &) {
        return nullptr;
    }
}

std::shared_ptr<StorageSerializer> InMemoryStorageBackend::bucketDumpStreamOpener(
        const PolicyBucketId &bucketId) {
    std::string bucketFilename = m_dbPath + m_bucketFilenamePrefix +
                                 bucketId + m_backupFilenameSuffix;
    auto bucketStream = std::make_shared<std::ofstream>();

    openDumpFileStream(bucketStream, bucketFilename);
    return std::make_shared<StorageSerializer>(bucketStream);
}

} /* namespace Cynara */
