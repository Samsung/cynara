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
 * @file        src/helpers/creds-dbus/creds-dbus.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-creds-dbus API
 */


#include <attributes/attributes.h>

#include <creds-dbus-inner.h>

#include <cynara-client-error.h>
#include <cynara-creds-commons.h>
#include <cynara-creds-dbus.h>

CYNARA_API
int cynara_creds_dbus_get_client(DBusConnection *connection, const char *uniqueName,
                                 enum cynara_client_creds method, char **client) {
    if (connection == nullptr || uniqueName == nullptr || client == nullptr)
        return CYNARA_API_INVALID_PARAM;

    switch (method) {
        case cynara_client_creds::CLIENT_METHOD_SMACK:
            return getClientSmackLabel(connection, uniqueName, client);
        case cynara_client_creds::CLIENT_METHOD_PID:
            return getClientPid(connection, uniqueName, client);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

CYNARA_API
int cynara_creds_dbus_get_user(DBusConnection *connection, const char *uniqueName,
                               enum cynara_user_creds method, char **user) {
    if (connection == nullptr || uniqueName == nullptr || user == nullptr)
        return CYNARA_API_INVALID_PARAM;

    switch (method) {
        case cynara_user_creds::USER_METHOD_UID:
            return getUserId(connection, uniqueName, user);
        case cynara_user_creds::USER_METHOD_GID:
            return getUserGid(connection, uniqueName, user);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

CYNARA_API
int cynara_creds_dbus_get_pid(DBusConnection *connection, const char *uniqueName, pid_t *pid) {
    if (connection == nullptr || uniqueName == nullptr)
        return CYNARA_API_INVALID_PARAM;

    return getPid(connection, uniqueName, pid);
}
