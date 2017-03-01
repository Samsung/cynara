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
 * @file        src/monitor/logic/Logic.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class -- main libcynara-monitor class
 */

#include <mutex>

#include <cynara-error.h>
#include <log/log.h>
#include <request/MonitorGetEntriesRequest.h>
#include <request/MonitorGetFlushRequest.h>
#include <types/MonitorEntry.h>

#include "Logic.h"

namespace {
Cynara::ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static Cynara::ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

/*
 * Made for scoped lock/unlock of mutex
 */
class StatusChangeWrapper {
public:
    StatusChangeWrapper(std::mutex &mutex, bool &status) : m_mutex(mutex), m_status(status) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_status = true;
    }
    ~StatusChangeWrapper() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_status = false;
    }
private:
    std::mutex &m_mutex;
    bool &m_status;
};

/*
 * Made for scoped condition variable notify
 */
class CVNotifyWrapper {
public:
    CVNotifyWrapper(std::condition_variable &cv) : m_cv(cv) {
    }

    ~CVNotifyWrapper() {
        m_cv.notify_all();
    }
private:
    std::condition_variable &m_cv;
};
} // namespace anonymous

namespace Cynara {

int Logic::init(void) {
    if (!m_notify.init()) {
        LOGE("Couldn't initialize notification object");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    m_client.setNotifyFd(m_notify.getNotifyFd());
    return CYNARA_API_SUCCESS;
}

bool Logic::connect(void) {
    std::unique_lock<std::mutex> lock(m_mutexCond);
    CVNotifyWrapper cv(m_connectedCV);
    bool connectionEstablished = true;
    m_connectionResolved = true;
    if (!m_client.isConnected()) {
        connectionEstablished = m_client.connect();
    }
    if (!connectionEstablished) {
        LOGE("Cannot connect to Cynara service");
    }
    return connectionEstablished;
}

int Logic::sendAndFetch(std::vector<MonitorEntry> &entries) {
    StatusChangeWrapper status(m_mutexCond, m_isRunning);
    if (!connect()) {
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
    if (!m_client.sendRequest(MonitorGetEntriesRequest(m_conf.getBufferSize(),
            generateSequenceNumber()))) {
        LOGE("Failed sending request to Cynara");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    MonitorSocketClient::Event event;
    if (!m_client.waitForEvent(event)) {
        LOGE("Waiting for event failed");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
    switch(event) {
    case MonitorSocketClient::Event::FETCH_ENTRIES:
    {
        auto responsePtr = m_client.fetchEntries();
        if (!responsePtr) {
            LOGE("Error fetching response");
            return CYNARA_API_UNKNOWN_ERROR;
        }
        entries.assign(responsePtr->entries().begin(), responsePtr->entries().end());
        break;
    }
    case MonitorSocketClient::Event::NOTIFY_RETURN:
        LOGD("Got notification to stop working");
        (void)m_notify.snooze();
        break;
    }
    return CYNARA_API_SUCCESS;
}

int Logic::entriesGet(std::vector<MonitorEntry> &entries) {
    std::unique_lock<std::mutex> guard(m_reentrantGuard, std::defer_lock);
    if (!guard.try_lock()) {
        LOGE("Function is not reentrant");
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    }
    // When sendAndFetch finishes, notify
    CVNotifyWrapper cv(m_finishedCV);
    int ret = sendAndFetch(entries);
    return ret;
}

bool Logic::waitForConnectionResolved(void) {
    std::unique_lock<std::mutex> lock(m_mutexCond);
    m_connectedCV.wait(lock, [&] { return m_connectionResolved; });
    if (!m_client.isConnected()) {
        LOGE("Cynara monitor is not connected to service.");
        return false;
    }
    return true;
}

int Logic::entriesFlush(void) {
    if (!isRunning()) {
        // If no entriesGet waiting, flush doesn't make sense
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    }
    // Wait until entriesGet establishes connection
    if (!waitForConnectionResolved()) {
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
    if (!m_client.sendRequest(MonitorGetFlushRequest(generateSequenceNumber()))) {
        LOGE("Error sending request to Cynara");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    return CYNARA_API_SUCCESS;
}

bool Logic::isRunning(void) {
    std::unique_lock<std::mutex> lock(m_mutexCond);
    bool isRunning = m_isRunning;
    return isRunning;
}

void Logic::notifyFinish(void) {
    if (!isRunning()) {
        // If no entriesGet waiting, no action required
        return;
    }

    // Notify entriesGet to unblock it and let it finish gracefully
    if (!m_notify.notify()) {
        LOGE("Failed to notify");
        return;
    }

    // Wait until entriesGet finished, only then object destruction is safe
    std::unique_lock<std::mutex> lock(m_mutexCond);
    m_finishedCV.wait(lock, [&] {return !m_isRunning;});
}

} // namespace Cynara
