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
 * @file        src/agent/api/agent-api.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-agent API
 */

#include <cstdlib>
#include <cstring>

#include <common.h>
#include <containers/RawBuffer.h>
#include <exceptions/TryCatch.h>
#include <log/log.h>
#include <response/AgentActionResponse.h>
#include <response/pointers.h>
#include <types/Agent.h>
#include <types/ProtocolFields.h>

#include <api/ApiInterface.h>
#include <cynara-agent.h>
#include <logic/Logic.h>

struct cynara_agent {
    Cynara::ApiInterface *impl;

    cynara_agent(Cynara::ApiInterface *_impl) : impl(_impl) {}

    ~cynara_agent() {
        delete impl;
    }
};

CYNARA_API
int cynara_agent_initialize(cynara_agent **pp_cynara_agent, const char *p_agent_type) {
    if (!pp_cynara_agent)
        return CYNARA_API_INVALID_PARAM;

    init_log();

    return Cynara::tryCatch([&]() {
        Cynara::LogicUniquePtr ptr(new Cynara::Logic(p_agent_type));
        *pp_cynara_agent = new cynara_agent(ptr.get());
        ptr.release();

        LOGD("Cynara agent initialized");

        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_agent_finish(cynara_agent *pp_cynara_agent) {
    delete pp_cynara_agent;

    return CYNARA_API_SUCCESS;
}

CYNARA_API
int cynara_agent_get_request(cynara_agent *p_cynara_agent, cynara_agent_msg_type *req_type,
                             cynara_agent_req_id *req_id, void **data, size_t *data_size) {

    if (!p_cynara_agent || !p_cynara_agent->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!req_type || !req_id)
        return CYNARA_API_INVALID_PARAM;
    if (!data || !data_size)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        Cynara::AgentActionResponsePtr actionResponsePtr;
        int ret = p_cynara_agent->impl->getRequest(actionResponsePtr);
        if (ret != CYNARA_API_SUCCESS)
            return ret;

        size_t dataSize = actionResponsePtr->data().size();
        if (dataSize > 0) {
            void *tmpData = malloc(dataSize);
            if (tmpData == nullptr)
                return CYNARA_API_OUT_OF_MEMORY;

            *data = tmpData;
            memcpy(*data, actionResponsePtr->data().data(), dataSize);
        }

        *data_size = dataSize;
        *req_type = static_cast<cynara_agent_msg_type>(actionResponsePtr->type());
        *req_id = actionResponsePtr->sequenceNumber();

        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_agent_put_response(cynara_agent *p_cynara_agent, const cynara_agent_msg_type resp_type,
                              const cynara_agent_req_id req_id, const void *data,
                              const size_t data_size) {

    if (!p_cynara_agent || !p_cynara_agent->impl)
        return CYNARA_API_INVALID_PARAM;
    if ((resp_type != CYNARA_MSG_TYPE_ACTION) && (resp_type != CYNARA_MSG_TYPE_CANCEL))
        return CYNARA_API_INVALID_PARAM;
    if ((!data && data_size) || (data && !data_size))
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        const char *first = static_cast<const char *>(data);
        const char *last = first + data_size;
        Cynara::RawBuffer pluginData(first, last);
        return p_cynara_agent->impl->putResponse(resp_type, req_id, pluginData);
    });
}
