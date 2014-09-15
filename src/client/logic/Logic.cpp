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
 * @file        Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-client class
 */

#include <memory>

#include <cache/CapacityCache.h>
#include <common.h>
#include <cynara-client-error.h>
#include <exceptions/ServerConnectionErrorException.h>
#include <log/log.h>
#include <plugins/NaiveInterpreter.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolClient.h>
#include <request/CheckRequest.h>
#include <request/pointers.h>
#include <response/CheckResponse.h>
#include <response/pointers.h>
#include <sockets/SocketClient.h>

#include <logic/Logic.h>

namespace Cynara {

const std::string clientSocketPath("/run/cynara/cynara.socket");

static ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

Logic::Logic() {
    m_socket = std::make_shared<SocketClient>(clientSocketPath, std::make_shared<ProtocolClient>());
    m_cache = std::make_shared<CapacityCache>();
    auto naiveInterpreter = std::make_shared<NaiveInterpreter>();
    m_cache->registerPlugin(PredefinedPolicyType::ALLOW, naiveInterpreter);
    m_cache->registerPlugin(PredefinedPolicyType::DENY, naiveInterpreter);
    m_cache->registerPlugin(PredefinedPolicyType::BUCKET, naiveInterpreter);
}

int Logic::check(const std::string &client, const ClientSession &session, const std::string &user,
                 const std::string &privilege) noexcept
{
    if (!m_socket->isConnected()){
        onDisconnected();
    }

    PolicyKey key(client, user, privilege);
    auto ret = m_cache->get(session, key);
    //Any other situation than cache miss
    if (ret != CYNARA_API_CACHE_MISS) {
        return ret;
    }

    //No value in Cache
    PolicyResult result;
    ret = requestResult(key, result);
    if (ret != CYNARA_API_SUCCESS) {
        LOGE("Error fetching new entry.");
        return ret;
    }

    return m_cache->update(session, key, result);
}

int Logic::requestResult(const PolicyKey &key, PolicyResult &result) noexcept {
    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    CheckResponsePtr checkResponse;
    try {
        RequestPtr request = std::make_shared<CheckRequest>(key, sequenceNumber);
        ResponsePtr response = m_socket->askCynaraServer(request);
        if (!response) {
            LOGW("Disconnected by cynara server.");
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
        }
        checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (!checkResponse) {
            LOGC("Critical error. Casting Response to CheckResponse failed.");
            return CYNARA_API_ACCESS_DENIED;
        }

        LOGD("checkResponse: policyType = %" PRIu16 ", metadata = %s",
             checkResponse->m_resultRef.policyType(),
             checkResponse->m_resultRef.metadata().c_str());
    } catch (const ServerConnectionErrorException &ex) {
        LOGE("Cynara service not available.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    result = checkResponse->m_resultRef;
    return CYNARA_API_SUCCESS;
}

void Logic::onDisconnected(void) {
    m_cache->clear();
}

} // namespace Cynara
