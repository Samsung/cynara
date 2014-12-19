/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/admin/api/ApiInterface.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains libcynara-admin API interface definition.
 */

#ifndef SRC_ADMIN_API_APIINTERFACE_H_
#define SRC_ADMIN_API_APIINTERFACE_H_

#include <map>
#include <string>
#include <vector>

#include <types/Policy.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyDescription.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <cynara-admin.h>

namespace Cynara {

class ApiInterface {
public:

    typedef std::map<PolicyBucketId, std::vector<Policy>> PoliciesByBucket;
    typedef std::map<PolicyBucketId, std::vector<PolicyKey>> KeysByBucket;

    ApiInterface() = default;
    virtual ~ApiInterface() {};

    virtual int setPolicies(const PoliciesByBucket &insertOrUpdate,
                            const KeysByBucket &remove) = 0;
    virtual int insertOrUpdateBucket(const PolicyBucketId &bucket,
                                     const PolicyResult &policyResult) = 0;
    virtual int removeBucket(const PolicyBucketId &bucket) = 0;
    virtual int adminCheck(const PolicyBucketId &startBucket, bool recursive,
                           const PolicyKey &key, PolicyResult &result) = 0;
    virtual int listPolicies(const PolicyBucketId &bucket, const PolicyKey &filter,
                             std::vector<Policy> &policies) = 0;
    virtual int erasePolicies(const PolicyBucketId &startBucket, bool recursive,
                              const PolicyKey &filter) = 0;
    virtual int listDescriptions(std::vector<PolicyDescription> &descriptions) = 0;

};

} // namespace Cynara

#endif /* SRC_ADMIN_API_APIINTERFACE_H_ */
