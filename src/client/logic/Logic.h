/*
 *  Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/client/logic/Logic.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of Logic class - main libcynara-client class
 */

#ifndef SRC_CLIENT_LOGIC_LOGIC_H_
#define SRC_CLIENT_LOGIC_LOGIC_H_

#include <memory>
#include <string>

#include <sockets/SocketClient.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <configuration/Configuration.h>

#include <api/ApiInterface.h>
#include <cache/CapacityCache.h>
#include <cache/MonitorCache.h>

namespace Cynara {

class Logic;
typedef std::unique_ptr<Logic> LogicUniquePtr;

class Logic : public ApiInterface {
public:
    explicit Logic(const Configuration &conf = Configuration());

    virtual int check(const std::string &client, const ClientSession &session,
                      const std::string &user, const std::string &privilege);
    virtual int simpleCheck(const std::string &client, const ClientSession &session,
                            const std::string &user, const std::string &privilege);
    virtual void flushMonitor(void);
private:
    SocketClient m_socketClient;
    CapacityCache m_cache;
    MonitorCache m_monitorCache;
    const bool m_monitoringEnabled;

    void onDisconnected(void);
    bool ensureConnection(void);
    template <typename Req, typename Res>
    std::shared_ptr<Res> requestResponse(const PolicyKey &key);
    int requestResult(const PolicyKey &key, PolicyResult &result);
    int requestSimpleResult(const PolicyKey &key, PolicyResult &result);
    bool requestMonitorEntriesPut();

    void updateMonitor(const PolicyKey &policyKey, int result);
};

} // namespace Cynara

#endif /* SRC_CLIENT_LOGIC_LOGIC_H_ */
