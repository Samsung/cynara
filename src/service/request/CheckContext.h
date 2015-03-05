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
 * @file        src/service/request/CheckContext.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Declaration of CheckContext class
 */

#ifndef SRC_SERVICE_REQUEST_CHECKCONTEXT_H_
#define SRC_SERVICE_REQUEST_CHECKCONTEXT_H_

#include <memory>

#include <containers/BinaryQueue.h>
#include <request/pointers.h>
#include <request/RequestContext.h>
#include <types/PolicyKey.h>
#include <types/ProtocolFields.h>

#include <cynara-plugin.h>

#include <agent/AgentTalker.h>

namespace Cynara {

class CheckContext {
public:
    CheckContext(const PolicyKey &key, const RequestContext &requestContext,
                 ProtocolFrameSequenceNumber checkId, ServicePluginInterfacePtr plugin,
                 const AgentTalkerPtr &agentTalkerPtr) : m_agentTalker(agentTalkerPtr),
                     m_checkId(checkId), m_key(key), m_plugin(plugin),
                     m_requestContext(requestContext), m_cancelled(false) {}
    ~CheckContext() {}

    AgentTalkerPtr m_agentTalker;
    const ProtocolFrameSequenceNumber m_checkId;
    const PolicyKey m_key;
    ServicePluginInterfacePtr m_plugin;
    RequestContext m_requestContext;
    bool m_cancelled;

    void cancel(void) {
        m_cancelled = true;
    }

    bool cancelled(void) const {
        return m_cancelled;
    }
};

typedef std::shared_ptr<CheckContext> CheckContextPtr;

} // namespace Cynara

#endif /* SRC_SERVICE_REQUEST_CHECKCONTEXT_H_ */
