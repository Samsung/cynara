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
 * @file        src/storage/Integrity.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     0.1
 * @brief       Headers for Cynara::Integrity
 */

#ifndef SRC_STORAGE_INTEGRITY_H_
#define SRC_STORAGE_INTEGRITY_H_

#include <fcntl.h>
#include <memory>
#include <string>

#include <storage/Buckets.h>

namespace Cynara {

class Integrity;
typedef std::unique_ptr<Integrity> IntegrityUniquePtr;

class Integrity
{
public:
    typedef std::function<bool(const PolicyBucketId &)> BucketPresenceTester;
    Integrity(const std::string &path) : m_dbPath(path) {}
    virtual ~Integrity() {};

    virtual bool backupGuardExists(void) const;
    virtual void createBackupGuard(void) const;
    virtual void syncDatabase(const Buckets &buckets, bool syncBackup);
    virtual void revalidatePrimaryDatabase(const Buckets &buckets);
    virtual void deleteNonIndexedFiles(BucketPresenceTester tester);

protected:
    static void syncElement(const std::string &filename, int flags = O_RDONLY,
                            mode_t mode = S_IRUSR | S_IWUSR);
    static void syncDirectory(const std::string &dirname, mode_t mode = S_IRUSR | S_IWUSR);

    void createPrimaryHardLinks(const Buckets &buckets);
    void deleteBackupHardLinks(const Buckets &buckets);

    static void createHardLink(const std::string &oldName, const std::string &newName);
    static void deleteHardLink(const std::string &filename);

private:
    const std::string m_dbPath;
    static const std::string m_indexFilename;
    static const std::string m_backupFilenameSuffix;
    static const std::string m_bucketFilenamePrefix;
    static const std::string m_guardFilename;
};

} // namespace Cynara

#endif /* SRC_STORAGE_INTEGRITY_H_ */
