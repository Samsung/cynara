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
 * @file        Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements main class of logic layer in cynara service
 */

#include <log/log.h>
#include <common.h>
#include <exceptions/PluginNotFoundException.h>
#include <signal.h>

#include <main/Cynara.h>
#include <request/CheckRequest.h>
#include <request/RequestContext.h>
#include <request/SignalRequest.h>
#include <response/CheckResponse.h>
#include <storage/Storage.h>

#include <sockets/SocketManager.h>

#include "Logic.h"

namespace Cynara {
Logic::Logic() {
}

Logic::~Logic() {
}

void Logic::execute(RequestContextPtr context UNUSED, SignalRequestPtr request) {
    LOGD("Processing signal: [%d]", request->signalNumber());

    switch (request->signalNumber()) {
    case SIGTERM:
        LOGI("SIGTERM received!");
        m_socketManager->mainLoopStop();
        break;
    }
}

void Logic::execute(RequestContextPtr context, CheckRequestPtr request) {
    PolicyResult result(PredefinedPolicyType::DENY);
    if (check(context, request->key(), result)) {
        context->returnResponse(context, std::make_shared<CheckResponse>(result, request->sequenceNumber()));
    }
}

bool Logic::check(RequestContextPtr context UNUSED, const PolicyKey &key,
                  PolicyResult& result) {
    result = m_storage->checkPolicy(key);

    switch (result.policyType()) {
        case PredefinedPolicyType::ALLOW :
            LOGD("check of policy key <%s> returned ALLOW", key.toString().c_str());
            return true;
        case PredefinedPolicyType::DENY :
            LOGD("check of policy key <%s> returned DENY", key.toString().c_str());
            return true;
    }
    //todo pass question to proper plugin that:
    //  1) returns false when answer has to be waited for (UI)
    //  2) returns true if Response is to be generated
    // In case 1) context should be saved in plugin in order to return answer when ready.

    //in case no proper plugin is found
    throw PluginNotFoundException(result);
}

} // namespace Cynara
