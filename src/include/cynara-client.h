/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/include/cynara-client.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Oskar Switalski <o.switalski@samsung.com>
 * @version     1.0
 * @brief       This file contains client APIs of Cynara available with libcynara-client.
 * @example     cynara-client.example
 */

#ifndef CYNARA_CLIENT_H
#define CYNARA_CLIENT_H

#include <stddef.h>

#include <cynara-error.h>
#include <cynara-limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cynara cynara;
typedef struct cynara_configuration cynara_configuration;

/**
 * \par Description:
 * Initialize cynara_configuration. Create structure used in following configuration
 * API calls.
 *
 * \par Purpose:
 * For configuration parameter to be used in cynara_initialize() function, this API must be
 * called before any other cynara configuration API function.
 * It will create cynara_configuration structure, an optional parameter of cynara initialization.
 *
 * \par Typical use case:
 * Once before setting parameters of cynara configuration and passing to
 * cynara_initialize().
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success returns pointer
 * to created cynara_configuration structure.
 *
 * \par Sync (or) Async:
 * This as a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Structure cynara_configuration created by cynara_configuration_create() call
 * should be released with cynara_configuration_destroy().
 * Structure cynara_configuration should be destroyed after passing it to
 * cynara_initialize().
 *
 * \param[out] pp_conf Placeholder for created cynara_configuration structure.
 *
 * \return CYNARA_API_SUCCESS on success
 *         or negative error code on error.
 */
int cynara_configuration_create(cynara_configuration **pp_conf);

/**
 * \par Description:
 * Release cynara_configuration structure created with cynara_configuration_create().
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara_configuration.
 *
 * \par Typical use case:
 * Once cynara_configuration is not needed.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_configuration structure.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \param[in] p_conf cynara_configuration structure. If NULL, the call has no effect.
 */
void cynara_configuration_destroy(cynara_configuration *p_conf);

/**
 * \par Description:
 * Set client cache size.
 *
 * \par Purpose:
 * This API is used to change default number of cached responses returned from cynara.
 *
 * \par Typical use case:
 * Once after cynara_configuration is created with cynara_configuration_create()
 * and before passing configuration to cynara_initialize().
 *
 * \par Method of function operation:
 * This API initializes cache with given capacity.
 *
 * \par Sync (or) Async:
 * This as a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * After passing cynara_configuration to cynara_initialize() calling this API will have
 * no effect.
 *
 * \param[in] p_conf cynara_configuration structure pointer.
 * \param[in] cache_size Cache size to be set.
 *
 * \return CYNARA_API_SUCCESS on success
 *        or negative error code on error.
 */
int cynara_configuration_set_cache_size(cynara_configuration *p_conf, size_t cache_size);

/**
 * \par Description:
 * Initialize cynara-client library with given configuration.
 * Create structured used in following API calls.
 *
 * \par Purpose:
 * This API must be used by prior calling cynara_check() function.
 *
 * \par Typical use case:
 * Once before a service can call cynara_check().
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success
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
 * Structure cynara created by cynara_initialize() call should be released with cynara_finish().
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
 * Release cynara-client library and destroy structure created with cynara_initialize().
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-client library.
 *
 * \par Typical use case:
 * Once after last call to cynara_check().
 *
 * \par Method of function operation:
 * This API releases inner library structures and destroys cynara structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * No other call to libcynara-client should be made after call to cynara_finish().
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
 * \parblock
 * An external application may be launched to allow user interaction in granting or denying access.
 *
 * Call to cynara_check() needs cynara structure to be created first with call to
 * cynara_initialize().
 *
 * String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 * \endparblock
 *
 * \param[in] p_cynara Cynara structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Session of client (connection, launch).
 * \param[in] user User running client.
 * \param[in] privilege Privilege that is a subject of a check..
 *
 * \return CYNARA_API_ACCESS_ALLOWED on access allowed, CYNARA_API_ACCESS_DENIED on access denial
 * or other error code on error.
 */
int cynara_check(cynara *p_cynara, const char *client, const char *client_session, const char *user,
                 const char *privilege);

/**
 * \par Description:
 * Check for (potential) permission to take some action or access a resource.
 *
 * \par Purpose:
 * This API should be used for a quick check if a user running application identified as client
 * has access to a given privilege.
 *
 * \par Typical use case:
 * An application may use this API to check, if it has (potential) permission to take some action
 * or access resource in future (permissions may rarely change). The typical use would be to disable
 * or hide some of functionalities, if they probably could not be used anyways.
 *
 * \par Method of function operation:
 * This function is very similar to cynara_check() with the difference, that in case of answer not
 * being one of CYNARA_API_ACCESS_DENIED or CYNARA_API_ACCESS_ALLOWED, no external application will
 * be consulted. Instead, CYNARA_API_ACCESS_NOT_RESOLVED is returned, meaning, that only running
 * full cynara_check() API would yield eventual answer.
 * Similarly, like in cynara_check(), argument client_session can be used to distinguish client
 * sessions and grant possibility to yield answer from cache.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * \parblock
 * The answer will be taken from Cynara's database without consulting any external applications.
 * If the answer cannot be resolved in one of CYNARA_API_PERMISSION_DENIED or
 * CYNARA_API_PERMISSION_ALLOWED without communicating with external application the API will return
 * CYNARA_API_ACCESS_NOT_RESOLVED.
 *
 * Call to cynara_simple_check() needs cynara structure to be created first with call to
 * cynara_initialize(). String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise
 * CYNARA_API_INVALID_PARAM will be returned.
 * \endparblock
 *
 * \param[in] p_cynara Cynara structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Session of client (connection, launch).
 * \param[in] user User running client.
 * \param[in] privilege Privilege that is a subject of a check.
 *
 * \return CYNARA_API_ACCESS_ALLOWED on access allowed, CYNARA_API_ACCESS_DENIED on access denial,
 * CYNARA_API_ACCESS_NOT_RESOLVED when decision is not known without usage of external plugins or
 * agents or negative error code on error.
 */
int cynara_simple_check(cynara *p_cynara, const char *client, const char *client_session,
                        const char *user, const char *privilege);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CLIENT_H */
