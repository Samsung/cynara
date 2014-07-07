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
 * @file        NoCache.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of NoCache class - stub for no-cache version
 */

#ifndef SRC_CLIENT_CACHE_NOCACHE_H_
#define SRC_CLIENT_CACHE_NOCACHE_H_

#include <string.h>

#include <common.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <cache/CacheInterface.h>
#include <cynara-client.h>

namespace Cynara {

class NoCache : public CacheInterface {
public:
    NoCache() = default;
    virtual ~NoCache() = default;

    virtual cynara_api_result check(const std::string &session UNUSED,
                                    const PolicyKey &key UNUSED) {
        return cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    virtual cynara_api_result updateAndCheck(const std::string &session, const PolicyKey &key,
                                             const PolicyResult &result);

    virtual void clear(void) {
    }
};

} // namespace Cynara

#endif /* SRC_CLIENT_CACHE_NOCACHE_H_ */
