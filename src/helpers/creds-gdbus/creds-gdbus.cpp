/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <cynara-creds-commons.h>
#include <cynara-creds-gdbus.h>
#include <cynara-error.h>

namespace {
int call_dbus_daemon_method_str(GDBusConnection *connection, const gchar *methodName,
                                const gchar *arg, gchar **result) {
    GVariant *reply = g_dbus_connection_call_sync(connection,
                "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
                methodName, g_variant_new("(s)", arg), G_VARIANT_TYPE("(s)"),
                G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL);

    if (reply != NULL) {
        g_variant_get(reply, "(s)", result);
        g_variant_unref(reply);
        return CYNARA_API_SUCCESS;
    } else {
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

int call_dbus_daemon_method_u32(GDBusConnection *connection, const gchar *methodName,
                                const gchar *arg, guint32 *result) {
    GVariant *reply = g_dbus_connection_call_sync(connection,
                "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
                methodName, g_variant_new("(s)", arg), G_VARIANT_TYPE("(u)"),
                G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL);

    if (reply != NULL) {
        g_variant_get(reply, "(u)", result);
        g_variant_unref(reply);
        return CYNARA_API_SUCCESS;
    } else {
        return CYNARA_API_UNKNOWN_ERROR;
    }
}
}

CYNARA_API
int cynara_creds_gdbus_get_client(GDBusConnection *connection, const gchar *uniqueName,
                                  enum cynara_client_creds method, gchar **client) {
    int ret;

    if (connection == nullptr || uniqueName == nullptr || client == nullptr)
        return CYNARA_API_INVALID_PARAM;

    switch (method) {
        case cynara_client_creds::CLIENT_METHOD_SMACK:
            ret = call_dbus_daemon_method_str(connection, "GetConnectionSmackContext", uniqueName,
                      client);
            break;
        case cynara_client_creds::CLIENT_METHOD_PID:
        {
            guint32 pid;
            ret = call_dbus_daemon_method_u32(connection, "GetConnectionUnixProcessID",
                                              uniqueName, &pid);
            if (ret == CYNARA_API_SUCCESS)
                *client = g_strdup_printf("%u", pid);
            break;
        }
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
    return ret;
}

CYNARA_API
int cynara_creds_gdbus_get_user(GDBusConnection *connection, const gchar *uniqueName,
                                enum cynara_user_creds method, gchar **user) {
    if (connection == nullptr || uniqueName == nullptr || user == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (method != cynara_user_creds::USER_METHOD_UID)
        return CYNARA_API_METHOD_NOT_SUPPORTED;

    guint32 uid;
    int ret = call_dbus_daemon_method_u32(connection, "GetConnectionUnixUser", uniqueName, &uid);
    if (ret == CYNARA_API_SUCCESS) {
        *user = g_strdup_printf("%u", uid);
    }

    return ret;
}

CYNARA_API
int cynara_creds_gdbus_get_pid(GDBusConnection *connection, const char *uniqueName, pid_t *pid) {
    if (connection == nullptr || uniqueName == nullptr || pid == nullptr)
        return CYNARA_API_INVALID_PARAM;

    guint32 pidU32;
    int ret = call_dbus_daemon_method_u32(connection, "GetConnectionUnixProcessID", uniqueName,
                                          &pidU32);
    if (ret == CYNARA_API_SUCCESS) {
        *pid = static_cast<pid_t>(pidU32);
    }

    return ret;
}
