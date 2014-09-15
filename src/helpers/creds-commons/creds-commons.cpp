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

#include <cynara-client-error.h>
#include <cynara-creds-commons.h>

CYNARA_API
int cynara_creds_get_default_client_method(enum cynara_client_creds *method) {
    //todo read from proper file and parse

    *method = CLIENT_METHOD_SMACK;
    return CYNARA_API_SUCCESS;
}

CYNARA_API
int cynara_creds_get_default_user_method(enum cynara_user_creds *method) {
    //todo read from proper file and parse

    *method = USER_METHOD_UID;
    return CYNARA_API_SUCCESS;
}
