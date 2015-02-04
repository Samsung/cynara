/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
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
#include <request/DescriptionListRequest.h>
#include <request/EraseRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/ListRequest.h>
#include <request/RemoveBucketRequest.h>
#include <request/RequestContext.h>
#include <request/SetPoliciesRequest.h>
#include <response/AdminCheckResponse.h>
#include <response/CodeResponse.h>
#include <response/DescriptionListResponse.h>
#include <response/ListResponse.h>
#include <types/PolicyKey.h>

#include "ProtocolAdmin.h"

namespace Cynara {

ProtocolAdmin::ProtocolAdmin() {
}

ProtocolAdmin::~ProtocolAdmin() {
}

ProtocolPtr ProtocolAdmin::clone(void) {
    return std::make_shared<ProtocolAdmin>();
}

RequestPtr ProtocolAdmin::deserializeAdminCheckRequest(void) {
    std::string clientId, userId, privilegeId;
    PolicyBucketId startBucket;
    bool recursive;

    ProtocolDeserialization::deserialize(m_frameHeader, clientId);
    ProtocolDeserialization::deserialize(m_frameHeader, userId);
    ProtocolDeserialization::deserialize(m_frameHeader, privilegeId);
    ProtocolDeserialization::deserialize(m_frameHeader, startBucket);
    ProtocolDeserialization::deserialize(m_frameHeader, recursive);

    LOGD("Deserialized AdminCheckRequest: clientId <%s>, userId <%s>, privilegeId <%s>, "
         "startBucket <%s>, recursive [%d]", clientId.c_str(), userId.c_str(),
         privilegeId.c_str(), startBucket.c_str(), recursive);

    return std::make_shared<AdminCheckRequest>(PolicyKey(clientId, userId, privilegeId),
                                               startBucket, recursive,
                                               m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeDescriptionListRequest(void) {
    LOGD("Deserialized DescriptionListRequest");
    return std::make_shared<DescriptionListRequest>(m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeEraseRequest(void) {
    PolicyBucketId startBucket;
    bool recursive;
    PolicyKeyFeature::ValueType client, user, privilege;

    ProtocolDeserialization::deserialize(m_frameHeader, startBucket);
    ProtocolDeserialization::deserialize(m_frameHeader, recursive);
    ProtocolDeserialization::deserialize(m_frameHeader, client);
    ProtocolDeserialization::deserialize(m_frameHeader, user);
    ProtocolDeserialization::deserialize(m_frameHeader, privilege);

    LOGD("Deserialized EraseRequest: startBucket <%s>, recursive [%d], filter client <%s> "
         "filter user <%s>, filter privilege <%s>", startBucket.c_str(),
         static_cast<int>(recursive), client.c_str(), user.c_str(), privilege.c_str());

    return std::make_shared<EraseRequest>(startBucket, recursive,
                                          PolicyKey(client, user, privilege),
                                          m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeInsertOrUpdateBucketRequest(void) {
    PolicyBucketId policyBucketId;
    PolicyType policyType;
    PolicyResult::PolicyMetadata policyMetaData;

    ProtocolDeserialization::deserialize(m_frameHeader, policyBucketId);
    ProtocolDeserialization::deserialize(m_frameHeader, policyType);
    ProtocolDeserialization::deserialize(m_frameHeader, policyMetaData);

    LOGD("Deserialized InsertOrUpdateBucketRequest: bucketId <%s>, "
         "result.type [%" PRIu16 "], result.meta <%s>", policyBucketId.c_str(),
         policyType, policyMetaData.c_str());

    return std::make_shared<InsertOrUpdateBucketRequest>(policyBucketId,
            PolicyResult(policyType, policyMetaData), m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeListRequest(void) {
    PolicyBucketId bucketId;
    PolicyKeyFeature::ValueType client, user, privilege;

    ProtocolDeserialization::deserialize(m_frameHeader, bucketId);
    ProtocolDeserialization::deserialize(m_frameHeader, client);
    ProtocolDeserialization::deserialize(m_frameHeader, user);
    ProtocolDeserialization::deserialize(m_frameHeader, privilege);

    LOGD("Deserialized ListRequest: bucketId <%s>, filter client <%s> filter user <%s>, filter "
         "privilege <%s>", bucketId.c_str(), client.c_str(), user.c_str(), privilege.c_str());

    return std::make_shared<ListRequest>(bucketId, PolicyKey(client, user, privilege),
                                         m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeRemoveBucketRequest(void) {
    PolicyBucketId policyBucketId;

    ProtocolDeserialization::deserialize(m_frameHeader, policyBucketId);

    LOGD("Deserialized RemoveBucketRequest: bucketId <%s>", policyBucketId.c_str());

    return std::make_shared<RemoveBucketRequest>(policyBucketId, m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAdmin::deserializeSetPoliciesRequest(void) {
    ProtocolFrameFieldsCount toBeInsertedOrUpdatedCount, toBeRemovedCount;
    ProtocolFrameFieldsCount policyCount;
    PolicyKeyFeature::ValueType clientId, user, privilege;
    PolicyType policyType;
    PolicyResult::PolicyMetadata metadata;
    std::map<PolicyBucketId, std::vector<Policy>> toBeInsertedOrUpdatedPolicies;
    std::map<PolicyBucketId, std::vector<PolicyKey>> toBeRemovedPolicies;

    ProtocolDeserialization::deserialize(m_frameHeader, toBeInsertedOrUpdatedCount);
    for (ProtocolFrameFieldsCount b = 0; b < toBeInsertedOrUpdatedCount; ++b) {
        PolicyBucketId policyBucketId;
        ProtocolDeserialization::deserialize(m_frameHeader, policyBucketId);
        ProtocolDeserialization::deserialize(m_frameHeader, policyCount);
        for (ProtocolFrameFieldsCount p = 0; p < policyCount; ++p) {
            // PolicyKey
            ProtocolDeserialization::deserialize(m_frameHeader, clientId);
            ProtocolDeserialization::deserialize(m_frameHeader, user);
            ProtocolDeserialization::deserialize(m_frameHeader, privilege);
            // PolicyResult
            ProtocolDeserialization::deserialize(m_frameHeader, policyType);
            ProtocolDeserialization::deserialize(m_frameHeader, metadata);

            toBeInsertedOrUpdatedPolicies[policyBucketId].push_back(
                    Policy(PolicyKey(clientId, user, privilege),
                    PolicyResult(policyType, metadata)));
        }
    }

    ProtocolDeserialization::deserialize(m_frameHeader, toBeRemovedCount);
    for (ProtocolFrameFieldsCount b = 0; b < toBeRemovedCount; ++b) {
        PolicyBucketId policyBucketId;
        ProtocolDeserialization::deserialize(m_frameHeader, policyBucketId);
        ProtocolDeserialization::deserialize(m_frameHeader, policyCount);
        for (ProtocolFrameFieldsCount p = 0; p < policyCount; ++p) {
            // PolicyKey
            ProtocolDeserialization::deserialize(m_frameHeader, clientId);
            ProtocolDeserialization::deserialize(m_frameHeader, user);
            ProtocolDeserialization::deserialize(m_frameHeader, privilege);

            toBeRemovedPolicies[policyBucketId].push_back(PolicyKey(clientId, user, privilege));
        }
    }

    LOGD("Deserialized SetPoliciesRequestPtr: insertOrUpdate count [%" PRIu16 "], "
         "remove count [%" PRIu16 "]", toBeInsertedOrUpdatedCount, toBeRemovedCount);

    return std::make_shared<SetPoliciesRequest>(toBeInsertedOrUpdatedPolicies,
                                                toBeRemovedPolicies,
                                                m_frameHeader.sequenceNumber());
}

RequestPtr ProtocolAdmin::extractRequestFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpAdminCheckRequest:
            return deserializeAdminCheckRequest();
        case OpDescriptionListRequest:
            return deserializeDescriptionListRequest();
        case OpEraseRequest:
            return deserializeEraseRequest();
        case OpInsertOrUpdateBucket:
            return deserializeInsertOrUpdateBucketRequest();
        case OpListRequest:
            return deserializeListRequest();
        case OpRemoveBucket:
            return deserializeRemoveBucketRequest();
        case OpSetPolicies:
            return deserializeSetPoliciesRequest();
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

ResponsePtr ProtocolAdmin::deserializeAdminCheckResponse(void) {
    PolicyType result;
    PolicyResult::PolicyMetadata additionalInfo;
    bool bucketValid;
    bool dbCorrupted;

    ProtocolDeserialization::deserialize(m_frameHeader, result);
    ProtocolDeserialization::deserialize(m_frameHeader, additionalInfo);
    ProtocolDeserialization::deserialize(m_frameHeader, bucketValid);
    ProtocolDeserialization::deserialize(m_frameHeader, dbCorrupted);

    const PolicyResult policyResult(result, additionalInfo);

    LOGD("Deserialized AdminCheckResponse: result [%" PRIu16 "], metadata <%s>, bucketValid [%d], "
         "dbCorrupted [%d]", policyResult.policyType(), policyResult.metadata().c_str(),
         static_cast<int>(bucketValid), static_cast<int>(dbCorrupted));

    return std::make_shared<AdminCheckResponse>(policyResult, bucketValid, dbCorrupted,
                                                m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolAdmin::deserializeCodeResponse(void) {
    ProtocolResponseCode responseCode;
    ProtocolDeserialization::deserialize(m_frameHeader, responseCode);

    LOGD("Deserialized CodeResponse: code [%" PRIu16 "], ", responseCode);

    return std::make_shared<CodeResponse>(static_cast<CodeResponse::Code>(responseCode),
                                          m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolAdmin::deserializeDescriptionListResponse(void) {
    ProtocolFrameFieldsCount descriptionsCount;

    ProtocolDeserialization::deserialize(m_frameHeader, descriptionsCount);
    std::vector<PolicyDescription> descriptions(descriptionsCount,
                                                PolicyDescription(PredefinedPolicyType::NONE));

    for (ProtocolFrameFieldsCount fields = 0; fields < descriptionsCount; fields++) {
        ProtocolDeserialization::deserialize(m_frameHeader, descriptions[fields].type);
        ProtocolDeserialization::deserialize(m_frameHeader, descriptions[fields].name);
    }

    bool dbCorrupted;
    ProtocolDeserialization::deserialize(m_frameHeader, dbCorrupted);

    LOGD("Deserialized DescriptionListResponse: number of descriptions [%" PRIu16 "], "
         "dbCorrupted [%d]", descriptionsCount, static_cast<int>(dbCorrupted));

    return std::make_shared<DescriptionListResponse>(descriptions, dbCorrupted,
                                                     m_frameHeader.sequenceNumber());
}

ResponsePtr ProtocolAdmin::deserializeListResponse(void) {
    ProtocolFrameFieldsCount policiesCount;
    PolicyKeyFeature::ValueType client, user, privilege;
    PolicyType policyType;
    PolicyResult::PolicyMetadata metadata;
    std::vector<Policy> policies;

    ProtocolDeserialization::deserialize(m_frameHeader, policiesCount);
    policies.reserve(policiesCount);

    for (ProtocolFrameFieldsCount p = 0; p < policiesCount; ++p) {
        // PolicyKey
        ProtocolDeserialization::deserialize(m_frameHeader, client);
        ProtocolDeserialization::deserialize(m_frameHeader, user);
        ProtocolDeserialization::deserialize(m_frameHeader, privilege);
        // PolicyResult
        ProtocolDeserialization::deserialize(m_frameHeader, policyType);
        ProtocolDeserialization::deserialize(m_frameHeader, metadata);

        policies.push_back(Policy(PolicyKey(client, user, privilege),
                                        PolicyResult(policyType, metadata)));
    }

    bool bucketValid;
    bool dbCorrupted;
    ProtocolDeserialization::deserialize(m_frameHeader, bucketValid);
    ProtocolDeserialization::deserialize(m_frameHeader, dbCorrupted);

    LOGD("Deserialized ListResponse: number of policies [%" PRIu16 "], bucketValid [%d], "
         "dbCorrupted [%d]", policiesCount, static_cast<int>(bucketValid),
         static_cast<int>(dbCorrupted));

    return std::make_shared<ListResponse>(policies, bucketValid, dbCorrupted,
                                          m_frameHeader.sequenceNumber());

}

ResponsePtr ProtocolAdmin::extractResponseFromBuffer(BinaryQueuePtr bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%" PRIu8 "]", opCode);
        switch (opCode) {
        case OpAdminCheckPolicyResponse:
            return deserializeAdminCheckResponse();
        case OpCodeResponse:
            return deserializeCodeResponse();
        case OpDescriptionListResponse:
            return deserializeDescriptionListResponse();
        case OpListResponse:
            return deserializeListResponse();
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

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpAdminCheckRequest);
    ProtocolSerialization::serialize(frame, request->key().client().value());
    ProtocolSerialization::serialize(frame, request->key().user().value());
    ProtocolSerialization::serialize(frame, request->key().privilege().value());
    ProtocolSerialization::serialize(frame, request->startBucket());
    ProtocolSerialization::serialize(frame, request->recursive());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, DescriptionListRequestPtr request) {
    LOGD("Serializing DescriptionListRequest");
    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpDescriptionListRequest);

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, EraseRequestPtr request) {
    LOGD("Serializing EraseRequest: sequenceNumber [%" PRIu16 "], startBucket <%s>, "
         "recursive [%d], filter client <%s> filter user <%s> filter privilege <%s>",
         request->sequenceNumber(), request->startBucket().c_str(),
         static_cast<int>(request->recursive()), request->filter().client().value().c_str(),
         request->filter().user().value().c_str(), request->filter().privilege().value().c_str());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpEraseRequest);
    ProtocolSerialization::serialize(frame, request->startBucket());
    ProtocolSerialization::serialize(frame, request->recursive());
    ProtocolSerialization::serialize(frame, request->filter().client().value());
    ProtocolSerialization::serialize(frame, request->filter().user().value());
    ProtocolSerialization::serialize(frame, request->filter().privilege().value());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, InsertOrUpdateBucketRequestPtr request) {
    LOGD("Serializing InsertOrUpdateBucketRequest: sequenceNumber [%" PRIu16 "], bucketId <%s>, "
         "result.type [%" PRIu16 "], result.meta <%s>", request->sequenceNumber(),
         request->bucketId().c_str(), request->result().policyType(),
         request->result().metadata().c_str());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpInsertOrUpdateBucket);
    ProtocolSerialization::serialize(frame, request->bucketId());
    ProtocolSerialization::serialize(frame, request->result().policyType());
    ProtocolSerialization::serialize(frame, request->result().metadata());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, ListRequestPtr request) {
    LOGD("Serializing ListRequest: sequenceNumber [%" PRIu16 "], bucketId <%s>, "
         "filter client <%s> filter user <%s> filter privilege <%s>", request->sequenceNumber(),
         request->bucket().c_str(), request->filter().client().value().c_str(),
         request->filter().user().value().c_str(), request->filter().privilege().value().c_str());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpListRequest);
    ProtocolSerialization::serialize(frame, request->bucket());
    ProtocolSerialization::serialize(frame, request->filter().client().value());
    ProtocolSerialization::serialize(frame, request->filter().user().value());
    ProtocolSerialization::serialize(frame, request->filter().privilege().value());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, RemoveBucketRequestPtr request) {
    LOGD("Serializing RemoveBucketRequest: sequenceNumber [%" PRIu16 "], bucketId <%s>",
         request->sequenceNumber(), request->bucketId().c_str());

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpRemoveBucket);
    ProtocolSerialization::serialize(frame, request->bucketId());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, SetPoliciesRequestPtr request) {
    LOGD("Serializing SetPoliciesRequestPtr: sequenceNumber [%" PRIu16 "], "
         "insertOrUpdate count [%zu], remove count [%zu]", request->sequenceNumber(),
         request->policiesToBeInsertedOrUpdated().size(), request->policiesToBeRemoved().size());

    ProtocolFrame frame =
            ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpSetPolicies);

    ProtocolSerialization::serialize(frame,
            static_cast<ProtocolFrameFieldsCount>(request->policiesToBeInsertedOrUpdated().size()));
    for (auto policyBucket : request->policiesToBeInsertedOrUpdated()) {
        ProtocolSerialization::serialize(frame, policyBucket.first);
        ProtocolSerialization::serialize(frame,
                    static_cast<ProtocolFrameFieldsCount>(policyBucket.second.size()));
        for (auto policy : policyBucket.second) {
            // PolicyKey
            ProtocolSerialization::serialize(frame, policy.key().client().value());
            ProtocolSerialization::serialize(frame, policy.key().user().value());
            ProtocolSerialization::serialize(frame, policy.key().privilege().value());
            // PolicyResult
            ProtocolSerialization::serialize(frame, policy.result().policyType());
            ProtocolSerialization::serialize(frame, policy.result().metadata());
        }
    }

    ProtocolSerialization::serialize(frame,
            static_cast<ProtocolFrameFieldsCount>(request->policiesToBeRemoved().size()));
    for (auto policyBucket : request->policiesToBeRemoved()) {
        ProtocolSerialization::serialize(frame, policyBucket.first);
        ProtocolSerialization::serialize(frame,
                    static_cast<ProtocolFrameFieldsCount>(policyBucket.second.size()));
        for (auto policyKey : policyBucket.second) {
            // PolicyKey
            ProtocolSerialization::serialize(frame, policyKey.client().value());
            ProtocolSerialization::serialize(frame, policyKey.user().value());
            ProtocolSerialization::serialize(frame, policyKey.privilege().value());
        }
    }

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, AdminCheckResponsePtr response) {
    LOGD("Serializing AdminCheckResponse: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
         "policyType [%" PRIu16 "], metadata <%s>, bucketValid [%d], dbCorrupted [%d]",
         OpAdminCheckPolicyResponse, response->sequenceNumber(), response->result().policyType(),
         response->result().metadata().c_str(), static_cast<int>(response->isBucketValid()),
         static_cast<int>(response->isDbCorrupted()));

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(
            response->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpAdminCheckPolicyResponse);
    ProtocolSerialization::serialize(frame, response->result().policyType());
    ProtocolSerialization::serialize(frame, response->result().metadata());
    ProtocolSerialization::serialize(frame, response->isBucketValid());
    ProtocolSerialization::serialize(frame, response->isDbCorrupted());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, CodeResponsePtr response) {
    LOGD("Serializing CodeResponse: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
         "code [%" PRIu16 "]", OpCodeResponse, response->sequenceNumber(), response->m_code);

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(
            response->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpCodeResponse);
    ProtocolSerialization::serialize(frame, static_cast<ProtocolResponseCode>(response->m_code));

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, DescriptionListResponsePtr response) {
    ProtocolFrameFieldsCount descriptionsSize
        = static_cast<ProtocolFrameFieldsCount>(response->descriptions().size());

    LOGD("Serializing DescriptionListResponse: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
         "number of descriptions [%" PRIu16 "], dbCorrupted [%d]", OpDescriptionListResponse,
         response->sequenceNumber(), descriptionsSize, static_cast<int>(response->isDbCorrupted()));

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(response->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpDescriptionListResponse);
    ProtocolSerialization::serialize(frame, descriptionsSize);
    for (auto &desc : response->descriptions()) {
        ProtocolSerialization::serialize(frame, desc.type);
        ProtocolSerialization::serialize(frame, desc.name);
    }
    ProtocolSerialization::serialize(frame, response->isDbCorrupted());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

void ProtocolAdmin::execute(RequestContextPtr context, ListResponsePtr response) {
    ProtocolFrameFieldsCount policiesSize
        = static_cast<ProtocolFrameFieldsCount>(response->policies().size());

    LOGD("Serializing ListResponse: op [%" PRIu8 "], sequenceNumber [%" PRIu16 "], "
         "number of policies [%" PRIu16 "], bucketValid [%d], dbCorrupted [%d]", OpListResponse,
         response->sequenceNumber(), policiesSize, static_cast<int>(response->isBucketValid()),
         static_cast<int>(response->isDbCorrupted()));

    ProtocolFrame frame = ProtocolFrameSerializer::startSerialization(response->sequenceNumber());

    ProtocolSerialization::serialize(frame, OpListResponse);
    ProtocolSerialization::serialize(frame, policiesSize);
    for (auto &policy : response->policies()) {
        // PolicyKey
        ProtocolSerialization::serialize(frame, policy.key().client().value());
        ProtocolSerialization::serialize(frame, policy.key().user().value());
        ProtocolSerialization::serialize(frame, policy.key().privilege().value());
        // PolicyResult
        ProtocolSerialization::serialize(frame, policy.result().policyType());
        ProtocolSerialization::serialize(frame, policy.result().metadata());
    }
    ProtocolSerialization::serialize(frame, response->isBucketValid());
    ProtocolSerialization::serialize(frame, response->isDbCorrupted());

    ProtocolFrameSerializer::finishSerialization(frame, *(context->responseQueue()));
}

} // namespace Cynara
