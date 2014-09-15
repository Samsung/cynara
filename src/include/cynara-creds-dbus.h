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
 * @file        src/include/cynara-creds-dbus.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       This file contains Cynara credentials helper APIs for dbus clients.
 */


#ifndef CYNARA_CREDS_DBUS_H
#define CYNARA_CREDS_DBUS_H

#include <dbus/dbus.h>
#include <sys/types.h>

#include "cynara-creds-commons.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \par Description:
 * Creates a client identification string with given method. Client is a process identified by the
 * unique name at the other side of the dbus connection.
 *
 * \par Purpose:
 * Client identification string is required for cynara_check() and cynara_async_check() functions.
 *
 * \par Typical use case:
 * The function is called before the call of one of ...check() functions.
 * Returned string is used as client parameter in ...check() function.
 * String is released with free() function when it is no longer needed.
 *
 * \par Method of function operation:
 * The function generates client string by calling a method from DBus Interface
 * ("org.freedesktop.DBus") which is placed on system bus ("org.freedesktop.DBus").
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * Memory for returned client string is obtained with malloc(), and should be freed with free().
 * Allocated string is returned only, when function succeeds.
 *
 * \param[in] connection DBus connection to a bus. It manages incomming and outgoing messages
 * \param[in] uniqueName DBus identifier of the client
 * \param[in] method Method of client identifier creation
 * \param[out] client Placeholder for allocated string containing client id
 *
 * \return CYNARA_API_SUCCESS on success
 *         CYNARA_API_INVALID_PARAM when client is NULL or uniqueName or client has wrong
 *                                  value (i.e NULL or non-existing)
 *         CYNARA_API_METHOD_NOT_SUPPORTED when requested method is not supported
 *         CYNARA_API_OUT_OF_MEMORY when there was error allocating memory
 */
int cynara_creds_dbus_get_client(DBusConnection *connection, const char *uniqueName,
                                 enum cynara_client_creds method, char **client);

/**
 * \par Description:
 * Creates a user identification string with given method. User is an executor of process
 * at the other side of socket.
 *
 * \par Purpose:
 * User identification string is required for cynara_check() and cynara_async_check() functions.
 *
 * \par Typical use case:
 * The function is called before the call of one of ...check() functions.
 * Returned string is used as user parameter in ...check() function.
 * String is released with free() function when it is no longer needed.
 *
 * \par Method of function operation:
 * The function generates user string by calling a method from DBus Interface
 * ("org.freedesktop.DBus") which is placed on system bus ("org.freedesktop.DBus").
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * Memory for returned user string is obtained with malloc(), and should be freed with free().
 * Allocated string is returned only, when function succeeds.
 *
 * \param[in] connection DBus connection to a bus. It manages incomming and outgoing messages
 * \param[in] uniqueName DBus identifier of the client invoked by the user
 * \param[in] method Method of client identifier creation
 * \param[out] user Placeholder for allocated string containing user id
 *
 * \return CYNARA_API_SUCCESS on success
 *         CYNARA_API_INVALID_PARAM when user is NULL or connection is not valid DBus connection or
 *                                  uniqueName does not represent a process conected to the DBus
 *         CYNARA_API_METHOD_NOT_SUPPORTED when requested method is not supported
 *         CYNARA_API_OUT_OF_MEMORY when there was error allocating memory
 */
int cynara_creds_dbus_get_user(DBusConnection *connection, const char *uniqueName,
                               enum cynara_user_creds method, char **user);

/**
 * \par Description:
 * Return PID of a proces identified by the unique name at the other side of the dbus connection.
 *
 * \par Purpose:
 * PID may be used for client_session creation with cynara_helper_session_from_pid() function
 * from libcynara-helper-session library. Client_session is needed for cynara_check()
 * and cynara_async_check() functions.
 *
 * \par Typical use case:
 * The function is called before the call of cynara_helper_session_from_pid() function.
 *
 * \par Method of function operation:
 * The function reads PID of the peer by calling a method from DBus Interface
 * ("org.freedesktop.DBus") which is placed on system bus ("org.freedesktop.DBus")
 * with "GetConnectionUnixProcessID" argument.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \param[in] connection DBus connection to a bus. It manages incomming and outgoing messages
 * \param[in] uniqueName DBus identifier of the client invoked by the user
 * \param[out] pid Placeholder for PID returned by function
 *
 * \return CYNARA_API_SUCCESS on success
 *         CYNARA_API_INVALID_PARAM when socket_fd is not valid connected socket descriptor
 *         CYNARA_API_UNKNOWN_ERROR when system function fails in incredible situation
 *         CYNARA_API_OUT_OF_MEMORY when there was error allocating memory
 */
int cynara_creds_dbus_get_pid(DBusConnection *connection, const char *uniqueName, pid_t *pid);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CREDS_DBUS_H */
