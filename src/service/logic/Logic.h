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
 * @file        src/service/logic/Logic.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines main class of logic layer in cynara service
 */

#ifndef SRC_SERVICE_LOGIC_LOGIC_H_
#define SRC_SERVICE_LOGIC_LOGIC_H_

#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include <main/pointers.h>
#include <plugin/PluginManager.h>
#include <request/CheckRequestManager.h>
#include <request/pointers.h>
#include <request/RequestTaker.h>

#include <cynara-plugin.h>

namespace Cynara {

class Logic : public RequestTaker {
public:
    Logic();
    virtual ~Logic();

    void bindAgentManager(const AgentManagerPtr &agentManager) {
        m_agentManager = agentManager;
    }

    void bindPluginManager(PluginManagerPtr pluginManager) {
        m_pluginManager = pluginManager;
    }

    void bindStorage(StoragePtr storage) {
        m_storage = storage;
    }

    void bindSocketManager(SocketManagerPtr socketManager) {
        m_socketManager = socketManager;
    }

    void unbindAll(void) {
        m_agentManager.reset();
        m_pluginManager.reset();
        m_storage.reset();
        m_socketManager.reset();
    }

    virtual void execute(RequestContextPtr context, AdminCheckRequestPtr request);
    virtual void execute(RequestContextPtr context, AgentActionRequestPtr request);
    virtual void execute(RequestContextPtr context, AgentRegisterRequestPtr request);
    virtual void execute(RequestContextPtr context, CancelRequestPtr request);
    virtual void execute(RequestContextPtr context, CheckRequestPtr request);
    virtual void execute(RequestContextPtr context, EraseRequestPtr request);
    virtual void execute(RequestContextPtr context, InsertOrUpdateBucketRequestPtr request);
    virtual void execute(RequestContextPtr context, ListRequestPtr request);
    virtual void execute(RequestContextPtr context, RemoveBucketRequestPtr request);
    virtual void execute(RequestContextPtr context, SetPoliciesRequestPtr request);
    virtual void execute(RequestContextPtr context, SignalRequestPtr request);

    virtual void contextClosed(RequestContextPtr context);

private:
    AgentManagerPtr m_agentManager;
    CheckRequestManager m_checkRequestManager;
    PluginManagerPtr m_pluginManager;
    StoragePtr m_storage;
    SocketManagerPtr m_socketManager;

    bool check(const RequestContextPtr &context, const PolicyKey &key,
               ProtocolFrameSequenceNumber checkId, PolicyResult &result);
    bool pluginCheck(const RequestContextPtr &context, const PolicyKey &key,
                     ProtocolFrameSequenceNumber checkId, PolicyResult &result);
    bool update(const PolicyKey &key, ProtocolFrameSequenceNumber checkId,
                const PluginData &agentData, const RequestContextPtr &request,
                const ServicePluginInterfacePtr &plugin);

    void handleAgentTalkerDisconnection(const AgentTalkerPtr &agentTalkerPtr);
    void handleClientDisconnection(const CheckContextPtr &checkContextPtr);

    void onPoliciesChanged(void);
};

} // namespace Cynara

#endif /* SRC_SERVICE_LOGIC_LOGIC_H_ */
