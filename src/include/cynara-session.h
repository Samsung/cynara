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
 * @file        cynara-session.h
 * \author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * \author      Radoslaw Bartosiak <r.bartosiak@samsung.com.pl>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains Cynara session helper APIs.
 */


#ifndef CYNARA_SESSION_H
#define CYNARA_SESSION_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \par Description:
 * Creates a client session string based on pid and time of creation of client process
 *
 * \par Purpose:
 * This function can be used to create session string identifier used in cynara_check()
 * and cynara_async_check() functions defined in  client libraries.
 *
 * \par Typical use case:
 * The function is called before the call of one of ...check() functions.
 * Returned string is used as client_session param in ...check() function.
 * String is released with free() function.
 *
 * \par Method of function operation:
 * The function generates client session based on the pid and start time of the client process.
 * Time is acquired from /proc/PID directory.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread safety:
 * This function is thread-safe.
 *
 * \par Important notes:
 * Memory for returned string is obtained with malloc(), and should be freed with free().
 *
 * \param[in] client_pid client application process identifier (PID).
 *
 * \return session string on success
 *         or NULL on error.
 */
char *cynara_session_from_pid(pid_t client_pid);

/* //sample code
 *
 * (...)
 *
 * //create client session
 * char *client_session;
 * client_session = cynara_session_from_pid(client_pid);
 * if (!client_session) {
 *     //use another way to create session or abandon request sending
 * }
 *
 * //check access (details of this function can be found in cynara-client.h)
 * int ret = cynara_check(p_cynara, client, client_session, user, privilege);
 *
 * //release client_session memory
 * free(client_session);
 *
 * //handle check answer
 * if (ret < 0) {
 *     //handle error
 * } else {
 *     //handle response
 * }
 *
 * (...)
 */

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_SESSION_H */
