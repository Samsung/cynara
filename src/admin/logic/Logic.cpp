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
 * @file        src/admin/logic/Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-admin class
 */

#include <cinttypes>
#include <memory>

#include <cynara-error.h>
#include <common.h>
#include <exceptions/Exception.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolAdmin.h>
#include <request/AdminCheckRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/pointers.h>
#include <request/RemoveBucketRequest.h>
#include <request/SetPoliciesRequest.h>
#include <response/CheckResponse.h>
#include <response/CodeResponse.h>
#include <response/pointers.h>
#include <sockets/SocketClient.h>
#include <sockets/SocketPath.h>
#include <types/ProtocolFields.h>

#include "Logic.h"

namespace Cynara {

Logic::Logic() {
    m_socketClient = std::make_shared<SocketClient>(SocketPath::admin,
                                                    std::make_shared<ProtocolAdmin>());
}

ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

bool Logic::ensureConnection(void) {
    return m_socketClient->isConnected() || m_socketClient->connect();
}

template<typename T, typename... Args>
int Logic::askCynaraAndInterpreteCodeResponse(Args... args) {
    try {
        if (!ensureConnection()) {
            LOGE("Cannot connect to cynara. Service not available.");
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
        }

        ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

        //Ask cynara service
        CodeResponsePtr codeResponse;

        RequestPtr request = std::make_shared<T>(args..., sequenceNumber);
        ResponsePtr response;
        while (!(response = m_socketClient->askCynaraServer(request))) {
            if (!m_socketClient->connect())
                return CYNARA_API_SERVICE_NOT_AVAILABLE;
        }

        codeResponse = std::dynamic_pointer_cast<CodeResponse>(response);
        if (!codeResponse) {
            LOGC("Critical error. Casting Response to CodeResponse failed.");
            return CYNARA_API_UNKNOWN_ERROR;
        }

        LOGD("codeResponse: code [%" PRIu16 "]", codeResponse->m_code);
        switch (codeResponse->m_code) {
            case CodeResponse::Code::OK:
                LOGI("Policies set successfully.");
                return CYNARA_API_SUCCESS;
            case CodeResponse::Code::NOT_ALLOWED:
                LOGE("Cynara service answered: Operation not allowed.");
                return CYNARA_API_OPERATION_NOT_ALLOWED;
            case CodeResponse::Code::NO_BUCKET:
                LOGE("Trying to use unexisting bucket.");
                return CYNARA_API_BUCKET_NOT_FOUND;
            default:
                LOGE("Unexpected response code from server: [%d]",
                     static_cast<int>(codeResponse->m_code));
                return CYNARA_API_UNKNOWN_ERROR;
        }
    } catch (const std::bad_alloc &ex) {
        LOGE("Cynara admin client out of memory.");
        return CYNARA_API_OUT_OF_MEMORY;
    } catch (const std::exception &ex) {
        LOGE("Unexpected client error: <%s>", ex.what());
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

int Logic::setPolicies(const std::map<PolicyBucketId, std::vector<Policy>> &insertOrUpdate,
                       const std::map<PolicyBucketId, std::vector<PolicyKey>> &remove) {
    return askCynaraAndInterpreteCodeResponse<SetPoliciesRequest>(insertOrUpdate, remove);
}

int Logic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                const PolicyResult &policyResult) {
    return askCynaraAndInterpreteCodeResponse<InsertOrUpdateBucketRequest>(bucket, policyResult);
}

int Logic::removeBucket(const PolicyBucketId &bucket) {
    return askCynaraAndInterpreteCodeResponse<RemoveBucketRequest>(bucket);
}

int Logic::adminCheck(const PolicyBucketId &startBucket, bool recursive, const PolicyKey &key,
                      PolicyResult &result) {
    try {
        if (!ensureConnection()) {
            LOGE("Cannot connect to cynara. Service not available.");
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
        }

        ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

        //Ask cynara service
        CheckResponsePtr checkResponse;

        RequestPtr request = std::make_shared<AdminCheckRequest>(key, startBucket, recursive,
                                                                 sequenceNumber);
        ResponsePtr response;
        while (!(response = m_socketClient->askCynaraServer(request))) {
            if (!m_socketClient->connect())
                return CYNARA_API_SERVICE_NOT_AVAILABLE;
        }

        checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (!checkResponse) {
            LOGC("Casting Response to CheckResponse failed.");
            return CYNARA_API_UNKNOWN_ERROR;
        }

        LOGD("checkResponse: policyType [%" PRIu16 "], metadata <%s>",
             checkResponse->m_resultRef.policyType(),
             checkResponse->m_resultRef.metadata().c_str());

        result = checkResponse->m_resultRef;
        return CYNARA_API_SUCCESS;
    } catch (const UnexpectedErrorException &ex) {
        LOGE(ex.what());
        return CYNARA_API_UNKNOWN_ERROR;
    } catch (const std::bad_alloc &ex) {
        LOGE("Cynara admin client out of memory.");
        return CYNARA_API_OUT_OF_MEMORY;
    } catch (const std::exception &ex) {
        LOGE("Unexpected client error: <%s>", ex.what());
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

} // namespace Cynara
