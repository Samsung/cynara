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
 * @file        src/admin/logic/OfflineLogic.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of OfflineLogic class
 */
#ifdef DB_FILES_SMACK_LABEL
#include <dirent.h>
#include <linux/xattr.h>
#include <sys/smack.h>
#endif

#include <common.h>
#include <config/PathConfig.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DatabaseBusyException.h>
#include <exceptions/DatabaseCorruptedException.h>
#include <exceptions/DatabaseException.h>
#include <exceptions/DefaultBucketDeletionException.h>
#include <exceptions/DefaultBucketSetNoneException.h>
#include <exceptions/InvalidBucketIdException.h>
#include <exceptions/UnknownPolicyTypeException.h>
#include <log/log.h>
#include <plugin/PluginManager.h>
#include <types/PolicyDescription.h>

#include <storage/InMemoryStorageBackend.h>
#include <storage/Storage.h>

#include <cynara-error.h>

#include "OfflineLogic.h"

namespace Cynara {

OfflineLogic::OfflineLogic() {}

void OfflineLogic::acquireDatabase(void) {
    m_storageBackend.reset(new InMemoryStorageBackend(PathConfig::StoragePath::dbDir));
    m_storage.reset(new Storage(*m_storageBackend));
    m_storage->load();
}

void OfflineLogic::acquirePlugins(void) {
    m_pluginManager.reset(new PluginManager(PathConfig::PluginPath::serviceDir));
    m_pluginManager->loadPlugins();
}

void OfflineLogic::checkPoliciesTypes(const ApiInterface::PoliciesByBucket &policies,
                                      bool allowBucket, bool allowNone) {
    for (const auto &group : policies) {
        for (const auto &policy : group.second) {
            checkSinglePolicyType(policy.result().policyType(), allowBucket, allowNone);
        }
    }
}

void OfflineLogic::checkSinglePolicyType(const PolicyType &policyType, bool allowBucket,
                                         bool allowNone) {
    if (allowBucket && policyType == PredefinedPolicyType::BUCKET)
        return;
    if (allowNone && policyType == PredefinedPolicyType::NONE)
        return;
    for (const auto &descr : predefinedPolicyDescr) {
        if (descr.type == policyType)
            return;
    }
    m_pluginManager->checkPolicyType(policyType);
}

int OfflineLogic::setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                              const ApiInterface::KeysByBucket &remove) {
    try {
        acquireDatabase();
        acquirePlugins();
        checkPoliciesTypes(insertOrUpdate, true, false);
        m_storage->insertPolicies(insertOrUpdate);
        m_storage->deletePolicies(remove);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    } catch (const DatabaseException &) {
        return CYNARA_API_OPERATION_FAILED;
    } catch (const DatabaseCorruptedException &) {
        return CYNARA_API_DATABASE_CORRUPTED;
    } catch (const UnknownPolicyTypeException &ex) {
        return CYNARA_API_INVALID_PARAM;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                       const PolicyResult &policyResult) {
    try {
        acquireDatabase();
        acquirePlugins();
        checkSinglePolicyType(policyResult.policyType(), true, true);
        m_storage->addOrUpdateBucket(bucket, policyResult);
        onPoliciesChanged();
    } catch (const DefaultBucketSetNoneException &) {
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    } catch (const InvalidBucketIdException &ex) {
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    } catch (const DatabaseException &) {
        return CYNARA_API_OPERATION_FAILED;
    } catch (const DatabaseCorruptedException &) {
        return CYNARA_API_DATABASE_CORRUPTED;
    } catch (const UnknownPolicyTypeException &ex) {
        return CYNARA_API_INVALID_PARAM;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::removeBucket(const PolicyBucketId &bucket) {
    try {
        acquireDatabase();
        m_storage->deleteBucket(bucket);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    } catch (const DefaultBucketDeletionException &) {
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    } catch (const DatabaseException &) {
        return CYNARA_API_OPERATION_FAILED;
    } catch (const DatabaseCorruptedException &) {
        return CYNARA_API_DATABASE_CORRUPTED;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::adminCheck(const PolicyBucketId &startBucket, bool recursive,
                             const PolicyKey &key, PolicyResult &result) {
    try {
        acquireDatabase();
        result = m_storage->checkPolicy(key, startBucket, recursive);
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    } catch (const DatabaseCorruptedException &) {
        return CYNARA_API_DATABASE_CORRUPTED;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::listDescriptions(std::vector<PolicyDescription> &descriptions) {
    acquirePlugins();
    descriptions = m_pluginManager->getPolicyDescriptions();
    descriptions.insert(descriptions.begin(), predefinedPolicyDescr.begin(),
                        predefinedPolicyDescr.end());
    return CYNARA_API_SUCCESS;
}

int OfflineLogic::listPolicies(const PolicyBucketId &bucket, const PolicyKey &filter,
                               std::vector<Policy> &policies) {
    try {
        acquireDatabase();
        policies = m_storage->listPolicies(bucket, filter);
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    } catch (const DatabaseCorruptedException &) {
        return CYNARA_API_DATABASE_CORRUPTED;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::erasePolicies(const PolicyBucketId &startBucket, bool recursive,
                                const PolicyKey &filter) {
    try {
        acquireDatabase();
        m_storage->erasePolicies(startBucket, recursive, filter);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    } catch (const DatabaseCorruptedException &) {
        return CYNARA_API_DATABASE_CORRUPTED;
    }

    return CYNARA_API_SUCCESS;
}


void OfflineLogic::labelDatabaseFiles(void)
{
#ifdef DB_FILES_SMACK_LABEL
   DIR           *dbDirectory;
   struct dirent *directoryEntry;

   dbDirectory = opendir(PathConfig::StoragePath::dbDir.c_str());
   if (dbDirectory) {
       while ((directoryEntry = readdir(dbDirectory)) != NULL) {
           std::string f = PathConfig::StoragePath::dbDir + directoryEntry->d_name;
           if (smack_set_label_for_path(f.c_str(), XATTR_NAME_SMACK, 1, DB_FILES_SMACK_LABEL) < 0) {
               LOGE("Failed to set label for database file: " << f);
           }
       }
       closedir(dbDirectory);
   }
#endif
}

void OfflineLogic::onPoliciesChanged(void) {
    m_storage->save();
    labelDatabaseFiles();
}

} /* namespace Cynara */
