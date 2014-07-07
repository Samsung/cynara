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
/*
 * @file        CacheInterface.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains cache interface definition.
 */

#ifndef SRC_CLIENT_CACHE_CACHEINTERFACE_H_
#define SRC_CLIENT_CACHE_CACHEINTERFACE_H_

#include <memory>
#include <string>

#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <cynara-client.h>

namespace Cynara {

class CacheInterface;
typedef std::shared_ptr<CacheInterface> CacheInterfacePtr;

class CacheInterface {
public:
    CacheInterface() = default;
    virtual ~CacheInterface() = default;

    virtual cynara_api_result check(const std::string &session, const PolicyKey &key) = 0;
    virtual cynara_api_result updateAndCheck(const std::string &session, const PolicyKey &key,
                                             const PolicyResult &result) = 0;
    virtual void clear(void) = 0;
};

} // namespace Cynara

#endif /* SRC_CLIENT_CACHE_CACHEINTERFACE_H_ */
