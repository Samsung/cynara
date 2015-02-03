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
 * @file        src/client-async/api/client-async-api.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-client-async API
 */

#include <new>

#include <common.h>
#include <exceptions/TryCatch.h>
#include <log/log.h>

#include <api/ApiInterface.h>
#include <configuration/Configuration.h>
#include <cynara-client-async.h>
#include <logic/Logic.h>

struct cynara_async {
    Cynara::ApiInterface *impl;

    cynara_async(Cynara::ApiInterface *_impl) : impl(_impl) {
    }

    ~cynara_async() {
        delete impl;
    }
};

struct cynara_async_configuration {
    Cynara::Configuration* impl;

    cynara_async_configuration(Cynara::Configuration *_impl) : impl(_impl) {}

    ~cynara_async_configuration() {
        delete impl;
    }
};

CYNARA_API
int cynara_async_configuration_create(cynara_async_configuration **pp_conf) {
    if (!pp_conf)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        Cynara::ConfigurationUniquePtr ptr(new Cynara::Configuration());
        *pp_conf = new cynara_async_configuration(ptr.get());
        ptr.release();
        LOGD("Cynara configuration initialized");
        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
void cynara_async_configuration_destroy(cynara_async_configuration *p_conf) {
    delete p_conf;
}

CYNARA_API
int cynara_async_configuration_set_cache_size(cynara_async_configuration *p_conf,
                                              size_t cache_size) {
    if (!p_conf || !p_conf->impl)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        p_conf->impl->setCacheSize(cache_size);
        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_async_initialize(cynara_async **pp_cynara,
                            const cynara_async_configuration *p_conf,
                            cynara_status_callback callback, void *user_status_data) {
    if (!pp_cynara)
        return CYNARA_API_INVALID_PARAM;

    init_log();

    return Cynara::tryCatch([&]() {
        Cynara::LogicUniquePtr ptr;
        if (p_conf && p_conf->impl) {
            ptr.reset(new Cynara::Logic(callback, user_status_data, *(p_conf->impl)));
        } else {
            ptr.reset(new Cynara::Logic(callback, user_status_data));
        }
        *pp_cynara = new cynara_async(ptr.get());
        ptr.release();

        LOGD("Cynara client async initialized");

        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
void cynara_async_finish(cynara_async *p_cynara) {
    if (!p_cynara->impl->isFinishPermitted())
        return;

    delete p_cynara;
}

CYNARA_API
int cynara_async_check_cache(cynara_async *p_cynara, const char *client, const char *client_session,
                             const char *user, const char *privilege) {
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!client || !client_session || !user || !privilege)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        std::string clientStr;
        std::string clientSessionStr;
        std::string userStr;
        std::string privilegeStr;

        try {
            clientStr = client;
            clientSessionStr = client_session;
            userStr = user;
            privilegeStr = privilege;
        } catch (const std::length_error &e) {
            LOGE("%s", e.what());
            return CYNARA_API_INVALID_PARAM;
        }
        return p_cynara->impl->checkCache(clientStr, clientSessionStr, userStr, privilegeStr);
    });
}

CYNARA_API
int cynara_async_create_request(cynara_async *p_cynara, const char *client,
                                const char *client_session, const char *user, const char *privilege,
                                cynara_check_id *p_check_id, cynara_response_callback callback,
                                void *user_response_data) {
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!client || !client_session || !user || !privilege)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        std::string clientStr;
        std::string clientSessionStr;
        std::string userStr;
        std::string privilegeStr;

        try {
            clientStr = client;
            clientSessionStr = client_session;
            userStr = user;
            privilegeStr = privilege;
        } catch (const std::length_error &e) {
            LOGE("%s", e.what());
            return CYNARA_API_INVALID_PARAM;
        }
        cynara_check_id checkId;
        int ret = p_cynara->impl->createRequest(clientStr, clientSessionStr, userStr, privilegeStr,
                                                checkId, callback, user_response_data);
        if (p_check_id && ret == CYNARA_API_SUCCESS)
            *p_check_id = checkId;
        return ret;
    });
}

CYNARA_API
int cynara_async_process(cynara_async *p_cynara) {
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        return p_cynara->impl->process();
    });
}

CYNARA_API
int cynara_async_cancel_request(cynara_async *p_cynara, cynara_check_id check_id) {
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        return p_cynara->impl->cancelRequest(check_id);
    });
}
