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
 * \file        src/include/cynara-policy-types.h
 * \author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * \version     1.0
 * \brief       This file contains policy types / operations definitions.
 */

#ifndef CYNARA_POLICY_TYPES_H
#define CYNARA_POLICY_TYPES_H

/**
 * \name Operation Codes
 * Operation codes that define action type to be taken
 * @{
 */

/*! \brief   a policy or bucket should be removed */
#define CYNARA_ADMIN_DELETE -1

/*! \brief   set policy result or bucket's default policy to DENY */
#define CYNARA_ADMIN_DENY 0

/*! \brief   set bucket's default policy to NONE */
#define CYNARA_ADMIN_NONE 1

/*! \brief   set policy to point into another bucket */
#define CYNARA_ADMIN_BUCKET 0xFFFE

/*! \brief   set policy result or bucket's default policy to ALLOW */
#define CYNARA_ADMIN_ALLOW 0xFFFF
/** @}*/

#endif /* CYNARA_POLICY_TYPES_H */
