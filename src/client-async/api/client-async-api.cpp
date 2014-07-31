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
 * @file        client-async-api.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-client-async API
 */

#include <new>

#include <common.h>
#include <log/log.h>

#include <api/ApiInterface.h>
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

CYNARA_API
int cynara_async_initialize(cynara_async **pp_cynara,
                            const cynara_async_configuration *p_conf UNUSED)
{
    if (!pp_cynara)
        return CYNARA_ASYNC_API_INVALID_PARAM;

    try {
        *pp_cynara = new cynara_async(new Cynara::Logic);
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ASYNC_API_OUT_OF_MEMORY;
    }

    init_log();

    LOGD("Cynara client async initialized");

    return CYNARA_ASYNC_API_SUCCESS;
}

CYNARA_API
int cynara_async_finish(cynara_async *p_cynara)
{
    delete p_cynara;

    return CYNARA_ASYNC_API_SUCCESS;
}

CYNARA_API
int cynara_async_connect(cynara_async *p_cynara, int *p_sock_fd)
{
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_ASYNC_API_INVALID_PARAM;
    if (!p_sock_fd)
        return CYNARA_ASYNC_API_INVALID_PARAM;

    return p_cynara->impl->connect(*p_sock_fd);
}

CYNARA_API
int cynara_async_check(cynara_async *p_cynara,
                       const char *client, const char *client_session,
                       const char *user, const char *privilege,
                       cynara_check_id *p_check_id)
{
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_ASYNC_API_INVALID_PARAM;
    if (!client || !client_session || !user || !privilege)
        return CYNARA_ASYNC_API_INVALID_PARAM;
    if (!p_check_id)
        return CYNARA_ASYNC_API_INVALID_PARAM;

    return p_cynara->impl->check(client, client_session,
                                 user, privilege,
                                 *p_check_id);
}

CYNARA_API
int cynara_async_receive(cynara_async *p_cynara, cynara_check_id *p_check_id)
{
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_ASYNC_API_INVALID_PARAM;
    if (!p_check_id)
        return CYNARA_ASYNC_API_INVALID_PARAM;

    return p_cynara->impl->receive(*p_check_id);
}

CYNARA_API
int cynara_async_cancel(cynara_async *p_cynara, const cynara_check_id check_id)
{
    if (!p_cynara || !p_cynara->impl)
        return CYNARA_ASYNC_API_INVALID_PARAM;

    return p_cynara->impl->cancel(check_id);
}
