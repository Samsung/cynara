/*
 *  Copyright (c) 2014-2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/include/cynara-error.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains error codes returned by APIs of Cynara.
 */

#ifndef CYNARA_ERROR_H
#define CYNARA_ERROR_H

#include <stddef.h>

/**
 * \name Return Codes
 * Exported by the foundation API. Return codes beginning with negative codes indicate an error.
 * @{
*/

/*! \brief  indicating that API call was interrupted by user*/
#define CYNARA_API_INTERRUPTED                  4

/*! \brief  indicating access that cannot be resolved without further actions*/
#define CYNARA_API_ACCESS_NOT_RESOLVED          3

/*! \brief   indicating access that was checked is allowed */
#define CYNARA_API_ACCESS_ALLOWED               2

/*! \brief   indicating that access that was checked is denied */
#define CYNARA_API_ACCESS_DENIED                1

/*! \brief   indicating the result of the one specific API is successful */
#define CYNARA_API_SUCCESS                      0

/*! \brief   indicating that value is not present in cache */
#define CYNARA_API_CACHE_MISS                   -1

/*! \brief   indicating that pending requests reached maximum */
#define CYNARA_API_MAX_PENDING_REQUESTS         -2

/*! \brief   indicating system is running out of memory state */
#define CYNARA_API_OUT_OF_MEMORY                -3

/*! \brief   indicating the API's parameter is malformed */
#define CYNARA_API_INVALID_PARAM                -4

/*! \brief   indicating that service is not available */
#define CYNARA_API_SERVICE_NOT_AVAILABLE        -5

/*! \brief   indicating that provided method is not supported by library */
#define CYNARA_API_METHOD_NOT_SUPPORTED         -6

/*! \brief   cynara service does not allow to perform requested operation */
#define CYNARA_API_OPERATION_NOT_ALLOWED        -7

/*! \brief   cynara service failed to perform requested operation */
#define CYNARA_API_OPERATION_FAILED             -8

/*! \brief   cynara service hasn't found requested bucket */
#define CYNARA_API_BUCKET_NOT_FOUND             -9

/*! \brief   indicating an unknown error */
#define CYNARA_API_UNKNOWN_ERROR                -10

/*! \brief   indicating configuration error */
#define CYNARA_API_CONFIGURATION_ERROR          -11

/*! \brief   indicating invalid parameter in command-line */
#define CYNARA_API_INVALID_COMMANDLINE_PARAM    -12

/*! \brief   indicating that provided buffer is too short */
#define CYNARA_API_BUFFER_TOO_SHORT             -13

/*! \brief   indicating that database is corrupted */
#define CYNARA_API_DATABASE_CORRUPTED           -14

/*! \brief   indicating that user doesn't have enough permission to perform action */
#define CYNARA_API_PERMISSION_DENIED            -15
/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \par Description:
 * Returns the error string in the user-supplied buffer buf of length buflen.
 *
 * \par Purpose:
 * This function populates passed argument buf with a string that describes the error code
 * passed in the argument errnum, possibly using the LC_MESSAGES part of the current locale
 * to select the appropriate language.
 *
 * \par Typical use case:
 * Once after any of API functions returns negative value (if the error message is to be presented
 * to the user).
 *
 * \par Method of function operation:
 * This function copies error string to memory pointed by argument buf along with termination
 * character ('\0').
 *
 * \par Sync (or) async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is thread-safe as long as setlocale() is not invoked concurrently.
 *
 * \par Important notes:
 * This function copies error string to memory pointed by argument buf only if the buffer is
 * of sufficient size (indicated by argument buflen). User is responsible for allocating sufficient
 * memory for both error string and termination character ('\0').
 * Moreover, the user must not invoke setlocale() concurrently with this function, because results
 * are unspecified.
 *
 * \param[in] errnum error number
 * \param[out] buf buffer for error message
 * \param[in] buflen buffer size
 *
 * \return CYNARA_API_SUCCESS on success, CYNARA_API_BUFFER_TOO_SHORT, if error message couldn't fit
 *         into allocated buffer, or CYNARA_API_INVALID_PARAM if errnum is not a valid error number
 *         or argument buf is NULL.
 *
 * \brief Obtain error message from error number.
 */
int cynara_strerror(int errnum, char *buf, size_t buflen);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_ERROR_H */
