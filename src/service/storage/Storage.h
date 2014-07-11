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
 * @brief       This file is the implementation file of log system
 */

#ifndef CYNARA_SERVICE_STORAGE_STORAGE_H
#define CYNARA_SERVICE_STORAGE_STORAGE_H

#include "types/pointers.h"
#include "types/PolicyBucketId.h"
#include "types/PolicyResult.h"
#include "types/PolicyKey.h"

#include <memory>
#include <vector>
#include <tuple>

namespace Cynara {

class StorageBackend;
class PolicyBucket;

class Storage
{
public:
    // TODO: These tuples are ugly -- refactorize
    typedef std::tuple<PolicyPtr, PolicyBucketId> PolicyPolicyBucket;
    typedef std::tuple<PolicyKey, PolicyBucketId> PolicyKeyBucket;

    Storage(StorageBackend &backend) : m_backend(backend) {}

    PolicyResult checkPolicy(const PolicyKey &key);

    void insertPolicies(const std::vector<PolicyPolicyBucket> &policies);
    void addOrUpdateBucket(const PolicyBucketId &bucketId, const PolicyResult &defaultBucketPolicy);
    void deletePolicies(const std::vector<PolicyKeyBucket> &policies);
    void deleteBucket(const PolicyBucketId &bucketId);

protected:
    PolicyResult minimalPolicy(const PolicyBucket &bucket, const PolicyKey &key);

private:
    StorageBackend &m_backend; // backend strategy
};

} // namespace Cynara

#endif /* CYNARA_SERVICE_STORAGE_STORAGE_H */
