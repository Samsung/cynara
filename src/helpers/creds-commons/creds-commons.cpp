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
/**
 * @file        src/helpers/creds-commons/creds-commons.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-creds-commons API
 */

#include <attributes/attributes.h>
#include <exceptions/TryCatch.h>

#include <cynara-creds-commons.h>
#include <cynara-error.h>

#include "CredsCommonsInner.h"

CYNARA_API
int cynara_creds_get_default_client_method(enum cynara_client_creds *method) {
    return Cynara::tryCatch([&] () {
        int methodCode, ret;
        static const Cynara::CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                                           {"pid", CLIENT_METHOD_PID}};

        if ((ret = Cynara::CredsCommonsInnerBackend::
                           getMethodFromConfigurationFile(clientCredsMap,
                                                          "client_default",
                                                          methodCode))
            != CYNARA_API_SUCCESS)
            return ret;

        *method = static_cast<enum cynara_client_creds>(methodCode);
        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_creds_get_default_user_method(enum cynara_user_creds *method) {
     return Cynara::tryCatch([&] () {
        int methodCode, ret;
        static const Cynara::CredentialsMap userCredsMap{{"uid", USER_METHOD_UID},
                                                         {"gid", USER_METHOD_GID}};
        if ((ret = Cynara::CredsCommonsInnerBackend::
                           getMethodFromConfigurationFile(userCredsMap,
                                                          "user_default",
                                                          methodCode))
            != CYNARA_API_SUCCESS)
            return ret;

        *method = static_cast<enum cynara_user_creds>(methodCode);
        return CYNARA_API_SUCCESS;
    });
}
