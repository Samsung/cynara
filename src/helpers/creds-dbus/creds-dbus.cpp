/*
 *  Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @author      Dariusz Michaluk <d.michaluk@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-creds-dbus API
 */


#include <attributes/attributes.h>
#include <string>
#include <string.h>

#include <cynara-creds-commons.h>
#include <cynara-creds-dbus.h>
#include <cynara-error.h>

namespace {

const char dbusName[] = "org.freedesktop.DBus";
const char dbusObject[] = "/org/freedesktop/DBus";
const char dbusInterface[] = "org.freedesktop.DBus";
const char dbusMethod[] = "GetConnectionCredentials";

struct Credentials
{
    Credentials() : m_pid(0), m_isPidSet(false), m_uid(0), m_isUidSet(false),
                    m_securityLabel(nullptr), m_isSecuritySet(false) {

    }
    int init(DBusConnection *connection, const char *uniqueId) {
        DBusMessageIter args_iter, arr_iter, pair_iter, var_iter;

        m_message = dbus_message_new_method_call(dbusName, dbusObject, dbusInterface, dbusMethod);
        if (m_message == nullptr)
            return CYNARA_API_OUT_OF_MEMORY;

        if (!dbus_message_append_args(m_message, DBUS_TYPE_STRING, &uniqueId, DBUS_TYPE_INVALID))
            return CYNARA_API_UNKNOWN_ERROR;

        m_reply = dbus_connection_send_with_reply_and_block(connection, m_message,
                                                            DBUS_TIMEOUT_USE_DEFAULT, nullptr);
        if (m_reply == nullptr)
            return CYNARA_API_UNKNOWN_ERROR;

        if (!dbus_message_iter_init(m_reply, &args_iter) ||
            dbus_message_iter_get_arg_type(&args_iter) != DBUS_TYPE_ARRAY ||
            dbus_message_iter_get_element_type(&args_iter) != DBUS_TYPE_DICT_ENTRY)
            return CYNARA_API_UNKNOWN_ERROR;

        dbus_message_iter_recurse(&args_iter, &arr_iter);

        while (dbus_message_iter_get_arg_type(&arr_iter) != DBUS_TYPE_INVALID) {
            char *key;

            dbus_message_iter_recurse(&arr_iter, &pair_iter);
            if (dbus_message_iter_get_arg_type(&pair_iter) != DBUS_TYPE_STRING)
                    return CYNARA_API_UNKNOWN_ERROR;

            dbus_message_iter_get_basic(&pair_iter, &key);

            dbus_message_iter_next(&pair_iter);
            if (dbus_message_iter_get_arg_type(&pair_iter) != DBUS_TYPE_VARIANT)
                    return CYNARA_API_UNKNOWN_ERROR;

            dbus_message_iter_recurse(&pair_iter, &var_iter);
            if (!strcmp(key, "ProcessID")) {
                if (dbus_message_iter_get_arg_type(&var_iter) != DBUS_TYPE_UINT32)
                    return CYNARA_API_UNKNOWN_ERROR;

                dbus_message_iter_get_basic(&var_iter, &m_pid);
                m_isPidSet = true;
            } else if (!strcmp(key, "UnixUserID")) {
                if (dbus_message_iter_get_arg_type(&var_iter) != DBUS_TYPE_UINT32)
                    return CYNARA_API_UNKNOWN_ERROR;

                dbus_message_iter_get_basic(&var_iter, &m_uid);
                m_isUidSet = true;
            } else if (!strcmp(key, "LinuxSecurityLabel")) {
                DBusMessageIter ay_iter;
                char *label;
                int len;

                if (dbus_message_iter_get_arg_type(&var_iter) != DBUS_TYPE_ARRAY)
                    return CYNARA_API_UNKNOWN_ERROR;

                dbus_message_iter_recurse(&var_iter, &ay_iter);
                if (dbus_message_iter_get_arg_type(&ay_iter) != DBUS_TYPE_BYTE)
                    return CYNARA_API_UNKNOWN_ERROR;

                dbus_message_iter_get_fixed_array(&ay_iter, &label, &len);
                if (label != nullptr) {
                    m_securityLabel = strdup(label);
                    if (m_securityLabel == nullptr)
                        return CYNARA_API_OUT_OF_MEMORY;
                    m_isSecuritySet = true;
                }
            }
            dbus_message_iter_next(&arr_iter);
        }
        return CYNARA_API_SUCCESS;
    }
    ~Credentials() {
        free(m_securityLabel);
        dbus_message_unref(m_message);
        dbus_message_unref(m_reply);
    }

