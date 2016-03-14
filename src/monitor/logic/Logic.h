/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/monitor/logic/Logic.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of Logic class -- main libcynara-monitor class
 */

#ifndef SRC_MONITOR_LOGIC_LOGIC_H_
#define SRC_MONITOR_LOGIC_LOGIC_H_

#include <condition_variable>
#include <memory>
#include <mutex>

#include <api/ApiInterface.h>
#include <configuration/MonitorConfiguration.h>
#include <notify/FdNotifyObject.h>
#include <socket/MonitorSocketClient.h>

namespace Cynara {

class Logic;

typedef std::unique_ptr<Logic> LogicUniquePtr;

class Logic : public ApiInterface {
public:
    explicit Logic(const MonitorConfiguration &conf = MonitorConfiguration())
        : m_conf(conf), m_connectionResolved(false), m_isRunning(false) {}
    int init(void);
    int entriesGet(std::vector<MonitorEntry> &entries);
    int entriesFlush(void);
    void notifyFinish(void);

private:
    bool isRunning(void);
    bool connect(void);
    bool waitForConnectionResolved(void);
    int sendAndFetch(std::vector<MonitorEntry> &entries);
    std::condition_variable m_connectedCV;
    std::condition_variable m_finishedCV;
    std::mutex m_mutexCond;
    std::mutex m_reentrantGuard;
    MonitorConfiguration m_conf;

    FdNotifyObject m_notify;
    MonitorSocketClient m_client;
    bool m_connectionResolved;
    bool m_isRunning;
};

} // namespace Cynara

#endif // SRC_MONITOR_LOGIC_LOGIC_H_
