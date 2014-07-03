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
 * @file        admin-api.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-admin API
 */

#include <new>

#include <common.h>
#include <cynara-admin.h>

#include <api/ApiInterface.h>
#include <logic/Logic.h>

struct cynara_admin {
    Cynara::ApiInterface *impl;

    cynara_admin(Cynara::ApiInterface *_impl) : impl(_impl) {
    }
    ~cynara_admin() {
        delete impl;
    }
};

CYNARA_API
int cynara_admin_initialize(struct cynara_admin **pp_cynara_admin) {
    if (!pp_cynara_admin)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    try {
        *pp_cynara_admin = new cynara_admin(new Cynara::Logic);
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }

    return CYNARA_ADMIN_API_SUCCESS;
}

CYNARA_API
int cynara_admin_finish(struct cynara_admin *p_cynara_admin) {
    delete p_cynara_admin;

    return CYNARA_ADMIN_API_SUCCESS;
}

CYNARA_API
int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin,
                              const cynara_admin_policy *const *policies) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!policies)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    //todo This is a stub. Parameters should be passed to p_cynara_admin->impl
    return CYNARA_ADMIN_API_SUCCESS;
}

CYNARA_API
int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket,
                            int operation, const char *extra UNUSED) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!bucket)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    switch (operation) {
        case CYNARA_ADMIN_DELETE:
            //todo This is a stub. Parameters should be passed to p_cynara_admin->impl
            return CYNARA_ADMIN_API_SUCCESS;
        case CYNARA_ADMIN_DENY:
        case CYNARA_ADMIN_ALLOW:
            //todo This is a stub. Parameters should be passed to p_cynara_admin->impl
            return CYNARA_ADMIN_API_SUCCESS;
        case CYNARA_ADMIN_BUCKET:
        default:
            return CYNARA_ADMIN_API_INVALID_PARAM;
    }
}
