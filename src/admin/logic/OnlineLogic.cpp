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
 * @file        src/admin/logic/OnlineLogic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of online version of Logic class
 */

#include <cinttypes>
#include <memory>

#include <cynara-error.h>
#include <common.h>
#include <config/PathConfig.h>
#include <exceptions/Exception.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolAdmin.h>
#include <request/AdminCheckRequest.h>
#include <request/EraseRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/ListRequest.h>
#include <request/pointers.h>
#include <request/RemoveBucketRequest.h>
#include <request/SetPoliciesRequest.h>
#include <response/AdminCheckResponse.h>
#include <response/CodeResponse.h>
#include <response/ListResponse.h>
#include <response/pointers.h>
#include <sockets/SocketClient.h>
#include <types/ProtocolFields.h>

#include "OnlineLogic.h"

namespace Cynara {

OnlineLogic::OnlineLogic() {
    m_socketClient = std::make_shared<SocketClient>(PathConfig::SocketPath::admin,
                                                    std::make_shared<ProtocolAdmin>());
}

ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

bool OnlineLogic::ensureConnection(void) {
    return m_socketClient->isConnected() || m_socketClient->connect();
}

template<typename T, typename... Args>
int OnlineLogic::askCynaraAndInterpreteCodeResponse(Args... args) {
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
            LOGI("Cynara command finished successfully.");
            return CYNARA_API_SUCCESS;
        case CodeResponse::Code::NOT_ALLOWED:
            LOGE("Cynara service answered: Operation not allowed.");
            return CYNARA_API_OPERATION_NOT_ALLOWED;
        case CodeResponse::Code::NO_BUCKET:
            LOGE("Trying to use unexisting bucket.");
            return CYNARA_API_BUCKET_NOT_FOUND;
        case CodeResponse::Code::FAILED:
            LOGC("Cynara service answered: Operation failed.");
            return CYNARA_API_OPERATION_FAILED;
        default:
            LOGE("Unexpected response code from server: [%d]",
                 static_cast<int>(codeResponse->m_code));
            return CYNARA_API_UNKNOWN_ERROR;
    }
}

int OnlineLogic::setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                             const ApiInterface::KeysByBucket &remove) {
    return askCynaraAndInterpreteCodeResponse<SetPoliciesRequest>(insertOrUpdate, remove);
}

int OnlineLogic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                      const PolicyResult &policyResult) {
    return askCynaraAndInterpreteCodeResponse<InsertOrUpdateBucketRequest>(bucket, policyResult);
}

int OnlineLogic::removeBucket(const PolicyBucketId &bucket) {
    return askCynaraAndInterpreteCodeResponse<RemoveBucketRequest>(bucket);
}

int OnlineLogic::adminCheck(const PolicyBucketId &startBucket, bool recursive, const PolicyKey &key,
                            PolicyResult &result) {
    if (!ensureConnection()) {
        LOGE("Cannot connect to cynara. Service not available.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    AdminCheckResponsePtr adminCheckResponse;

    RequestPtr request = std::make_shared<AdminCheckRequest>(key, startBucket, recursive,
                                                             sequenceNumber);
    ResponsePtr response;
    while (!(response = m_socketClient->askCynaraServer(request))) {
        if (!m_socketClient->connect())
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    adminCheckResponse = std::dynamic_pointer_cast<AdminCheckResponse>(response);
    if (!adminCheckResponse) {
        LOGC("Casting Response to AdminCheckResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }

    LOGD("AdminCheckResponse: policyType [%" PRIu16 "], metadata <%s>, bucketValid [%d]",
         adminCheckResponse->result().policyType(), adminCheckResponse->result().metadata().c_str(),
         static_cast<int>(adminCheckResponse->isBucketValid()));

    if (!adminCheckResponse->isBucketValid()) {
        LOGE("Bucket <%s> provided as startBucket in adminCheck does not exist in cynara database",
             startBucket.c_str());
        return CYNARA_API_BUCKET_NOT_FOUND;
    }

    result = adminCheckResponse->result();
    return CYNARA_API_SUCCESS;
}

int OnlineLogic::listPolicies(const PolicyBucketId &bucket, const PolicyKey &filter,
                        std::vector<Policy> &policies) {
    if (!ensureConnection()) {
        LOGE("Cannot connect to cynara. Service not available.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    ListResponsePtr listResponse;

    RequestPtr request = std::make_shared<ListRequest>(bucket, filter, sequenceNumber);
    ResponsePtr response;
    while (!(response = m_socketClient->askCynaraServer(request))) {
        if (!m_socketClient->connect())
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    listResponse = std::dynamic_pointer_cast<ListResponse>(response);
    if (!listResponse) {
        LOGC("Casting Response to ListResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }

    LOGD("listResponse: number of policies [%zu], bucketValid [%d]",
         listResponse->policies().size(), listResponse->isBucketValid());

    if (!listResponse->isBucketValid()) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    }

    policies = listResponse->policies();
    return CYNARA_API_SUCCESS;
}

int OnlineLogic::erasePolicies(const PolicyBucketId &startBucket, bool recursive,
                         const PolicyKey &filter) {
    return askCynaraAndInterpreteCodeResponse<EraseRequest>(startBucket, recursive, filter);
}

} // namespace Cynara
