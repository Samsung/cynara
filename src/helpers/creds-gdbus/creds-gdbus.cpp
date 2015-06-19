/*
 *  Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/helpers/creds-gdbus/creds-gdbus.cpp
 * @author      Jacek Bukarewicz <j.bukarewicz@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-creds-gdbus API
 */

#include <attributes/attributes.h>
#include <cstring>

#include <cynara-creds-commons.h>
#include <cynara-creds-gdbus.h>
#include <cynara-error.h>

namespace {

struct Credentials
{
    Credentials() : m_pid(0), m_isPidSet(false), m_uid(0), m_isUidSet(false),
                    m_securityLabel(nullptr), m_isSecuritySet(false) {

    }
    int init(GDBusConnection *connection, const gchar *uniqueId) {
        GVariant *reply = g_dbus_connection_call_sync(connection,
                            "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
                            "GetConnectionCredentials", g_variant_new("(s)", uniqueId),
                            G_VARIANT_TYPE("(a{sv})"), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL);

        if (reply == nullptr)
            return CYNARA_API_UNKNOWN_ERROR;

        GVariantIter *iter;
        gchar *key;
        GVariant *value;
        g_variant_get(reply, "(a{sv})", &iter);

        while (g_variant_iter_next(iter, "{&sv}", &key, &value)) {
            if (!strcmp(key, "ProcessID")) {
                m_pid = g_variant_get_uint32(value);
                m_isPidSet = true;
            } else if (!strcmp(key, "UnixUserID")) {
                m_uid = g_variant_get_uint32(value);
                m_isUidSet = true;
            } else if (!strcmp(key, "LinuxSecurityLabel")) {
                gsize n_elements;
                const gchar *label = static_cast<const gchar *>(g_variant_get_fixed_array(value,
                        &n_elements, 1));
                if (label != nullptr) {
                    m_securityLabel = g_strdup(label);
                    m_isSecuritySet = true;
                }
            }
            g_variant_unref(value);
        }
        g_variant_iter_free(iter);
        g_variant_unref(reply);
        return CYNARA_API_SUCCESS;
    }
    ~Credentials() {
        if (m_isSecuritySet)
            g_free(m_securityLabel);
    }

    bool isPidSet(void) const { return m_isPidSet; }
    bool isUidSet(void) const { return m_isUidSet; }
    bool isSecurityLabelSet(void) const { return m_isSecuritySet; }

    guint32 m_pid;
    bool m_isPidSet;
    guint32 m_uid;
    bool m_isUidSet;
    gchar *m_securityLabel;
    bool m_isSecuritySet;
};

}

CYNARA_API
int cynara_creds_gdbus_get_client(GDBusConnection *connection, const gchar *uniqueName,
                                  enum cynara_client_creds method, gchar **client) {
    int ret;

    if (connection == nullptr || uniqueName == nullptr || client == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (method == cynara_client_creds::CLIENT_METHOD_DEFAULT) {
        int ret = cynara_creds_get_default_client_method(&method);
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
            *client = g_strdup(credentials.m_securityLabel);
            break;
        case cynara_client_creds::CLIENT_METHOD_PID:
            if (!credentials.isPidSet())
                return CYNARA_API_UNKNOWN_ERROR;

            *client = g_strdup_printf("%" G_GUINT32_FORMAT, credentials.m_pid);
            break;
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
    return ret;
}

CYNARA_API
int cynara_creds_gdbus_get_user(GDBusConnection *connection, const gchar *uniqueName,
                                enum cynara_user_creds method, gchar **user) {
    int ret;

    if (connection == nullptr || uniqueName == nullptr || user == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (method == cynara_user_creds::USER_METHOD_DEFAULT) {
        ret = cynara_creds_get_default_user_method(&method);
        if (ret != CYNARA_API_SUCCESS)
            return ret;
    }

    if (method != cynara_user_creds::USER_METHOD_UID)
        return CYNARA_API_METHOD_NOT_SUPPORTED;

    Credentials credentials;
    ret = credentials.init(connection, uniqueName);
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    if (!credentials.isUidSet())
        return CYNARA_API_UNKNOWN_ERROR;

    *user = g_strdup_printf("%" G_GUINT32_FORMAT, credentials.m_uid);

    return ret;
}

CYNARA_API
int cynara_creds_gdbus_get_pid(GDBusConnection *connection, const gchar *uniqueName, pid_t *pid) {
    if (connection == nullptr || uniqueName == nullptr || pid == nullptr)
        return CYNARA_API_INVALID_PARAM;

    Credentials credentials;
    int ret = credentials.init(connection, uniqueName);
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    if (!credentials.isPidSet())
        return CYNARA_API_UNKNOWN_ERROR;

    if (ret == CYNARA_API_SUCCESS)
        *pid = credentials.m_pid;

    return ret;
}
