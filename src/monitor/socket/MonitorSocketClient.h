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
 * @file        src/monitor/socket/MonitorSocketClient.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of cynara's monitor socket client
 */

#ifndef SRC_MONITOR_SOCKET_MONITORSOCKETCLIENT_H_
#define SRC_MONITOR_SOCKET_MONITORSOCKETCLIENT_H_

#include <mutex>

#include <containers/BinaryQueue.h>
#include <protocol/ProtocolMonitorGet.h>
#include <request/pointers.h>
#include <response/MonitorGetEntriesResponse.h>
#include <sockets/Socket.h>

namespace Cynara {

class MonitorSocketClient {
public:
    enum class Event {
        FETCH_ENTRIES,
        NOTIFY_RETURN
    };
    MonitorSocketClient();
    void setNotifyFd(int notifyFd);
    bool isConnected(void);
    bool connect(void);
    bool waitForEvent(Event &event);
    bool sendRequest(const Request &request);
    MonitorGetEntriesResponsePtr fetchEntries(void);
private:
    Socket m_socket;
    ProtocolMonitorGet m_protocol;
    BinaryQueue m_readQueue;
    BinaryQueue m_writeQueue;
    int m_notifyFd;
    std::mutex m_mutex;
};

} /* namespace Cynara */

#endif /* SRC_MONITOR_SOCKET_MONITORSOCKETCLIENT_H_ */
