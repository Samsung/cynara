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
 * @file        Logic.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of Logic class - main libcynara-client class
 */

#ifndef SRC_CLIENT_LOGIC_LOGIC_H_
#define SRC_CLIENT_LOGIC_LOGIC_H_

#include <string>

#include <sockets/SocketClient.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <api/ApiInterface.h>
#include <cache/CacheInterface.h>

namespace Cynara {

class Logic : public ApiInterface {
private:
    SocketClientPtr m_socket;
    PluginCachePtr m_cache;

    void onDisconnected(void);
    int requestResult(const PolicyKey &key, PolicyResult &result) noexcept;
public:
    Logic();
    virtual ~Logic() {};

    virtual int check(const std::string &client, const ClientSession &session,
                      const std::string &user, const std::string &privilege) noexcept;
};

} // namespace Cynara

#endif /* SRC_CLIENT_LOGIC_LOGIC_H_ */
