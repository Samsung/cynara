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
 * @file        src/storage/Integrity.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     0.1
 * @brief       Implementation of Cynara::Integrity
 */

#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <functional>
#include <memory>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <config/PathConfig.h>
#include <exceptions/CannotCreateFileException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>

#include "Integrity.h"

namespace Cynara {

namespace StorageConfig = PathConfig::StoragePath;

bool Integrity::backupGuardExists(void) const {
    struct stat buffer;
    std::string guardFilename = m_dbPath + StorageConfig::guardFilename;

    int ret = stat(guardFilename.c_str(), &buffer);

    if (ret == 0) {
        return true;
    } else {
        int err = errno;
        if (err != ENOENT) {
            LOGE("'stat' function error [%d] : <%s>", err, strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        }
        return false;
    }
}

void Integrity::createBackupGuard(void) const {
    syncElement(m_dbPath + StorageConfig::guardFilename, O_CREAT | O_EXCL | O_WRONLY | O_TRUNC);
    syncDirectory(m_dbPath);
}

void Integrity::syncDatabase(const Buckets &buckets, bool syncBackup) {
    std::string suffix = "";

    if (syncBackup) {
        suffix += StorageConfig::backupFilenameSuffix;
    }

    for (const auto &bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        const auto &bucketFilename = m_dbPath + StorageConfig::bucketFilenamePrefix +
                bucketId + suffix;

        syncElement(bucketFilename);
    }

    syncElement(m_dbPath + StorageConfig::indexFilename + suffix);
    syncElement(m_dbPath + PathConfig::StoragePath::checksumFilename + suffix);
    syncDirectory(m_dbPath);
}

void Integrity::revalidatePrimaryDatabase(const Buckets &buckets) {
    createPrimaryHardLinks(buckets);
    syncDatabase(buckets, false);

    deleteHardLink(m_dbPath + StorageConfig::guardFilename);
    syncDirectory(m_dbPath);

    deleteBackupHardLinks(buckets);
}

void Integrity::deleteNonIndexedFiles(BucketPresenceTester tester) {
    DIR *dirPtr = nullptr;
    struct dirent *direntPtr;

    if ((dirPtr = opendir(m_dbPath.c_str())) == nullptr) {
        int err = errno;
        LOGE("'opendir' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
        return;
    }

    std::unique_ptr<DIR, std::function<void(DIR*)>> dirStream(dirPtr,
            [](DIR *dir) {
                if (closedir(dir) < 0) {
                    int err = errno;
                    (void) err;
                    LOGE("'closedir' function error [%d] : <%s>", err, strerror(err));
                }
            });

    while (errno = 0, (direntPtr = readdir(dirPtr)) != nullptr) {
        std::string filename = direntPtr->d_name;
        //ignore all special files (working dir, parent dir, index, checksums)
        if (isSpecialDirectory(filename) || isSpecialDatabaseEntry(filename)) {
            continue;
        }

        std::string bucketId;
        auto nameLength = filename.length();
        auto prefixLength = StorageConfig::bucketFilenamePrefix.length();

        //remove if it is impossible that it is a bucket file
        if (nameLength < prefixLength) {
            deleteHardLink(m_dbPath + filename);
            continue;
        }

        //remove if there is no bucket filename prefix
        //0 is returned from string::compare() if strings are equal
        if (0 != filename.compare(0, prefixLength, StorageConfig::bucketFilenamePrefix)) {
            deleteHardLink(m_dbPath + filename);
            continue;
        }

        //remove if bucket is not in index
        bucketId = filename.substr(prefixLength);
        if (!tester(bucketId)) {
            deleteHardLink(m_dbPath + filename);
        }
    }

    if (errno) {
        int err = errno;
        LOGE("'readdir' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
        return;
    }
}

void Integrity::syncElement(const std::string &filename, int flags, mode_t mode) {
    int fileFd = TEMP_FAILURE_RETRY(open(filename.c_str(), flags, mode));

    if (fileFd < 0) {
        int err = errno;
        if (err != EEXIST) {
            LOGE("File <%s> : 'open' function error [%d] : <%s>", filename.c_str(), err,
                 strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        } else {
            throw CannotCreateFileException(filename);
        }
    }

    int ret = fsync(fileFd);

    if (ret < 0) {
        int err = errno;
        LOGE("'fsync' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }

    ret = close(fileFd);

    if (ret < 0) {
        int err = errno;
        LOGE("'close' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }
}

// from: man 2 fsync
// Calling fsync() does not necessarily ensure that the entry in the directory containing
// the file has also reached disk.  For that an explicit fsync() on a file descriptor for
// the directory is also needed.
void Integrity::syncDirectory(const std::string &dirname, mode_t mode) {
    syncElement(dirname, O_DIRECTORY, mode);
}

void Integrity::createPrimaryHardLinks(const Buckets &buckets) {
    for (const auto &bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        const auto &bucketFilename = m_dbPath + StorageConfig::bucketFilenamePrefix + bucketId;

        deleteHardLink(bucketFilename);
        createHardLink(bucketFilename + StorageConfig::backupFilenameSuffix, bucketFilename);
    }

    const auto &indexFilename = m_dbPath + StorageConfig::indexFilename;
    const auto &checksumFilename = m_dbPath + PathConfig::StoragePath::checksumFilename;

    deleteHardLink(indexFilename);
    createHardLink(indexFilename + StorageConfig::backupFilenameSuffix, indexFilename);
    deleteHardLink(checksumFilename);
    createHardLink(checksumFilename + StorageConfig::backupFilenameSuffix, checksumFilename);
}

void Integrity::deleteBackupHardLinks(const Buckets &buckets) {
    for (const auto &bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        const auto &bucketFilename = m_dbPath + StorageConfig::bucketFilenamePrefix +
                                     bucketId + StorageConfig::backupFilenameSuffix;

        deleteHardLink(bucketFilename);
    }

    deleteHardLink(m_dbPath + StorageConfig::indexFilename + StorageConfig::backupFilenameSuffix);
    deleteHardLink(m_dbPath + StorageConfig::checksumFilename +
            StorageConfig::backupFilenameSuffix);
}

void Integrity::createHardLink(const std::string &oldName, const std::string &newName) {
    int ret = link(oldName.c_str(), newName.c_str());

    if (ret < 0) {
        int err = errno;
        throw UnexpectedErrorException(err, strerror(err));
        LOGN("Trying to link to non-existent file: <%s>", oldName.c_str());
    }
}

void Integrity::deleteHardLink(const std::string &filename) {
    int ret = unlink(filename.c_str());

    if (ret < 0) {
        int err = errno;
        if (err != ENOENT) {
            LOGE("'unlink' function error [%d] : <%s>", err, strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        } else {
            LOGN("Trying to unlink non-existent file: <%s>", filename.c_str());
        }
    }
}

bool Integrity::isSpecialDirectory(const std::string &filename) {
    return "." == filename || ".." == filename;
}

bool Integrity::isSpecialDatabaseEntry(const std::string &filename) {
    return PathConfig::StoragePath::indexFilename == filename ||
           PathConfig::StoragePath::checksumFilename == filename;
}

} /* namespace Cynara */
