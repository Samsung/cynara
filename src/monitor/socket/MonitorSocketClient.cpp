/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
/**
 * @file        src/monitor/socket/MonitorSocketClient.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of cynara's monitor socket client
 */

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <config/PathConfig.h>
#include <log/log.h>
#include <request/Request.h>
#include <request/RequestContext.h>

#include "MonitorSocketClient.h"

namespace Cynara {

MonitorSocketClient::MonitorSocketClient()
    : m_socket(PathConfig::SocketPath::monitorGet),
      m_notifyFd(-1)
{

}

void MonitorSocketClient::setNotifyFd(int notifyFd) {
    m_notifyFd = notifyFd;
}

bool MonitorSocketClient::isConnected(void) {
    return m_socket.isConnected();
}

bool MonitorSocketClient::connect(void) {
    switch (m_socket.connect()) {
    case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
        return true;
    case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
        return m_socket.completeConnection() == Socket::ConnectionStatus::CONNECTION_SUCCEEDED;
    default:
        return false;
    }
}

bool MonitorSocketClient::sendRequest(const Request &request) {
    std::lock_guard<std::mutex> lock(m_mutex);
    //pass request to protocol
    BinaryQueuePtr bbqSharedPtr(&m_writeQueue, [](BinaryQueue *) {});
    RequestContext context(ResponseTakerPtr(), bbqSharedPtr);
    request.execute(m_protocol, context);

    //send request to cynara
    if (m_socket.sendToServer(m_writeQueue) == Socket::SendStatus::CONNECTION_LOST) {
        LOGW("Disconnected while sending request to Cynara.");
        return false;
    }
    return true;
}

bool MonitorSocketClient::waitForEvent(MonitorSocketClient::Event &event) {
    int cynaraFd = m_socket.getSockFd();
    pollfd desc[2];
    desc[0].fd = cynaraFd;
    desc[0].events = POLLIN;

    desc[1].fd = m_notifyFd;
    desc[1].events = POLLIN;

    int ret = TEMP_FAILURE_RETRY(poll(desc, 2, -1));
    if (ret == -1) {
        int err = errno;
        LOGE("Poll returned with error: " << strerror(err));
        return false;
    }
    if (desc[1].revents & POLLIN) {
        LOGD("Poll returned with notification to return");
        event = Event::NOTIFY_RETURN;
    }

    if (desc[0].revents & POLLIN) {
        LOGD("Poll returned with fetch entries");
        event = Event::FETCH_ENTRIES;
    }
    return true;
}

MonitorGetEntriesResponsePtr MonitorSocketClient::fetchEntries(void) {

    while (true) {
        if (!m_socket.receiveFromServer(m_readQueue)) {
            LOGW("Disconnected while receiving response from Cynara.");
            return nullptr;
        }
        BinaryQueuePtr bbqSharedPtr(&m_readQueue, [](BinaryQueue *) {});
        ResponsePtr response = m_protocol.extractResponseFromBuffer(bbqSharedPtr);
        if (response) {
            MonitorGetEntriesResponsePtr monRes =
                    std::dynamic_pointer_cast<MonitorGetEntriesResponse>(response);
            return monRes;
        }
    }
}

} /* namespace Cynara */
