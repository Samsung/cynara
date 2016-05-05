/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/service/monitor/MonitorLogic.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements logic class of monitor part of cynara service
 */

#include <log/log.h>

#include "MonitorLogic.h"

namespace Cynara {
void MonitorLogic::addClient(const RequestContext &context, ProtocolFrameSequenceNumber seq,
                             int bufferSize) {
    auto insertIt = m_clients.find(context.clientId());
    if (insertIt == m_clients.end()) {
        if (!m_manager.addClient(context.clientId(), bufferSize)) {
            LOGE("Error adding client to entries manager");
            return;
        }
        m_clients.emplace(context.clientId(), MonitorResponseInfo(context, seq));
    } else {
        if (!m_manager.modifyClient(context.clientId(), bufferSize)) {
            LOGE("Error modifying client in entries manager");
            return;
        }
        insertIt->second.seq = seq;
    }
    if (m_manager.isClientFilled(context.clientId())) {
        m_responseCache.emplace(context.clientId(),
                MonitorResponse(context, seq, m_manager.fetchEntriesForClient(context.clientId())));
    }
}

void MonitorLogic::addEntry(const MonitorEntry &e) {
    if (!m_manager.addEntry(e)) {
        LOGD("No entry added.");
        return;
    }
    RequestContext::ClientId id;
    while ((id = m_manager.getFilledClientId()) != -1) {
        auto clientIt = m_clients.find(id);
        if (clientIt == m_clients.end()) {
            LOGE("Client [" << id << "] doesn't exist in logic but kept in manager!");
            return;
        }
        m_responseCache.emplace(id, MonitorResponse(clientIt->second,
                                                    m_manager.fetchEntriesForClient(id)));
    }
}

void MonitorLogic::removeClient(const RequestContext &context) {
    (void)m_manager.removeClient(context.clientId());
    m_clients.erase(context.clientId());
    m_responseCache.erase(context.clientId());
}

void MonitorLogic::flushClient(const RequestContext &context) {
    auto clientIt = m_clients.find(context.clientId());
    if (clientIt == m_clients.end()) {
        LOGE("Client [" << context.clientId() << "] doesn't exist");
        return;
    }
    m_responseCache.emplace(context.clientId(),
                    MonitorResponse(clientIt->second.context, clientIt->second.seq,
                                    m_manager.fetchEntriesForClient(context.clientId(), true)));
}

bool MonitorLogic::shouldSend(void) {
    return m_responseCache.size() > 0;
}

std::vector<MonitorLogic::MonitorResponse> MonitorLogic::getResponses(void) {
    std::vector<MonitorLogic::MonitorResponse> responses;
    responses.reserve(m_responseCache.size());
    for (auto &responseInfoPair : m_responseCache) {
        responses.push_back(std::move(std::get<1>(responseInfoPair)));
    }
    m_responseCache.clear();
    return responses;
}
} /* namespace Cynara */
