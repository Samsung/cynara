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
 * @file        Storage.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       This file is the implementation storage
 */

#ifndef SRC_STORAGE_STORAGE_H_
#define SRC_STORAGE_STORAGE_H_

#include <map>
#include <string>
#include <vector>

#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <storage/StorageBackend.h>

namespace Cynara {

class Storage
{
public:
    Storage(StorageBackend &backend) : m_backend(backend) {}

    PolicyResult checkPolicy(const PolicyKey &key,
                             const PolicyBucketId &startBucketId = defaultPolicyBucketId,
                             bool recursive = true);

    void insertPolicies(const std::map<PolicyBucketId, std::vector<Policy>> &policiesByBucketId);
    void deletePolicies(const std::map<PolicyBucketId, std::vector<PolicyKey>> &keysByBucketId);

    void addOrUpdateBucket(const PolicyBucketId &bucketId, const PolicyResult &defaultBucketPolicy);
    void deleteBucket(const PolicyBucketId &bucketId);

    void load(void);
    void save(void);

protected:
    PolicyResult minimalPolicy(const PolicyBucket &bucket, const PolicyKey &key, bool recursive);

private:
    StorageBackend &m_backend; // backend strategy
};

} // namespace Cynara

#endif /* SRC_STORAGE_STORAGE_H_ */
