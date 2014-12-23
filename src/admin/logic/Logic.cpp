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
 * @file        src/admin/logic/Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-admin class
 */

#include <common.h>
#include <config/PathConfig.h>
#include <log/log.h>

#include "Logic.h"
#include "OfflineLogic.h"
#include "OnlineLogic.h"

namespace Cynara {

Logic::Logic() : m_onlineLogic(new OnlineLogic()), m_offlineLogic(new OfflineLogic()),
                 m_lockable(PathConfig::StoragePath::lockFile) {}

Logic::~Logic() {
    delete m_onlineLogic;
    delete m_offlineLogic;
}

int Logic::setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                       const ApiInterface::KeysByBucket &remove) {
    using std::placeholders::_1;
    return callApiFunction(std::bind(&ApiInterface::setPolicies, _1,
                           std::cref(insertOrUpdate), std::cref(remove)));
}

int Logic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                const PolicyResult &policyResult) {
    using std::placeholders::_1;
    auto f = std::bind(&ApiInterface::insertOrUpdateBucket, _1,
                       std::cref(bucket), std::cref(policyResult));
    return callApiFunction(f);
}

int Logic::removeBucket(const PolicyBucketId &bucket) {
    using std::placeholders::_1;
    return callApiFunction(std::bind(&ApiInterface::removeBucket, _1, std::cref(bucket)));
}

int Logic::adminCheck(const PolicyBucketId &startBucket, bool recursive, const PolicyKey &key,
                      PolicyResult &result) {
    using std::placeholders::_1;
    return callApiFunction(std::bind(&ApiInterface::adminCheck, _1, std::cref(startBucket),
                           recursive, std::cref(key), std::ref(result)));
}

int Logic::listPolicies(const PolicyBucketId &bucket, const PolicyKey &filter,
                        std::vector<Policy> &policies) {
    using std::placeholders::_1;
    return callApiFunction(std::bind(&ApiInterface::listPolicies, _1, std::cref(bucket),
                           std::cref(filter), std::ref(policies)));
}

int Logic::erasePolicies(const PolicyBucketId &startBucket, bool recursive,
                         const PolicyKey &filter) {
    using std::placeholders::_1;
    return callApiFunction(std::bind(&ApiInterface::erasePolicies, _1, std::cref(startBucket),
                           recursive, std::cref(filter)));
}

int Logic::callApiFunction(std::function<int(ApiInterface *api)> apiCall) {
    FileLock lock(m_lockable);
    if (lock.tryLock() == true) {
        m_offlineLogic->acquireDatabase();
        LOGI("Admin uses offline API");
        return apiCall(m_offlineLogic);
    } else {
        LOGI("Admin uses online API");
        return apiCall(m_onlineLogic);
    }
}

} // namespace Cynara
