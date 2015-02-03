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

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes beginning with negative codes indicate an error.
 * @{
*/
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
/** @}*/

#endif /* CYNARA_ERROR_H */
