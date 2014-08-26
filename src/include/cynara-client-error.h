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
 * @file        cynara-error.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains error codes returned by client APIs of Cynara.
 */

#ifndef CYNARA_ERROR_H
#define CYNARA_ERROR_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define CYNARA_API_SUCCESS 0

/*! \brief   indicating that access that was checked is denied */
#define CYNARA_API_ACCESS_DENIED -1

/*! \brief   indicating system is running out of memory state */
#define CYNARA_API_OUT_OF_MEMORY -2

/*! \brief   indicating the API's parameter is malformed */
#define CYNARA_API_INVALID_PARAM -3

/*! \brief   service not available */
#define CYNARA_API_SERVICE_NOT_AVAILABLE -4
/** @}*/

#endif // CYNARA_ERROR_H
