/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        client-api.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-client API
 */

#include <new>

#include <common.h>

#include <log/log.h>
#include <system/signals.h>

#include <cynara-client.h>
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

CYNARA_API
int cynara_initialize(cynara **pp_cynara, const cynara_configuration *p_conf UNUSED)
{
    if (!pp_cynara)
        return CYNARA_API_INVALID_PARAM;

    try {
        *pp_cynara = new cynara(new Cynara::Logic);
    } catch (const std::bad_alloc &ex) {
        return CYNARA_API_OUT_OF_MEMORY;
    }

    init_log();

    ignore_sigpipe();

    LOGD("Cynara client initialized");

    return CYNARA_API_SUCCESS;
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

    return p_cynara->impl->check(client, client_session, user, privilege);
}
