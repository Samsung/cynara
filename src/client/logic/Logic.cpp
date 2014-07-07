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
 * @file        Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-client class
 */

#include <memory>

#include <common.h>
#include <exceptions/ServerConnectionErrorException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolClient.h>
#include <request/CheckRequest.h>
#include <request/pointers.h>
#include <response/CheckResponse.h>
#include <response/pointers.h>
#include <sockets/SocketClient.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <cache/NoCache.h>
#include "Logic.h"

namespace Cynara {

const std::string clientSocketPath("/run/cynara/cynara.socket");

Logic::Logic() {
    m_socketClient = std::make_shared<SocketClient>(clientSocketPath,
                                                    std::make_shared<ProtocolClient>());
    m_cache = std::make_shared<NoCache>();
}

ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

cynara_api_result Logic::check(const std::string &client, const std::string &session UNUSED,
                               const std::string &user, const std::string &privilege) noexcept
{
    PolicyKey key(client, user, privilege);

    auto cacheResponse = m_cache->check(session, key);
    if(cacheResponse != cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE)
        return cacheResponse;

    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    CheckResponsePtr checkResponse;
    try {
        RequestPtr request = std::make_shared<CheckRequest>(key, sequenceNumber);
        ResponsePtr response = m_socketClient->askCynaraServer(request);
        if (!response) {
            LOGW("Disconnected by cynara server.");
            onDisconnected();
            return cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE;
        }
        checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (!checkResponse) {
            LOGC("Critical error. Casting Response to CheckResponse failed.");
            throw UnexpectedErrorException("Error casting Response to CheckResponse");
        }
    } catch (const ServerConnectionErrorException &ex) {
        LOGE("Cynara service not available.");
        onDisconnected();
        return cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE;
    } catch (const std::exception &ex) {
        LOGE("Error during check of privilege: %s", ex.what());
        return cynara_api_result::CYNARA_API_ACCESS_DENIED;
    }

    return m_cache->updateAndCheck(session, key, checkResponse->m_resultRef);
}

void Logic::onDisconnected(void) {
    m_cache->clear();
}

} // namespace Cynara
