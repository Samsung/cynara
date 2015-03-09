/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of online version of Logic class
 */

#include <cinttypes>
#include <memory>
#include <utility>

#include <cynara-error.h>
#include <common.h>
#include <config/PathConfig.h>
#include <exceptions/Exception.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolAdmin.h>
#include <request/AdminCheckRequest.h>
#include <request/DescriptionListRequest.h>
#include <request/EraseRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/ListRequest.h>
#include <request/pointers.h>
#include <request/RemoveBucketRequest.h>
#include <request/SetPoliciesRequest.h>
#include <response/AdminCheckResponse.h>
#include <response/CodeResponse.h>
#include <response/DescriptionListResponse.h>
#include <response/ListResponse.h>
#include <response/pointers.h>
#include <sockets/SocketClient.h>
#include <types/ProtocolFields.h>

#include "OnlineLogic.h"

namespace Cynara {

OnlineLogic::OnlineLogic() : m_socketClient(PathConfig::SocketPath::admin,
                                            std::make_shared<ProtocolAdmin>()) {
}

ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

bool OnlineLogic::ensureConnection(void) {
    return m_socketClient.isConnected() || m_socketClient.connect();
}

template<typename Req, typename Res, typename... ReqArgs>
int OnlineLogic::getResponse(std::shared_ptr<Res> &retResponse, ReqArgs&&... args) {
    if (!ensureConnection()) {
        LOGE("Cannot connect to cynara. Service not available.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    Req request(std::forward<ReqArgs>(args)..., sequenceNumber);
    ResponsePtr response;
    while (!(response = m_socketClient.askCynaraServer(request))) {
        if (!m_socketClient.connect())
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    retResponse = std::dynamic_pointer_cast<Res>(response);
    if (!retResponse) {
        LOGC("Critical error. Casting Response failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }

    return CYNARA_API_SUCCESS;
}

static int interpretCodeResponse(const CodeResponse::Code &code) {
    LOGD("codeResponse: code [%" PRIu16 "]", code);
    switch (code) {
        case CodeResponse::Code::OK:
            LOGI("Cynara command finished successfully.");
            return CYNARA_API_SUCCESS;
        case CodeResponse::Code::NOT_ALLOWED:
            LOGE("Cynara service answered: Operation not allowed.");
            return CYNARA_API_OPERATION_NOT_ALLOWED;
        case CodeResponse::Code::NO_BUCKET:
            LOGE("Trying to use unexisting bucket.");
            return CYNARA_API_BUCKET_NOT_FOUND;
        case CodeResponse::Code::NO_POLICY_TYPE:
            LOGE("Trying to use unknown policy type.");
            return CYNARA_API_INVALID_PARAM;
        case CodeResponse::Code::FAILED:
            LOGC("Cynara service answered: Operation failed.");
            return CYNARA_API_OPERATION_FAILED;
        case CodeResponse::Code::DB_CORRUPTED:
            LOGC("Cynara service answered: Database is corrupted.");
            return CYNARA_API_DATABASE_CORRUPTED;
        default:
            LOGE("Unexpected response code from server: [%d]",
                 static_cast<int>(code));
            return CYNARA_API_UNKNOWN_ERROR;
    }
}

int OnlineLogic::setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                       const ApiInterface::KeysByBucket &remove) {
    CodeResponsePtr codeResponse;
    int ret = getResponse<SetPoliciesRequest>(codeResponse, insertOrUpdate, remove);
    if (ret != CYNARA_API_SUCCESS) {
        return ret;
    }

    return interpretCodeResponse(codeResponse->m_code);
}

int OnlineLogic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                const PolicyResult &policyResult) {
    CodeResponsePtr codeResponse;
    int ret = getResponse<InsertOrUpdateBucketRequest>(codeResponse, bucket, policyResult);
    if (ret != CYNARA_API_SUCCESS) {
        return ret;
    }

    return interpretCodeResponse(codeResponse->m_code);
}

int OnlineLogic::removeBucket(const PolicyBucketId &bucket) {
    CodeResponsePtr codeResponse;
    int ret = getResponse<RemoveBucketRequest>(codeResponse, bucket);
    if (ret != CYNARA_API_SUCCESS) {
        return ret;
    }

    return interpretCodeResponse(codeResponse->m_code);
}

int OnlineLogic::adminCheck(const PolicyBucketId &startBucket, bool recursive, const PolicyKey &key,
                      PolicyResult &result) {
    AdminCheckResponsePtr adminCheckResponse;
    int ret = getResponse<AdminCheckRequest>(adminCheckResponse, key, startBucket, recursive);
    if (ret != CYNARA_API_SUCCESS) {
        return ret;
    }

    if (adminCheckResponse->isDbCorrupted()) {
        LOGC("Cynara service answered: Database is corrupted.");
        return CYNARA_API_DATABASE_CORRUPTED;
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
    ListResponsePtr listResponse;
    int ret = getResponse<ListRequest>(listResponse, bucket, filter);
    if (ret != CYNARA_API_SUCCESS) {
        return ret;
    }

    if (listResponse->isDbCorrupted()) {
        LOGC("Cynara service answered: Database is corrupted.");
        return CYNARA_API_DATABASE_CORRUPTED;
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
    CodeResponsePtr codeResponse;
    int ret = getResponse<EraseRequest>(codeResponse, startBucket, recursive, filter);
    if (ret != CYNARA_API_SUCCESS) {
        return ret;
    }

    return interpretCodeResponse(codeResponse->m_code);
}

int OnlineLogic::listDescriptions(std::vector<PolicyDescription> &descriptions) {
    DescriptionListResponsePtr descrResponse;
    int ret = getResponse<DescriptionListRequest>(descrResponse);
    if (ret != CYNARA_API_SUCCESS) {
        return ret;
    }

    if (descrResponse->isDbCorrupted()) {
        LOGC("Cynara service answered: Database is corrupted.");
        return CYNARA_API_DATABASE_CORRUPTED;
    }

    LOGD("descriptionListResponse: number of plugin descriptions [%zu]",
         descrResponse->descriptions().size());

    descriptions = descrResponse->descriptions();
    return CYNARA_API_SUCCESS;
}

} // namespace Cynara
