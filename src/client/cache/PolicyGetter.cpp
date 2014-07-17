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
 * @file        PolicyGetter.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains PolicyResult getter class implementation.
 */

#include <cinttypes>

#include <exceptions/ServerConnectionErrorException.h>
#include <log/log.h>
#include <request/CheckRequest.h>
#include <request/pointers.h>
#include <response/CheckResponse.h>
#include <response/pointers.h>

#include <cache/PolicyGetter.h>

namespace Cynara {

cynara_api_result PolicyGetter::requestResult(const PolicyKey &key, PolicyResult &result) noexcept {
    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    CheckResponsePtr checkResponse;
    try {
        RequestPtr request = std::make_shared<CheckRequest>(key, sequenceNumber);
        ResponsePtr response = m_socketClient->askCynaraServer(request);
        if (!response) {
            LOGW("Disconnected by cynara server.");
            return cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE;
        }
        checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (!checkResponse) {
            LOGC("Critical error. Casting Response to CheckResponse failed.");
            return cynara_api_result::CYNARA_API_ACCESS_DENIED;
        }

        LOGD("checkResponse: policyType = %" PRIu16 ", metadata = %s",
             checkResponse->m_resultRef.policyType(),
             checkResponse->m_resultRef.metadata().c_str());
    } catch (const ServerConnectionErrorException &ex) {
        LOGE("Cynara service not available.");
        return cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    result = checkResponse->m_resultRef;
    return cynara_api_result::CYNARA_API_SUCCESS;
}

} // namespace Cynara
