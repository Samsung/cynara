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
 * @file        src/storage/InMemoryStorageBackend.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Implementation of InMemoryStorageBackend
 */

#include <errno.h>
#include <fstream>
#include <functional>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>

#include <log/log.h>
#include <config/PathConfig.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DatabaseCorruptedException.h>
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

const std::string InMemoryStorageBackend::m_chsFilename(PathConfig::StoragePath::checksumFilename);
const std::string InMemoryStorageBackend::m_indexFilename(PathConfig::StoragePath::indexFilename);
const std::string InMemoryStorageBackend::m_backupFilenameSuffix(
        PathConfig::StoragePath::backupFilenameSuffix);
const std::string InMemoryStorageBackend::m_bucketFilenamePrefix(
        PathConfig::StoragePath::bucketFilenamePrefix);

InMemoryStorageBackend::InMemoryStorageBackend(const std::string &path) : m_dbPath(path),
    m_checksum(path), m_integrity(path) {
}

void InMemoryStorageBackend::load(void) {
    bool isBackupValid = m_integrity.backupGuardExists();
    std::string bucketSuffix = "";
    std::string indexFilename = m_dbPath + m_indexFilename;
    std::string chsFilename = m_dbPath + m_chsFilename;

    if (isBackupValid) {
        bucketSuffix += m_backupFilenameSuffix;
        indexFilename += m_backupFilenameSuffix;
        chsFilename += m_backupFilenameSuffix;
    }

    try {
        std::ifstream chsStream;
        openFileStream(chsStream, chsFilename, isBackupValid);
        m_checksum.load(chsStream);

        auto indexStream = std::make_shared<std::ifstream>();
        openFileStream(*indexStream, indexFilename, isBackupValid);

        StorageDeserializer storageDeserializer(indexStream,
            std::bind(&InMemoryStorageBackend::bucketStreamOpener, this,
                      std::placeholders::_1, bucketSuffix, isBackupValid));

        storageDeserializer.initBuckets(buckets());
        storageDeserializer.loadBuckets(buckets());
    } catch (const DatabaseException &) {
        LOGC("Reading cynara database failed.");
        buckets().clear();
        throw DatabaseCorruptedException();
    }
    m_checksum.clear();

    if (!hasBucket(defaultPolicyBucketId)) {
        LOGN("Creating defaultBucket.");
        this->buckets().insert({ defaultPolicyBucketId, PolicyBucket(defaultPolicyBucketId) });
    }

    postLoadCleanup(isBackupValid);
}

void InMemoryStorageBackend::save(void) {
    std::string checksumFilename = m_dbPath + m_chsFilename;
    auto chsStream = std::make_shared<std::ofstream>();
    openDumpFileStream<std::ofstream>(*chsStream, checksumFilename + m_backupFilenameSuffix);

    dumpDatabase(chsStream);

    m_integrity.syncDatabase(buckets(), true);
    m_integrity.createBackupGuard();
    m_integrity.revalidatePrimaryDatabase(buckets());
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

void InMemoryStorageBackend::erasePolicies(const PolicyBucketId &bucketId, bool recursive,
                                           const PolicyKey &filter) {
    PolicyBucket::BucketIds bucketIds = {bucketId};

    while (!bucketIds.empty()) {
        auto it = bucketIds.begin();
        PolicyBucketId bucketId = *it;
        bucketIds.erase(it);
        try {
            auto &policyBucket = buckets().at(bucketId);
            if (recursive) {
                auto subBuckets = policyBucket.getSubBuckets();
                bucketIds.insert(subBuckets.begin(), subBuckets.end());
            }
            policyBucket.deletePolicy([&filter] (PolicyPtr policy) -> bool {
                 return policy->key().matchFilter(filter);
            });
        } catch (const std::out_of_range &) {
            throw BucketNotExistsException(bucketId);
        }
    }
}

void InMemoryStorageBackend::dumpDatabase(const std::shared_ptr<std::ofstream> &chsStream) {
    auto indexStream = std::make_shared<ChecksumStream>(m_indexFilename, chsStream);
    std::string indexFilename = m_dbPath + m_indexFilename;
    openDumpFileStream<ChecksumStream>(*indexStream, indexFilename + m_backupFilenameSuffix);

    StorageSerializer<ChecksumStream> storageSerializer(indexStream);
    storageSerializer.dump(buckets(), std::bind(&InMemoryStorageBackend::bucketDumpStreamOpener,
                           this, std::placeholders::_1, chsStream));
}

void InMemoryStorageBackend::openFileStream(std::ifstream &stream, const std::string &filename,
                                            bool isBackupValid) {
    // TODO: Consider adding exceptions to streams and handling them:
    // stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    stream.open(filename);

    if (!stream.is_open()) {
        throw FileNotFoundException(filename);
    }

    m_checksum.compare(stream, filename, isBackupValid);
}

std::shared_ptr<BucketDeserializer> InMemoryStorageBackend::bucketStreamOpener(
        const PolicyBucketId &bucketId, const std::string &filenameSuffix, bool isBackupValid) {
    std::string bucketFilename = m_dbPath + m_bucketFilenamePrefix + bucketId + filenameSuffix;
    auto bucketStream = std::make_shared<std::ifstream>();
    try {
        openFileStream(*bucketStream, bucketFilename, isBackupValid);
        return std::make_shared<BucketDeserializer>(bucketStream);
    } catch (const FileNotFoundException &) {
        return nullptr;
    } catch (const std::bad_alloc &) {
        return nullptr;
    }
}

std::shared_ptr<StorageSerializer<ChecksumStream> > InMemoryStorageBackend::bucketDumpStreamOpener(
        const PolicyBucketId &bucketId, const std::shared_ptr<std::ofstream> &chsStream) {
    std::string bucketFilename = m_dbPath + m_bucketFilenamePrefix +
                                 bucketId + m_backupFilenameSuffix;
    auto bucketStream = std::make_shared<ChecksumStream>(m_bucketFilenamePrefix + bucketId,
                                                         chsStream);

    openDumpFileStream<ChecksumStream>(*bucketStream, bucketFilename);
    return std::make_shared<StorageSerializer<ChecksumStream> >(bucketStream);
}

void InMemoryStorageBackend::postLoadCleanup(bool isBackupValid) {
    if (isBackupValid) {
        m_integrity.revalidatePrimaryDatabase(buckets());
    }
    //in case there were unnecessary files in db directory
    m_integrity.deleteNonIndexedFiles(std::bind(&InMemoryStorageBackend::hasBucket, this,
                                      std::placeholders::_1));
}

} /* namespace Cynara */
