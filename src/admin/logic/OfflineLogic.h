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
 * @file        src/admin/logic/OfflineLogic.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of OfflineLogic class
 */

#ifndef SRC_ADMIN_LOGIC_OFFLINELOGIC_H_
#define SRC_ADMIN_LOGIC_OFFLINELOGIC_H_

#include <memory>

#include <lock/FileLock.h>
#include <plugin/PluginManager.h>
#include <types/PolicyType.h>

#include <storage/Storage.h>
#include <storage/StorageBackend.h>

#include <api/ApiInterface.h>

namespace Cynara {

class OfflineLogic : public ApiInterface {
public:
    OfflineLogic();

    int setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                    const ApiInterface::KeysByBucket &remove);
    int insertOrUpdateBucket(const PolicyBucketId &bucket,
                             const PolicyResult &policyResult);
    int removeBucket(const PolicyBucketId &bucket);
    int adminCheck(const PolicyBucketId &startBucket, bool recursive,
                   const PolicyKey &key, PolicyResult &result);
    int listDescriptions(std::vector<PolicyDescription> &descriptions);
    int listPolicies(const PolicyBucketId &bucket, const PolicyKey &filter,
                     std::vector<Policy> &policies);
    int erasePolicies(const PolicyBucketId &startBucket, bool recursive,
                      const PolicyKey &filter);

protected:
    void acquireDatabase(void);
    void acquirePlugins(void);
    void onPoliciesChanged(void);
    void checkPoliciesTypes(const ApiInterface::PoliciesByBucket &policies, bool allowBucket,
                            bool allowNone);
    void checkSinglePolicyType(const PolicyType &policyType, bool allowBucket, bool allowNone);

private:
    typedef std::unique_ptr<Storage> StorageUniquePtr;
    typedef std::unique_ptr<StorageBackend> StorageBackendUniquePtr;
    typedef std::unique_ptr<PluginManager> PluginManagerUniquePtr;

    StorageUniquePtr m_storage;
    StorageBackendUniquePtr m_storageBackend;
    PluginManagerUniquePtr m_pluginManager;
};

} /* namespace Cynara */

#endif /* SRC_ADMIN_LOGIC_OFFLINELOGIC_H_ */
