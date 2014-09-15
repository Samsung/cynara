/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/protocol/ProtocolAdmin.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for administration
 */

#include <cinttypes>
#include <memory>

#include <exceptions/InvalidProtocolException.h>
#include <log/log.h>
#include <protocol/ProtocolFrame.h>
#include <protocol/ProtocolFrameSerializer.h>
#include <request/AdminCheckRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/RemoveBucketRequest.h>
#include <request/RequestContext.h>
#include <request/SetPoliciesRequest.h>
#include <response/CheckResponse.h>
#include <response/CodeResponse.h>

#include "ProtocolAdmin.h"

namespace Cynara {

ProtocolAdmin::ProtocolAdmin() {
}

ProtocolAdmin::~ProtocolAdmin() {
}

ProtocolPtr ProtocolAdmin::clone(void) {
    return std::make_shared<ProtocolAdmin>();
}

RequestPtr ProtocolAdmin::deserializeAdminCheckRequest(ProtocolFrameHeader &frame) {
    std::string clientId, userId, privilegeId;
    PolicyBucketId startBucket;
    bool recursive;

    ProtocolDeserialization::deserialize(frame, clientId);
    ProtocolDeserialization::deserialize(frame, userId);
    ProtocolDeserialization::deserialize(frame, privilegeId);
    ProtocolDeserialization::deserialize(frame, startBucket);
    ProtocolDeserialization::deserialize(frame, recursive);

    LOGD("Deserialized AdminCheckRequest: clientId <%s>, userId <%s>, privilegeId <%s>, "
         "startBucket <%s>, recursive [%d]", clientId.c_str(), userId.c_str(),
         privilegeId.c_str(), startBucket.c_str(), recursive);

    return std::make_shared<AdminCheckRequest>(PolicyKey(clientId, userId, privilegeId),
                                               startBucket, recursive, frame.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeInsertOrUpdateBucketRequest(ProtocolFrameHeader &frame) {
    PolicyBucketId policyBucketId;
    PolicyType policyType;
    PolicyResult::PolicyMetadata policyMetaData;

    ProtocolDeserialization::deserialize(frame, policyBucketId);
    ProtocolDeserialization::deserialize(frame, policyType);
    ProtocolDeserialization::deserialize(frame, policyMetaData);

    LOGD("Deserialized InsertOrUpdateBucketRequest: bucketId <%s>, "
         "result.type [%" PRIu16 "], result.meta <%s>", policyBucketId.c_str(),
         policyType, policyMetaData.c_str());

    return std::make_shared<InsertOrUpdateBucketRequest>(policyBucketId,
            PolicyResult(policyType, policyMetaData), frame.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeRemoveBucketRequest(ProtocolFrameHeader &frame) {
    PolicyBucketId policyBucketId;

    ProtocolDeserialization::deserialize(frame, policyBucketId);

    LOGD("Deserialized RemoveBucketRequest: bucketId <%s>", policyBucketId.c_str());

    return std::make_shared<RemoveBucketRequest>(policyBucketId, frame.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeSetPoliciesRequest(ProtocolFrameHeader &frame) {
    ProtocolFrameFieldsCount toBeInsertedOrUpdatedCount, toBeRemovedCount;
    ProtocolFrameFieldsCount policyCount;
    PolicyKeyFeature::ValueType clientId, user, privilege;
    PolicyType policyType;
    PolicyResult::PolicyMetadata metadata;
    std::map<PolicyBucketId, std::vector<Policy>> toBeInsertedOrUpdatedPolicies;
    std::map<PolicyBucketId, std::vector<PolicyKey>> toBeRemovedPolicies;

    ProtocolDeserialization::deserialize(frame, toBeInsertedOrUpdatedCount);
    for (ProtocolFrameFieldsCount b = 0; b < toBeInsertedOrUpdatedCount; ++b) {
        PolicyBucketId policyBucketId;
        ProtocolDeserialization::deserialize(frame, policyBucketId);
        ProtocolDeserialization::deserialize(frame, policyCount);
        for (ProtocolFrameFieldsCount p = 0; p < policyCount; ++p) {
            // PolicyKey
            ProtocolDeserialization::deserialize(frame, clientId);
            ProtocolDeserialization::deserialize(frame, user);
            ProtocolDeserialization::deserialize(frame, privilege);
            // PolicyResult
            ProtocolDeserialization::deserialize(frame, policyType);
            ProtocolDeserialization::deserialize(frame, metadata);

            toBeInsertedOrUpdatedPolicies[policyBucketId].push_back(
                    Policy(PolicyKey(clientId, user, privilege),
                    PolicyResult(policyType, metadata)));
        }
    }

    ProtocolDeserialization::deserialize(frame, toBeRemovedCount);
    for (ProtocolFrameFieldsCount b = 0; b < toBeRemovedCount; ++b) {
        PolicyBucketId policyBucketId;
        ProtocolDeserialization::deserialize(frame, policyBucketId);
        ProtocolDeserialization::deserialize(frame, policyCount);
        for (ProtocolFrameFieldsCount p = 0; p < policyCount; ++p) {
            // PolicyKey
            ProtocolDeserialization::deserialize(frame, clientId);
            ProtocolDeserialization::deserialize(frame, user);
            ProtocolDeserialization::deserialize(frame, privilege);

            toBeRemovedPolicies[policyBucketId].push_back(PolicyKey(clientId, user, privilege));
        }
    }

    LOGD("Deserialized SetPoliciesRequestPtr: insertOrUpdate count [%" PRIu16 "], "
         "remove count [%" PRIu16 "]", toBeInsertedOrUpdatedCount, toBeRemovedCount);

    return std::make_shared<SetPoliciesRequest>(toBeInsertedOrUpdatedPolicies,
                                                toBeRemovedPolicies, frame.sequenceNumber());
}

RequestPtr ProtocolAdmin::extractRequestFromBuffer(BinaryQueue &bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpAdminCheckRequest:
            return deserializeAdminCheckRequest(m_frameHeader);
        case OpInsertOrUpdateBucket:
            return deserializeInsertOrUpdateBucketRequest(m_frameHeader);
        case OpRemoveBucket:
            return deserializeRemoveBucketRequest(m_frameHeader);
        case OpSetPolicies:
            return deserializeSetPoliciesRequest(m_frameHeader);
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

ResponsePtr ProtocolAdmin::deserializeCheckResponse(ProtocolFrameHeader &frame) {
    PolicyType result;
    PolicyResult::PolicyMetadata additionalInfo;

    ProtocolDeserialization::deserialize(frame, result);
    ProtocolDeserialization::deserialize(frame, additionalInfo);

    const PolicyResult policyResult(result, additionalInfo);

    LOGD("Deserialized CheckResponse: result [%" PRIu16 "], metadata <%s>",
         policyResult.policyType(), policyResult.metadata().c_str());

    return std::make_shared<CheckResponse>(policyResult, frame.sequenceNumber());
}

ResponsePtr ProtocolAdmin::deserializeCodeResponse(ProtocolFrameHeader &frame) {
    ProtocolResponseCode responseCode;
    ProtocolDeserialization::deserialize(frame, responseCode);

    LOGD("Deserialized CodeResponse: code [%" PRIu16 "], ", responseCode);

    return std::make_shared<CodeResponse>(static_cast<CodeResponse::Code>(responseCode),
                                          frame.sequenceNumber());
}

ResponsePtr ProtocolAdmin::extractResponseFromBuffer(BinaryQueue &bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpCheckPolicyResponse:
            return deserializeCheckResponse(m_frameHeader);
        case OpCodeResponse:
            return deserializeCodeResponse(m_frameHeader);
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

void ProtocolAdmin::execute(RequestContextPtr context, AdminCheckRequestPtr request) {
    LOGD("Serializing AdminCheckRequest: client <%s>, user <%s>, privilege <%s>, "
         "startBucket <%s>, recursive [%d]", request->key().client().value().c_str(),
         request->key().user().value().c_str(), request->key().privilege().value().c_str(),
         request->startBucket().c_str(), request->recursive());

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpAdminCheckRequest);
    ProtocolSerialization::serialize(*frame, request->key().client().value());
    ProtocolSerialization::serialize(*frame, request->key().user().value());
    ProtocolSerialization::serialize(*frame, request->key().privilege().value());
    ProtocolSerialization::serialize(*frame, request->startBucket());
    ProtocolSerialization::serialize(*frame, request->recursive());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolAdmin::execute(RequestContextPtr context, InsertOrUpdateBucketRequestPtr request) {
    LOGD("Serializing InsertOrUpdateBucketRequest: sequenceNumber [%" PRIu16 "], bucketId <%s>, "
         "result.type [%" PRIu16 "], result.meta <%s>", request->sequenceNumber(),
         request->bucketId().c_str(), request->result().policyType(),
         request->result().metadata().c_str());

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpInsertOrUpdateBucket);
    ProtocolSerialization::serialize(*frame, request->bucketId());
    ProtocolSerialization::serialize(*frame, request->result().policyType());
    ProtocolSerialization::serialize(*frame, request->result().metadata());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolAdmin::execute(RequestContextPtr context, RemoveBucketRequestPtr request) {
    LOGD("Serializing RemoveBucketRequest: sequenceNumber [%" PRIu16 "], bucketId <%s>",
         request->sequenceNumber(), request->bucketId().c_str());

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpRemoveBucket);
    ProtocolSerialization::serialize(*frame, request->bucketId());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolAdmin::execute(RequestContextPtr context, SetPoliciesRequestPtr request) {
    LOGD("Serializing SetPoliciesRequestPtr: sequenceNumber [%" PRIu16 "], "
         "insertOrUpdate count [%zu], remove count [%zu]", request->sequenceNumber(),
         request->policiesToBeInsertedOrUpdated().size(), request->policiesToBeRemoved().size());

    ProtocolFramePtr frame =
            ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpSetPolicies);

    ProtocolSerialization::serialize(*frame,
            static_cast<ProtocolFrameFieldsCount>(request->policiesToBeInsertedOrUpdated().size()));
    for (auto policyBucket : request->policiesToBeInsertedOrUpdated()) {
        ProtocolSerialization::serialize(*frame, policyBucket.first);
        ProtocolSerialization::serialize(*frame,
                    static_cast<ProtocolFrameFieldsCount>(policyBucket.second.size()));
        for (auto policy : policyBucket.second) {
            // PolicyKey
            ProtocolSerialization::serialize(*frame, policy.key().client().value());
            ProtocolSerialization::serialize(*frame, policy.key().user().value());
            ProtocolSerialization::serialize(*frame, policy.key().privilege().value());
            // PolicyResult
            ProtocolSerialization::serialize(*frame, policy.result().policyType());
            ProtocolSerialization::serialize(*frame, policy.result().metadata());
        }
    }

    ProtocolSerialization::serialize(*frame,
            static_cast<ProtocolFrameFieldsCount>(request->policiesToBeRemoved().size()));
    for (auto policyBucket : request->policiesToBeRemoved()) {
        ProtocolSerialization::serialize(*frame, policyBucket.first);
        ProtocolSerialization::serialize(*frame,
                    static_cast<ProtocolFrameFieldsCount>(policyBucket.second.size()));
        for (auto policyKey : policyBucket.second) {
            // PolicyKey
            ProtocolSerialization::serialize(*frame, policyKey.client().value());
            ProtocolSerialization::serialize(*frame, policyKey.user().value());
            ProtocolSerialization::serialize(*frame, policyKey.privilege().value());
        }
    }

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolAdmin::execute(RequestContextPtr context, CheckResponsePtr response) {
    LOGD("Serializing CheckResponse: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
         "policyType [%" PRIu16 "], metadata <%s>", OpCheckPolicyResponse,
         response->sequenceNumber(), response->m_resultRef.policyType(),
         response->m_resultRef.metadata().c_str());

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(
            response->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpCheckPolicyResponse);
    ProtocolSerialization::serialize(*frame, response->m_resultRef.policyType());
    ProtocolSerialization::serialize(*frame, response->m_resultRef.metadata());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolAdmin::execute(RequestContextPtr context, CodeResponsePtr response) {
    LOGD("Serializing CodeResponse: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
         "code [%" PRIu16 "]", OpCodeResponse, response->sequenceNumber(), response->m_code);

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(
            response->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpCodeResponse);
    ProtocolSerialization::serialize(*frame, static_cast<ProtocolResponseCode>(response->m_code));

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

} // namespace Cynara
