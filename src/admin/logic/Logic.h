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
 * @file        src/admin/logic/Logic.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of Logic class - main libcynara-admin class
 */

#ifndef SRC_ADMIN_LOGIC_LOGIC_H_
#define SRC_ADMIN_LOGIC_LOGIC_H_

#include <string>

#include <sockets/SocketClient.h>

#include <api/ApiInterface.h>

namespace Cynara {

class Logic : public ApiInterface {
private:
    SocketClientPtr m_socketClient;

    bool ensureConnection(void);
    template<typename T, typename... Args>
    int askCynaraAndInterpreteCodeResponse(Args... args);

public:
    Logic();
    virtual ~Logic() {};

    virtual int setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                            const ApiInterface::KeysByBucket &remove);
    virtual int insertOrUpdateBucket(const PolicyBucketId &bucket,
                                     const PolicyResult &policyResult);
    virtual int removeBucket(const PolicyBucketId &bucket);

    virtual int adminCheck(const PolicyBucketId &startBucket, bool recursive,
                           const PolicyKey &key, PolicyResult &result);
};

} // namespace Cynara

#endif /* SRC_ADMIN_LOGIC_LOGIC_H_ */