    bool isPidSet(void) const { return m_isPidSet; }
    bool isUidSet(void) const { return m_isUidSet; }
    bool isSecurityLabelSet(void) const { return m_isSecuritySet; }

    dbus_uint32_t m_pid;
    bool m_isPidSet;
    dbus_uint32_t m_uid;
    bool m_isUidSet;
    char *m_securityLabel;
    bool m_isSecuritySet;

private:
    DBusMessage *m_message = nullptr;
    DBusMessage *m_reply = nullptr;
};
} // namespace anonymous

CYNARA_API
int cynara_creds_dbus_get_client(DBusConnection *connection, const char *uniqueName,
                                 enum cynara_client_creds method, char **client) {
    int ret;

    if (connection == nullptr || uniqueName == nullptr || client == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (method == cynara_client_creds::CLIENT_METHOD_DEFAULT) {
        ret = cynara_creds_get_default_client_method(&method);
        if (ret != CYNARA_API_SUCCESS)
            return ret;
    }

    Credentials credentials;
    ret = credentials.init(connection, uniqueName);
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    switch (method) {
        case cynara_client_creds::CLIENT_METHOD_SMACK:
            if (!credentials.isSecurityLabelSet())
                return CYNARA_API_UNKNOWN_ERROR;
            *client = strdup(credentials.m_securityLabel);
            if (*client == nullptr)
                return CYNARA_API_OUT_OF_MEMORY;
            break;
        case cynara_client_creds::CLIENT_METHOD_PID:
            if (!credentials.isPidSet())
                return CYNARA_API_UNKNOWN_ERROR;
            *client = strdup(std::to_string(credentials.m_pid).c_str());
            if (*client == nullptr)
                return CYNARA_API_OUT_OF_MEMORY;
            break;
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
    return ret;
}

CYNARA_API
int cynara_creds_dbus_get_user(DBusConnection *connection, const char *uniqueName,
                               enum cynara_user_creds method, char **user) {
    int ret;

    if (connection == nullptr || uniqueName == nullptr || user == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (method == cynara_user_creds::USER_METHOD_DEFAULT) {
        ret = cynara_creds_get_default_user_method(&method);
        if (ret != CYNARA_API_SUCCESS)
            return ret;
    }

    Credentials credentials;
    ret = credentials.init(connection, uniqueName);
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    switch (method) {
        case cynara_user_creds::USER_METHOD_UID:
            if (!credentials.isUidSet())
                return CYNARA_API_UNKNOWN_ERROR;
            *user = strdup(std::to_string(credentials.m_uid).c_str());
            if (*user == nullptr)
                return CYNARA_API_OUT_OF_MEMORY;
            break;
        case cynara_user_creds::USER_METHOD_GID:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
    return ret;
}

CYNARA_API
int cynara_creds_dbus_get_pid(DBusConnection *connection, const char *uniqueName, pid_t *pid) {
    if (connection == nullptr || uniqueName == nullptr || pid == nullptr)
        return CYNARA_API_INVALID_PARAM;

    Credentials credentials;
    int ret = credentials.init(connection, uniqueName);
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    if (!credentials.isPidSet())
        return CYNARA_API_UNKNOWN_ERROR;

    *pid = credentials.m_pid;

    return ret;
}
