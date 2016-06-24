/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/include/cynara-creds-self.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains Cynara credentials helper APIs for current process
 * @example     cynara-creds-self.example
 */

#ifndef CYNARA_CREDS_SELF_H
#define CYNARA_CREDS_SELF_H

#include <sys/types.h>

#include <cynara-creds-commons.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \par Description:
 * Creates a client identification string with given method. Client is a current process.
 *
 * \par Purpose:
 * Client identification string is required for cynara_check() and cynara_async_create_request()
 * functions.
 *
 * \par Typical use case:
 * The function is called before the call of cynara_check() or cynara_async_create_request()
 * function. Returned string is used as client parameter in cynara_check() or
 * cynara_async_create_request() function. String is released with free() function when it is no
 * longer needed.
 *
 * \par Method of function operation:
 * The function generates client string using current process context.
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
 * If method is CLIENT_METHOD_DEFAULT then it will be chosen based on Cynara configuration file.
 *
 * \param[in] method Method of client identifier creation
 * \param[out] client Placeholder for allocated string containing client id
 *
 * \return CYNARA_API_SUCCESS on success
 * \return CYNARA_API_INVALID_PARAM when client is NULL
 * \return CYNARA_API_METHOD_NOT_SUPPORTED when requested method is not supported
 * \return CYNARA_API_CONFIGURATION_ERROR if the configuration file can not be opened or
 *                                        there are errors in configuration file
 * \return CYNARA_API_OUT_OF_MEMORY when there was error allocating memory
 */
int cynara_creds_self_get_client(enum cynara_client_creds method, char **client);

/**
 * \par Description:
 * Creates a user identification string with given method. User is current process.
 *
 * \par Purpose:
 * User identification string is required for cynara_check() and cynara_async_create_request()
 * functions.
 *
 * \par Typical use case:
 * The function is called before the call of cynara_check() or cynara_async_create_request()
 * function. Returned string is used as user parameter in cynara_check() or
 * cynara_async_create_request() function. String is released with free() function when it is no
 * longer needed.
 *
 * \par Method of function operation:
 * The function generates user string using current process context.
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
 * If method is USER_METHOD_DEFAULT then it will be chosen based on Cynara configuration file.
 *
 * \param[in] method Method of user identifier creation
 * \param[out] user Placeholder for allocated string containing user id
 *
 * \return CYNARA_API_SUCCESS on success
 * \return CYNARA_API_INVALID_PARAM when user is NULL
 * \return CYNARA_API_METHOD_NOT_SUPPORTED when requested method is not supported
 * \return CYNARA_API_CONFIGURATION_ERROR if the configuration file can not be opened or
 *                                        there are errors in configuration file
 * \return CYNARA_API_OUT_OF_MEMORY when there was error allocating memory
 */
int cynara_creds_self_get_user(enum cynara_user_creds method, char **user);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CREDS_SELF_H */
