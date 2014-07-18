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
/*
 * @file        ProtocolAdmin.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for administration
 */

#include <memory>

#include <exceptions/InvalidProtocolException.h>
#include <log/log.h>
#include <protocol/ProtocolFrame.h>
#include <protocol/ProtocolFrameSerializer.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/RemoveBucketRequest.h>
#include <request/RequestContext.h>
#include <request/SetPoliciesRequest.h>
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

RequestPtr ProtocolAdmin::deserializeInsertOrUpdateBucketRequest(ProtocolFrameHeader &frame) {
    PolicyBucketId policyBucketId;
    PolicyType policyType;
    PolicyResult::PolicyMetadata policyMetaData;

    ProtocolDeserialization::deserialize(frame, policyBucketId);
    ProtocolDeserialization::deserialize(frame, policyType);
    ProtocolDeserialization::deserialize(frame, policyMetaData);

    LOGD("Deserialized InsertOrUpdateBucketRequest: bucketId <%s>, "
         "result.type [%u], result.meta <%s>", policyBucketId.c_str(),
         static_cast<unsigned int>(policyType), policyMetaData.c_str());

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

    LOGD("Deserialized SetPoliciesRequestPtr: insertOrUpdate count [%u], remove count [%u]",
         static_cast<unsigned int>(toBeInsertedOrUpdatedCount),
         static_cast<unsigned int>(toBeRemovedCount));

    return std::make_shared<SetPoliciesRequest>(toBeInsertedOrUpdatedPolicies,
                                                toBeRemovedPolicies, frame.sequenceNumber());
}

RequestPtr ProtocolAdmin::extractRequestFromBuffer(BinaryQueue &bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%u]", static_cast<unsigned int>(opCode));
        switch (opCode) {
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

ResponsePtr ProtocolAdmin::deserializeCodeResponse(ProtocolFrameHeader &frame) {
    ProtocolResponseCode responseCode;
    ProtocolDeserialization::deserialize(frame, responseCode);

    LOGD("Deserialized CodeResponse: code [%u], ", static_cast<unsigned int>(responseCode));

    return std::make_shared<CodeResponse>(static_cast<CodeResponse::Code>(responseCode),
                                          frame.sequenceNumber());
}

ResponsePtr ProtocolAdmin::extractResponseFromBuffer(BinaryQueue &bufferQueue) {
    ProtocolFrameSerializer::deserializeHeader(m_frameHeader, bufferQueue);

    if (m_frameHeader.isFrameComplete()) {
        ProtocolOpCode opCode;

        m_frameHeader.resetState();
        ProtocolDeserialization::deserialize(m_frameHeader, opCode);
        LOGD("Deserialized opCode [%u]", static_cast<unsigned int>(opCode));
        switch (opCode) {
        case OpCodeResponse:
            return deserializeCodeResponse(m_frameHeader);
        default:
            throw InvalidProtocolException(InvalidProtocolException::WrongOpCode);
            break;
        }
    }

    return nullptr;
}

void ProtocolAdmin::execute(RequestContextPtr context, InsertOrUpdateBucketRequestPtr request) {
    LOGD("Serializing InsertOrUpdateBucketRequest: sequenceNumber [%u], bucketId <%s>, "
         "result.type [%u], result.meta <%s>",
         static_cast<unsigned int>(request->sequenceNumber()),
         request->bucketId().c_str(),
         static_cast<unsigned int>(request->result().policyType()),
         request->result().metadata().c_str());

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpInsertOrUpdateBucket);
    ProtocolSerialization::serialize(*frame, request->bucketId());
    ProtocolSerialization::serialize(*frame, request->result().policyType());
    ProtocolSerialization::serialize(*frame, request->result().metadata());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolAdmin::execute(RequestContextPtr context, RemoveBucketRequestPtr request) {
    LOGD("Serializing RemoveBucketRequest: sequenceNumber [%u], bucketId <%s>",
         static_cast<unsigned int>(request->sequenceNumber()), request->bucketId().c_str());

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(request->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpRemoveBucket);
    ProtocolSerialization::serialize(*frame, request->bucketId());

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

void ProtocolAdmin::execute(RequestContextPtr context, SetPoliciesRequestPtr request) {
    LOGD("Serializing SetPoliciesRequestPtr: sequenceNumber [%u], insertOrUpdate count [%zu]",
         "remove count [%zu]", static_cast<unsigned int>(request->sequenceNumber()),
         request->policiesToBeInsertedOrUpdated().size(),
         request->policiesToBeRemoved().size());

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

void ProtocolAdmin::execute(RequestContextPtr context, CodeResponsePtr response) {
    LOGD("Serializing CodeResponse: sequenceNumber [%u], code [%u]",
         static_cast<unsigned int>(response->sequenceNumber()),
         static_cast<unsigned int>(response->m_code));

    ProtocolFramePtr frame = ProtocolFrameSerializer::startSerialization(
            response->sequenceNumber());

    ProtocolSerialization::serialize(*frame, OpCodeResponse);
    ProtocolSerialization::serialize(*frame, static_cast<ProtocolResponseCode>(response->m_code));

    ProtocolFrameSerializer::finishSerialization(frame, context->responseQueue());
}

} // namespace Cynara
