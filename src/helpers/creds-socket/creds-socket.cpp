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
 * @file        src/helpers/creds-socket/creds-socket.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-creds-socket API
 */

#include <sys/socket.h>
#include <sys/types.h>

#include <attributes/attributes.h>

#include <creds-socket-inner.h>

#include <cynara-creds-commons.h>
#include <cynara-creds-socket.h>
#include <cynara-error.h>

CYNARA_API
int cynara_creds_socket_get_client(int socket_fd, enum cynara_client_creds method, char **client) {
    if (client == nullptr)
        return CYNARA_API_INVALID_PARAM;

    switch (method) {
        case cynara_client_creds::CLIENT_METHOD_SMACK:
            return getClientSmackLabel(socket_fd, client);
        case cynara_client_creds::CLIENT_METHOD_PID:
            return getClientPid(socket_fd, client);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

CYNARA_API
int cynara_creds_socket_get_user(int socket_fd, enum cynara_user_creds method, char **user) {
    if (user == nullptr)
        return CYNARA_API_INVALID_PARAM;

    switch (method) {
        case cynara_user_creds::USER_METHOD_UID:
            return getUserId(socket_fd, user);
        case cynara_user_creds::USER_METHOD_GID:
            return getUserGid(socket_fd, user);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

CYNARA_API
int cynara_creds_socket_get_pid(int socket_fd, pid_t *pid) {
    return getPid(socket_fd, pid);
}
