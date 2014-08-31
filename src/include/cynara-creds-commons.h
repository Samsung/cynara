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
 * @file        cynara-creds-commons.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains common APIs for Cynara credentials helper.
 */


#ifndef CYNARA_CREDS_COMMONS_H
#define CYNARA_CREDS_COMMONS_H

enum cynara_client_creds {
    CLIENT_METHOD_SMACK,
    CLIENT_METHOD_PID
};

enum cynara_user_creds {
    USER_METHOD_UID,
    USER_METHOD_GID
};

#ifdef __cplusplus
extern "C" {
#endif

int cynara_creds_get_default_client_method(enum cynara_client_creds *method);

int cynara_creds_get_default_user_method(enum cynara_user_creds *method);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CREDS_COMMONS_H */
