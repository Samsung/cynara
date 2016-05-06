/*
 * Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/service/logic/Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file implements main class of logic layer in cynara service
 */

#include <csignal>
#include <cinttypes>
#include <functional>
#include <memory>
#include <vector>

#include <attributes/attributes.h>
#include <log/log.h>
#include <common.h>
#include <log/log.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DatabaseCorruptedException.h>
#include <exceptions/DatabaseException.h>
#include <exceptions/DefaultBucketDeletionException.h>
#include <exceptions/DefaultBucketSetNoneException.h>
#include <exceptions/InvalidBucketIdException.h>
#include <exceptions/PluginErrorException.h>
#include <exceptions/PluginNotFoundException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <exceptions/UnknownPolicyTypeException.h>
#include <request/AdminCheckRequest.h>
#include <request/AgentActionRequest.h>
#include <request/AgentRegisterRequest.h>
#include <request/CancelRequest.h>
#include <request/CheckRequest.h>
#include <request/DescriptionListRequest.h>
#include <request/EraseRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/ListRequest.h>
#include <request/MonitorEntriesPutRequest.h>
#include <request/MonitorEntryPutRequest.h>
#include <request/MonitorGetEntriesRequest.h>
#include <request/MonitorGetFlushRequest.h>
#include <request/RemoveBucketRequest.h>
#include <request/RequestContext.h>
#include <request/SetPoliciesRequest.h>
#include <request/SignalRequest.h>
#include <request/SimpleCheckRequest.h>
#include <response/AdminCheckResponse.h>
#include <response/AgentRegisterResponse.h>
#include <response/CancelResponse.h>
#include <response/CheckResponse.h>
#include <response/CodeResponse.h>
#include <response/DescriptionListResponse.h>
#include <response/ListResponse.h>
#include <response/MonitorGetEntriesResponse.h>
#include <response/SimpleCheckResponse.h>
#include <types/Policy.h>

#include <main/Cynara.h>
#include <agent/AgentManager.h>
#include <sockets/SocketManager.h>
#include <storage/Storage.h>

#include <cynara-plugin.h>

#include <cynara-agent.h>

#include "Logic.h"

