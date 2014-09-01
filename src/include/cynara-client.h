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
 * @file        cynara-client.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains client APIs of Cynara available with libcynara-client.
 */

#ifndef CYNARA_CLIENT_H
#define CYNARA_CLIENT_H

#include <cynara-client-error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cynara cynara;
typedef struct cynara_configuration cynara_configuration;


/**
 * \par Description:
 * Initialize cynara-client library with given configuration.
 * Create structured used in following API calls.
 *
 * \par Purpose:
 * This API must be used by prior calling cynara_check function.
 *
 * \par Typical use case:
 * Once before a service can call cynara_check.
 *
 * \par Method of function operation:
 * This API initializes inner library structures [TODO describe more details] and in case of success
 * creates and returns cynara structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * Structure cynara created by cynara_initialize call should be released with cynara_finish.
 *
 * \param[out] pp_cynara Place holder for created cynara structure.
 * \param[in] p_conf Configuration for cynara-client library. NULL for default parameters.
 * [TODO define and describe functions for custom parameters].
 *
 * \return CYNARA_API_SUCCESS on success, or error code on error.
 */
int cynara_initialize(cynara **pp_cynara, const cynara_configuration *p_conf);

/**
 * \par Description:
 * Release cynara-client library and destroy structure created with cynara_initialize.
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-client library.
 *
 * \par Typical use case:
 * Once after last call to cynara_check.
 *
 * \par Method of function operation:
 * This API initializes releases inner library structures [TODO describe more details]
 * and destroys cynara structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * No other call to libcynara-client should be made after call to cynara_finish.
 *
 * \param[in] p_cynara Cynara structure.
 *
 * \return CYNARA_API_SUCCESS on success, or error code on error.
 */
int cynara_finish(cynara *p_cynara);

/**
 * \par Description:
 * Check client, user access for given privilege.
 *
 * \par Purpose:
 * This API should be used to check if a user running application identified as client
 * has access to a privilege.
 *
 * \par Typical use case:
 * A service want to ask trusted process (Cynara), if a client demanding access to some privilege
 * has proper rights.
 *
 * \par Method of function operation:
 * Client (a process / application) demanding access to a privilege is running as some user.
 * For such triple an access to a privilege is checked by calling cynara.
 * Depending on defined policy, an external application may be launched to ask user a question,
 * e.g. if [s]he wants to allow client to use a privilege. Additional parameter client_session
 * may be used to distinguish between client session (e.g. for allowing access only for this
 * particular application launch).
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * An external application may be launched to allow user interaction in granting or denying access.
 * Call to cynara_check needs cynara structure to be created first with call to cynara_initialize.
 *
 * \param[in] p_cynara Cynara structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Session of client (connection, launch).
 * \param[in] user User running client.
 * \param[in] privilege Privilege that is a subject of a check..
 *
 * \return CYNARA_API_SUCCESS on success (access granted), CYNARA_API_ACCESS_DENIED on access denial
 * or other error code on error.
 */
int cynara_check(cynara *p_cynara, const char *client, const char *client_session, const char *user,
        const char *privilege);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CLIENT_H */
