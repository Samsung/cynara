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
 * @file        src/helpers/creds-dbus/creds-dbus-inner.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of internal libcynara-creds-dbus functions
 */

#include <attributes/attributes.h>
#include <string>
#include <string.h>

#include <cynara-error.h>

#include "creds-dbus-inner.h"

// TODO: Move this class somewhere else
// TODO: Find a better name for this class
// TODO: Introduce std::exception instead of throwing ints
// TODO: Make this class more general
// TODO: Write tests for this class
class DBusMethod {
public:
    typedef int ArgType;
    DBusMethod(DBusConnection *connection, const std::string &method) : m_connection(connection) {
        if (connection == nullptr)
            throw CYNARA_API_INVALID_PARAM;

        m_message = dbus_message_new_method_call(m_dbusName, m_dbusObject,
                                                 m_dbusInterface, method.c_str());

        if (m_message == nullptr)
            throw CYNARA_API_OUT_OF_MEMORY;
    }

    ~DBusMethod() {
        if (m_message != nullptr)
            dbus_message_unref(m_message);

        delete m_argsIter;
    }

    void appendArg(ArgType type, void *value) {
        if (dbus_message_append_args(m_message, type, value, DBUS_TYPE_INVALID) == FALSE)
            throw CYNARA_API_UNKNOWN_ERROR;
    }

    void getArgPtr(ArgType type, void *value) {
        if (m_argsIter == nullptr)
            throw CYNARA_API_UNKNOWN_ERROR;

        if (dbus_message_iter_get_arg_type(m_argsIter) != type)
            throw CYNARA_API_UNKNOWN_ERROR;

        dbus_message_iter_get_basic(m_argsIter, value);
        dbus_message_iter_next(m_argsIter);
    }

    DBusMethod send(void) {
        DBusPendingCall *reply = nullptr;
        auto ret = dbus_connection_send_with_reply(m_connection, m_message, &reply,
                                                   DBUS_TIMEOUT_USE_DEFAULT);
        if (ret == FALSE)
            throw CYNARA_API_OUT_OF_MEMORY;

        if (reply == nullptr)
            throw CYNARA_API_INVALID_PARAM;

        dbus_connection_flush(m_connection);
        dbus_pending_call_block(reply);

        DBusMessage *replyMsg = dbus_pending_call_steal_reply(reply);
        if (replyMsg == nullptr)
            throw CYNARA_API_UNKNOWN_ERROR;

        return { m_connection, replyMsg };
    }

private:
    DBusMethod(DBusConnection *connection, DBusMessage *message)
        : m_connection(connection), m_message(message), m_argsIter(new DBusMessageIter()) {

        if (dbus_message_iter_init(m_message, m_argsIter) == FALSE)
            throw CYNARA_API_UNKNOWN_ERROR;
    }

    DBusConnection *m_connection = nullptr;
    DBusMessage *m_message = nullptr;
    DBusMessageIter *m_argsIter = nullptr;

    static const char m_dbusName[];
    static const char m_dbusObject[];
    static const char m_dbusInterface[];
};

const char DBusMethod::m_dbusName[] = "org.freedesktop.DBus";
const char DBusMethod::m_dbusObject[] = "/org/freedesktop/DBus";
const char DBusMethod::m_dbusInterface[] = "org.freedesktop.DBus";


int getIdFromConnection(DBusConnection *connection, const char *uniqueName,
                        const std::string &dbusMethod, unsigned int *id) {

    if (uniqueName == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (dbusMethod != "GetConnectionUnixUser" && dbusMethod != "GetConnectionUnixProcessID")
        return CYNARA_API_INVALID_PARAM;

    try {
        DBusMethod call(connection, dbusMethod);
        call.appendArg(DBUS_TYPE_STRING, &uniqueName);
        auto reply = call.send();
        reply.getArgPtr(DBUS_TYPE_UINT32, id);
    } catch (int apiError) {
        return apiError;
    }

    return CYNARA_API_SUCCESS;
}

int getClientSmackLabel(DBusConnection *connection, const char *uniqueName, char **client) {
    if (uniqueName == nullptr)
        return CYNARA_API_INVALID_PARAM;

    try {
        DBusMethod call(connection, "GetConnectionSmackContext");
        call.appendArg(DBUS_TYPE_STRING, &uniqueName);
        auto reply = call.send();
        char *label;
        reply.getArgPtr(DBUS_TYPE_STRING, &label);
        *client = strdup(label);
        if (*client == nullptr)
          return CYNARA_API_OUT_OF_MEMORY;
    } catch (int apiError) {
        return apiError;
    }

    return CYNARA_API_SUCCESS;
}

int getUint32(DBusConnection *connection, const char *uniqueName, const char *method,
              char **value) {
    unsigned int dbusValue;
    int ret = getIdFromConnection(connection, uniqueName, method, &dbusValue);

    if (ret != CYNARA_API_SUCCESS)
        return ret;

    *value = strdup(std::to_string(dbusValue).c_str());

    if (*value == nullptr)
      return CYNARA_API_OUT_OF_MEMORY;

    return CYNARA_API_SUCCESS;
}

int getClientPid(DBusConnection *connection, const char *uniqueName, char **client) {
    return getUint32(connection, uniqueName, "GetConnectionUnixProcessID", client);
}

int getUserId(DBusConnection *connection, const char *uniqueName, char **user) {
    return getUint32(connection, uniqueName, "GetConnectionUnixUser", user);
}

int getUserGid(DBusConnection *connection UNUSED, const char *uniqueName UNUSED,
               char **user UNUSED) {
    //todo
    return CYNARA_API_METHOD_NOT_SUPPORTED;
}

int getPid(DBusConnection *connection, const char *uniqueName, pid_t *pid) {
    unsigned int _pid;
    auto ret = getIdFromConnection(connection, uniqueName, "GetConnectionUnixProcessID", &_pid);

    if (ret == CYNARA_API_SUCCESS)
        *pid = _pid;

    return ret;
}