namespace Cynara {

Logic::Logic() : m_dbCorrupted(false) {
}

Logic::~Logic() {
}

void Logic::execute(const RequestContext &context UNUSED, const SignalRequest &request) {
    LOGD("Processing signal: [%d]", request.signalNumber());

    switch (request.signalNumber()) {
    case SIGTERM:
        LOGI("SIGTERM received!");
        m_socketManager->mainLoopStop();
        break;
    }
}

void Logic::execute(const RequestContext &context, const AdminCheckRequest &request) {
    PolicyResult result;
    bool bucketValid = true;

    if (m_dbCorrupted) {
        bucketValid = false;
    } else {
        try {
            result = m_storage->checkPolicy(request.key(), request.startBucket(),
                                            request.recursive());
        } catch (const BucketNotExistsException &ex) {
            bucketValid = false;
        }
    }

    context.returnResponse(AdminCheckResponse(result, bucketValid, m_dbCorrupted,
                                              request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const AgentActionRequest &request) {
    AgentTalkerPtr talkerPtr = m_agentManager->getTalker(context.responseQueue(),
                                                         request.sequenceNumber());
    if (!talkerPtr) {
        LOGD("Received response from agent with invalid request id: [%" PRIu16 "]",
             request.sequenceNumber());
        return;
    }

    CheckContextPtr checkContextPtr = m_checkRequestManager.getContext(talkerPtr);
    if (!checkContextPtr) {
        LOGE("No matching check context for agent talker.");
        m_agentManager->removeTalker(talkerPtr);
        return;
    }

    if (!checkContextPtr->cancelled()) {
        PluginData data(request.data().begin(), request.data().end());
        if (request.type() == CYNARA_MSG_TYPE_CANCEL) {
            // Nothing to do for now
        } else if (request.type() == CYNARA_MSG_TYPE_ACTION) {
            update(checkContextPtr->m_key, checkContextPtr->m_checkId, data,
                   checkContextPtr->m_requestContext, checkContextPtr->m_plugin);
        } else {
            LOGE("Invalid response type [%d] in response from agent <%s>",
                 static_cast<int>(request.type()), talkerPtr->agentType().c_str());
            // TODO: disconnect agent
        }
    }

    m_agentManager->removeTalker(talkerPtr);
    m_checkRequestManager.removeRequest(checkContextPtr);
}

void Logic::execute(const RequestContext &context, const AgentRegisterRequest &request) {
    auto result = m_agentManager->registerAgent(request.agentType(), context.responseQueue());
    context.returnResponse(AgentRegisterResponse(result, request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const CancelRequest &request) {
    CheckContextPtr checkContextPtr = m_checkRequestManager.getContext(context.responseQueue(),
                                                                       request.sequenceNumber());
    if (!checkContextPtr) {
        LOGD("Cancel request id: [%" PRIu16 "] with no matching request in progress.",
             request.sequenceNumber());
        return;
    }

    if (checkContextPtr->cancelled())
        return;

    checkContextPtr->cancel();
    checkContextPtr->m_agentTalker->cancel();

    LOGD("Returning response for cancel request id: [%" PRIu16 "].", request.sequenceNumber());
    context.returnResponse(CancelResponse(request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const CheckRequest &request) {
    PolicyResult result(PredefinedPolicyType::DENY);
    if (check(context, request.key(), request.sequenceNumber(), result)) {
        m_auditLog.log(request.key(), result);
        context.returnResponse(CheckResponse(result, request.sequenceNumber()));
    }
}

bool Logic::check(const RequestContext &context, const PolicyKey &key,
                  ProtocolFrameSequenceNumber checkId, PolicyResult &result) {

    if (m_checkRequestManager.getContext(context.responseQueue(), checkId)) {
        LOGE("Check request for checkId: [%" PRIu16 "] is already processing", checkId);
        return false;
    }

    result = (m_dbCorrupted ? PredefinedPolicyType::DENY : m_storage->checkPolicy(key));

    switch (result.policyType()) {
        case PredefinedPolicyType::ALLOW :
            LOGD("check of policy key <%s> returned ALLOW", key.toString().c_str());
            return true;
        case PredefinedPolicyType::DENY :
            LOGD("check of policy key <%s> returned DENY", key.toString().c_str());
            return true;
    }

    return pluginCheck(context, key, checkId, result);
}

bool Logic::pluginCheck(const RequestContext &context, const PolicyKey &key,
                        ProtocolFrameSequenceNumber checkId, PolicyResult &result) {

    LOGD("Trying to check policy: <%s> in plugin.", key.toString().c_str());

    ExternalPluginPtr plugin = m_pluginManager->getPlugin(result.policyType());
    if (!plugin) {
        LOGE("Plugin not found for policy: [0x%x]", result.policyType());
        result = PolicyResult(PredefinedPolicyType::DENY);
        return true;
    }

    ServicePluginInterfacePtr servicePlugin =
            std::dynamic_pointer_cast<ServicePluginInterface>(plugin);
    if (!servicePlugin) {
        result = PolicyResult(PredefinedPolicyType::DENY);
        return true;
    }

    AgentType requiredAgent;
    PluginData pluginData;

    auto ret = servicePlugin->check(key.client().toString(), key.user().toString(),
                                    key.privilege().toString(), result, requiredAgent, pluginData);

    switch (ret) {
        case ServicePluginInterface::PluginStatus::ANSWER_READY:
            return true;
        case ServicePluginInterface::PluginStatus::ANSWER_NOTREADY: {
                result = PolicyResult(PredefinedPolicyType::DENY);
                AgentTalkerPtr agentTalker = m_agentManager->createTalker(requiredAgent);
                if (!agentTalker) {
                    LOGE("Required agent talker for: <%s> could not be created.",
                         requiredAgent.c_str());
                    return true;
                }

                if (!m_checkRequestManager.createContext(key, context, checkId, servicePlugin,
                                                         agentTalker)) {
                    LOGE("Check context for checkId: [%" PRIu16 "] could not be created.",
                         checkId);
                    m_agentManager->removeTalker(agentTalker);
                    return true;
                }
                agentTalker->send(pluginData);
            }
            return false;
        default:
            result = PolicyResult(PredefinedPolicyType::DENY);
            return true;
    }
}

bool Logic::update(const PolicyKey &key, ProtocolFrameSequenceNumber checkId,
                   const PluginData &agentData, const RequestContext &context,
                   const ServicePluginInterfacePtr &plugin) {

    LOGD("Check update: <%s>:[%" PRIu16 "]", key.toString().c_str(), checkId);

    PolicyResult result;
    bool answerReady = false;
    auto ret = plugin->update(key.client().toString(), key.user().toString(),
                              key.privilege().toString(), agentData, result);
    switch (ret) {
        case ServicePluginInterface::PluginStatus::SUCCESS:
            answerReady = true;
            break;
        case ServicePluginInterface::PluginStatus::ERROR:
            result = PolicyResult(PredefinedPolicyType::DENY);
            answerReady = true;
            break;
        default:
            throw PluginErrorException(key);
    }

    if (answerReady && context.responseQueue()) {
        m_auditLog.log(key, result);
        context.returnResponse(CheckResponse(result, checkId));
        return true;
    }

    return false;
}

void Logic::execute(const RequestContext &context, const DescriptionListRequest &request) {
    auto descriptions = m_pluginManager->getPolicyDescriptions();
    descriptions.insert(descriptions.begin(), predefinedPolicyDescr.begin(),
                        predefinedPolicyDescr.end());
    context.returnResponse(DescriptionListResponse(descriptions, m_dbCorrupted,
                                                   request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const EraseRequest &request) {
    auto code = CodeResponse::Code::OK;

    if (m_dbCorrupted) {
        code = CodeResponse::Code::DB_CORRUPTED;
    } else {
        try {
        m_storage->erasePolicies(request.startBucket(), request.recursive(), request.filter());
            onPoliciesChanged();
        } catch (const DatabaseException &ex) {
            code = CodeResponse::Code::FAILED;
        } catch (const BucketNotExistsException &ex) {
            code = CodeResponse::Code::NO_BUCKET;
        }
    }

    context.returnResponse(CodeResponse(code, request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const InsertOrUpdateBucketRequest &request) {
    auto code = CodeResponse::Code::OK;

    if (m_dbCorrupted) {
        code = CodeResponse::Code::DB_CORRUPTED;
    } else {
        try {
            checkSinglePolicyType(request.result().policyType(), true, true);
            m_storage->addOrUpdateBucket(request.bucketId(), request.result());
            onPoliciesChanged();
        } catch (const DatabaseException &ex) {
            code = CodeResponse::Code::FAILED;
        } catch (const DefaultBucketSetNoneException &ex) {
            code = CodeResponse::Code::NOT_ALLOWED;
        } catch (const InvalidBucketIdException &ex) {
            code = CodeResponse::Code::NOT_ALLOWED;
        } catch (const UnknownPolicyTypeException &ex) {
            code = CodeResponse::Code::NO_POLICY_TYPE;
        }
    }

    context.returnResponse(CodeResponse(code, request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const ListRequest &request) {
    bool bucketValid = true;
    std::vector<Policy> policies;

    if (m_dbCorrupted) {
        bucketValid = false;
    } else {
        try {
            policies = m_storage->listPolicies(request.bucket(), request.filter());
        } catch (const BucketNotExistsException &ex) {
            bucketValid = false;
        }
    }

    context.returnResponse(ListResponse(policies, bucketValid, m_dbCorrupted,
                                        request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const RemoveBucketRequest &request) {
    auto code = CodeResponse::Code::OK;

    if (m_dbCorrupted) {
        code = CodeResponse::Code::DB_CORRUPTED;
    } else {
        try {
            m_storage->deleteBucket(request.bucketId());
            onPoliciesChanged();
        } catch (const DatabaseException &ex) {
            code = CodeResponse::Code::FAILED;
        } catch (const BucketNotExistsException &ex) {
            code = CodeResponse::Code::NO_BUCKET;
        } catch (const DefaultBucketDeletionException &ex) {
            code = CodeResponse::Code::NOT_ALLOWED;
        }
    }
    context.returnResponse(CodeResponse(code, request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const SetPoliciesRequest &request) {
    auto code = CodeResponse::Code::OK;

    if (m_dbCorrupted) {
        code = CodeResponse::Code::DB_CORRUPTED;
    } else {
        try {
            checkPoliciesTypes(request.policiesToBeInsertedOrUpdated(), true, false);
            m_storage->insertPolicies(request.policiesToBeInsertedOrUpdated());
            m_storage->deletePolicies(request.policiesToBeRemoved());
            onPoliciesChanged();
        } catch (const DatabaseException &ex) {
            code = CodeResponse::Code::FAILED;
        } catch (const BucketNotExistsException &ex) {
            code = CodeResponse::Code::NO_BUCKET;
        } catch (const UnknownPolicyTypeException &ex) {
            code = CodeResponse::Code::NO_POLICY_TYPE;
        }
    }

    context.returnResponse(CodeResponse(code, request.sequenceNumber()));
}

void Logic::execute(const RequestContext &context, const SimpleCheckRequest &request) {
    int retValue = CYNARA_API_SUCCESS;
    PolicyResult result;
    PolicyKey key = request.key();
    result = m_storage->checkPolicy(key);

    switch (result.policyType()) {
    case PredefinedPolicyType::ALLOW:
        LOGD("simple check of policy key <%s> returned ALLOW", key.toString().c_str());
        break;
    case PredefinedPolicyType::DENY:
        LOGD("simple check of policy key <%s> returned DENY", key.toString().c_str());
        break;
    default: {
        ExternalPluginPtr plugin = m_pluginManager->getPlugin(result.policyType());
        if (!plugin) {
            LOGE("Plugin not found for policy: [0x%x]", result.policyType());
            result = PolicyResult(PredefinedPolicyType::DENY);
            retValue = CYNARA_API_SUCCESS;
            break;
        }

        ServicePluginInterfacePtr servicePlugin =
                std::dynamic_pointer_cast<ServicePluginInterface>(plugin);
        if (!servicePlugin) {
            LOGE("Couldn't cast plugin pointer to ServicePluginInterface");
            result = PolicyResult(PredefinedPolicyType::DENY);
            retValue = CYNARA_API_SUCCESS;
            break;
        }

        AgentType requiredAgent;
        PluginData pluginData;
        auto ret = servicePlugin->check(key.client().toString(), key.user().toString(),
                                        key.privilege().toString(), result, requiredAgent,
                                        pluginData);
        switch (ret) {
        case ServicePluginInterface::PluginStatus::ANSWER_READY:
            LOGD("simple check of policy key <%s> in plugin returned [" PRIu16 "]",
                 key.toString().c_str(), result.policyType());
            break;
        case ServicePluginInterface::PluginStatus::ANSWER_NOTREADY:
            retValue = CYNARA_API_ACCESS_NOT_RESOLVED;
            break;
        default:
            result = PolicyResult(PredefinedPolicyType::DENY);
            retValue = CYNARA_API_SUCCESS;
        }
    }
    }
    m_auditLog.log(request.key(), result);
    context.returnResponse(SimpleCheckResponse(retValue, result,
                                               request.sequenceNumber()));
}

void Logic::sendMonitorResponses(void) {
    if (m_monitorLogic.shouldSend()) {
        auto responses = m_monitorLogic.getResponses();
        for (auto &response : responses) {
            auto responseInfo = response.info;
            auto &responseVec = response.entries;
            responseInfo.context.returnResponse(MonitorGetEntriesResponse(responseVec,
                                                                          responseInfo.seq));
        }
    }
}

void Logic::execute(const RequestContext &context, const MonitorGetEntriesRequest &request) {
    m_monitorLogic.addClient(context, request.sequenceNumber(), request.bufferSize());
    sendMonitorResponses();
}

void Logic::execute(const RequestContext &context, const MonitorGetFlushRequest &request UNUSED) {
    m_monitorLogic.flushClient(context);
    sendMonitorResponses();
}

void Logic::execute(const RequestContext &context UNUSED, const MonitorEntriesPutRequest &request) {
    for (unsigned i = 0; i < request.monitorEntries().size(); i++) {
        m_monitorLogic.addEntry(request.monitorEntries()[i]);
        sendMonitorResponses();
    }
}

void Logic::execute(const RequestContext &context UNUSED, const MonitorEntryPutRequest &request) {
    m_monitorLogic.addEntry(request.monitorEntry());
    sendMonitorResponses();
}

void Logic::checkPoliciesTypes(const std::map<PolicyBucketId, std::vector<Policy>> &policies,
                               bool allowBucket, bool allowNone) {
    for (const auto &group : policies) {
        for (const auto &policy : group.second) {
            checkSinglePolicyType(policy.result().policyType(), allowBucket, allowNone);
        }
    }
}

void Logic::checkSinglePolicyType(const PolicyType &policyType, bool allowBucket, bool allowNone) {
    if (allowBucket && policyType == PredefinedPolicyType::BUCKET)
        return;
    if (allowNone && policyType == PredefinedPolicyType::NONE)
        return;
    for (const auto &descr : predefinedPolicyDescr) {
        if (descr.type == policyType)
            return;
    }
    m_pluginManager->checkPolicyType(policyType);
}

void Logic::contextClosed(const RequestContext &context) {
    LOGD("context closed");

    LinkId linkId = context.responseQueue();

    m_agentManager->cleanupAgent(linkId, [&](const AgentTalkerPtr &talker) -> void {
                                 handleAgentTalkerDisconnection(talker); });

    m_checkRequestManager.cancelRequests(linkId,
                                         [&](const CheckContextPtr &checkContextPtr) -> void {
                                         handleClientDisconnection(checkContextPtr); });
    m_monitorLogic.removeClient(context);
}

void Logic::onPoliciesChanged(void) {
    m_storage->save();
    m_socketManager->disconnectAllClients();
    m_pluginManager->invalidateAll();
    //todo remove all saved contexts (if there will be any saved contexts)
}

void Logic::handleAgentTalkerDisconnection(const AgentTalkerPtr &agentTalkerPtr) {
    CheckContextPtr checkContextPtr = m_checkRequestManager.getContext(agentTalkerPtr);
    if (checkContextPtr == nullptr) {
        LOGE("No matching check context for agent talker.");
        return;
    }

    RequestContext &context = checkContextPtr->m_requestContext;
    if (!checkContextPtr->cancelled() && context.responseQueue()) {
        PolicyResult result(PredefinedPolicyType::DENY);
        m_auditLog.log(checkContextPtr->m_key, result);
        context.returnResponse(CheckResponse(result, checkContextPtr->m_checkId));
    }

    m_checkRequestManager.removeRequest(checkContextPtr);
}

void Logic::handleClientDisconnection(const CheckContextPtr &checkContextPtr) {
    LOGD("Handle client disconnection");

    if (!checkContextPtr->cancelled()) {
        checkContextPtr->cancel();
        checkContextPtr->m_agentTalker->cancel();
    }
}

void Logic::loadDb(void) {
    try {
        m_storage->load();
    } catch (const DatabaseCorruptedException &) {
        m_dbCorrupted = true;
    }
}

} // namespace Cynara
