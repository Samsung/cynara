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
 * @file        src/client/api/client-api.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-client API
 */

#include <new>

#include <common.h>

#include <exceptions/TryCatch.h>
#include <log/log.h>

#include <configuration/Configuration.h>
#include <cynara-client.h>
#include <cynara-error.h>
#include <api/ApiInterface.h>
#include <logic/Logic.h>

struct cynara {
    Cynara::ApiInterface *impl;

    cynara(Cynara::ApiInterface *_impl) : impl(_impl) {
    }
    ~cynara() {
        delete impl;
    }
};

struct cynara_configuration {
    Cynara::Configuration* impl;

    cynara_configuration(Cynara::Configuration *_impl) : impl(_impl) {}

    ~cynara_configuration() {
        delete impl;
    }
};

CYNARA_API
int cynara_configuration_create(cynara_configuration **pp_conf) {
    if (!pp_conf)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        Cynara::ConfigurationUniquePtr ptr(new Cynara::Configuration());
        *pp_conf = new cynara_configuration(ptr.get());
        ptr.release();
        LOGD("Cynara configuration initialized");
        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
void cynara_configuration_destroy(cynara_configuration *p_conf) {
    delete p_conf;
}

CYNARA_API
int cynara_configuration_set_cache_size(cynara_configuration *p_conf, size_t cache_size) {
    if (!p_conf || !p_conf->impl)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        p_conf->impl->setCacheSize(cache_size);
        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_initialize(cynara **pp_cynara, const cynara_configuration *p_conf)
{
    if (!pp_cynara)
        return CYNARA_API_INVALID_PARAM;

    init_log();

    return Cynara::tryCatch([&]() {
        if (p_conf && p_conf->impl)
            *pp_cynara = new cynara(new Cynara::Logic(*(p_conf->impl)));
        else
            *pp_cynara = new cynara(new Cynara::Logic());

        LOGD("Cynara client initialized");

        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_finish(cynara *p_cynara)
{
    delete p_cynara;

    return CYNARA_API_SUCCESS;
}

CYNARA_API
int cynara_check(cynara *p_cynara, const char *client, const char *client_session, const char *user,
    const char *privilege)
{
    if(!p_cynara || !p_cynara->impl)
        return CYNARA_API_INVALID_PARAM;
    if(!client || !client_session || !user || !privilege)
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
        return p_cynara->impl->check(clientStr, clientSessionStr, userStr, privilegeStr);
    });
}
