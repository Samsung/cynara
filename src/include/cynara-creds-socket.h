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
 * @file        cynara-creds-socket.h
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains Cynara credentials helper APIs for socket clients.
 */


#ifndef CYNARA_CREDS_SOCKET_H
#define CYNARA_CREDS_SOCKET_H

#include <sys/types.h>

#include "cynara-creds-commons.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \par Description:
 * Creates a client identification string with given method. Client is a process at the other
 * side of socket.
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
 * The function generates client string using SO_PEERCRED on socket.
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
 * \param[in] socket_fd Descriptor of open connected UNIX socket
 * \param[in] method Method of client identifier creation
 * \param[out] client Placeholder for allocated string containing client id
 *
 * \return CYNARA_API_SUCCESS on success
 *         CYNARA_API_INVALID_PARAM when client is NULL or socket_fd is not valid connected socket
 *                                  descriptor
 *         CYNARA_API_METHOD_NOT_SUPPORTED when requested method is not supported
 *         CYNARA_API_OUT_OF_MEMORY when there was error allocating memory
 */
int cynara_creds_socket_get_client(int socket_fd, enum cynara_client_creds method, char **client);

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
 * The function generates user string using SO_PEERCRED on socket.
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
 * \param[in] socket_fd Descriptor of open connected UNIX socket
 * \param[in] method Method of user identifier creation
 * \param[out] user Placeholder for allocated string containing user id
 *
 * \return CYNARA_API_SUCCESS on success
 *         CYNARA_API_INVALID_PARAM when user is NULL or socket_fd is not valid connected socket
 *                                  descriptor
 *         CYNARA_API_METHOD_NOT_SUPPORTED when requested method is not supported
 *         CYNARA_API_OUT_OF_MEMORY when there was error allocating memory
 */
int cynara_creds_socket_get_user(int socket_fd, enum cynara_user_creds method, char **user);

/**
 * \par Description:
 * Return PID of process at the other side of socket.
 *
 * \par Purpose:
 * PID may be used for client_session creation with cynara_session_from_pid() function
 * from libcynara-session library. Client_session is needed for cynara_check()
 * and cynara_async_check() functions.
 *
 * \par Typical use case:
 * The function is called before the call of cynara_session_from_pid() function.
 *
 * \par Method of function operation:
 * The function reads PID of peer using SO_PEERCRED on socket.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \param[in] socket_fd Descriptor of open connected UNIX socket
 * \param[out] pid Placeholder for pid
 *
 * \return CYNARA_API_SUCCESS on success
 *         CYNARA_API_INVALID_PARAM when socket_fd is not valid connected socket descriptor
 *         CYNARA_API_UNKNOWN_ERROR when system function fails in incredible situation
 */
int cynara_creds_socket_get_pid(int socket_fd, pid_t *pid);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CREDS_SOCKET_H */
