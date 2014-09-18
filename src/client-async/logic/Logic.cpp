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
 * @file        src/client-async/logic/Logic.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main
 *              libcynara-client-async class
 */

#include <memory>

#include <cache/CapacityCache.h>
#include <common.h>
#include <exceptions/Exception.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <plugins/NaiveInterpreter.h>
#include <protocol/ProtocolClient.h>
#include <sockets/Socket.h>
#include <sockets/SocketPath.h>

#include "Logic.h"

namespace Cynara {

Logic::Logic(cynara_status_callback callback, void *userStatusData)
    : m_statusCallback(callback, userStatusData) {
    m_socketClient = std::make_shared<SocketClientAsync>(
        SocketPath::client, std::make_shared<ProtocolClient>());

    m_cache = std::make_shared<CapacityCache>();
    auto naiveInterpreter = std::make_shared<NaiveInterpreter>();
    m_cache->registerPlugin(PredefinedPolicyType::ALLOW, naiveInterpreter);
    m_cache->registerPlugin(PredefinedPolicyType::DENY, naiveInterpreter);
}

Logic::~Logic() {
    onDisconnected();
}

int Logic::checkCache(const std::string &client, const std::string &session,
                      const std::string &user, const std::string &privilege) {
    if (!checkCacheValid())
        return CYNARA_API_CACHE_MISS;

    return m_cache->get(session, PolicyKey(client, user, privilege));
}

int Logic::createRequest(const std::string &client UNUSED, const std::string &session UNUSED,
                         const std::string &user UNUSED, const std::string &privilege UNUSED,
                         cynara_check_id &checkId UNUSED, cynara_response_callback callback UNUSED,
                         void *userResponseData UNUSED) {
    if (!ensureConnection())
        return CYNARA_API_SERVICE_NOT_AVAILABLE;

    // MOCKUP
    return CYNARA_API_MAX_PENDING_REQUESTS;
}

int Logic::process(void) {
    bool completed;
    int ret = completeConnection(completed);
    if (!completed)
        return ret;

    // MOCKUP
    return CYNARA_API_SUCCESS;
}

int Logic::cancelRequest(cynara_check_id checkId UNUSED) {
    if (!ensureConnection())
        return CYNARA_API_SERVICE_NOT_AVAILABLE;

    // MOCKUP
    return CYNARA_API_SUCCESS;
}

bool Logic::checkCacheValid(void) {
    return m_socketClient->isConnected();
}

void Logic::prepareRequestsToSend(void) {
    // MOCKUP
}

cynara_async_status Logic::socketDataStatus(void) {
    return m_socketClient->isDataToSend() ? cynara_async_status::CYNARA_STATUS_FOR_RW
                                          : cynara_async_status::CYNARA_STATUS_FOR_READ;
}

bool Logic::ensureConnection(void) {
    if (m_socketClient->isConnected())
        return true;
    onDisconnected();
    switch (m_socketClient->connect()) {
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            prepareRequestsToSend();
            m_statusCallback.onStatusChange(m_socketClient->getSockFd(), socketDataStatus());
            return true;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            prepareRequestsToSend();
            m_statusCallback.onStatusChange(m_socketClient->getSockFd(),
                                            cynara_async_status::CYNARA_STATUS_FOR_RW);
            return true;
        default:
            return false;
    }
}

int Logic::completeConnection(bool &completed) {
    switch (m_socketClient->completeConnection()) {
        case Socket::ConnectionStatus::ALREADY_CONNECTED:
            completed = true;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            m_statusCallback.onStatusChange(m_socketClient->getSockFd(), socketDataStatus());
            completed = true;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            completed = false;
            return CYNARA_API_SUCCESS;
        default:
            completed = false;
            onDisconnected();
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

void Logic::onDisconnected(void) {
    m_cache->clear();
    m_statusCallback.onDisconnected();
}

} // namespace Cynara
