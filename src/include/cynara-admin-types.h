/**
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
 * \file        cynara-admin-types.h
 * \author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * \author      Aleksander Zdyb <a.zdyb@samsung.com>
 * \version     1.0
 * \brief       This file contains structs and consts for cynara admin.
 */

#ifndef CYNARA_ADMIN_TYPES_H
#define CYNARA_ADMIN_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name cynara_admin_policy
 * defines single policy
 * bucket - is the name of bucket, in which policy is placed
 * client, user, privilege - defines policy key
 * result - defines result of policy
 * result_extra - not always used, may contain some additional result data
 *                like e.g. name of bucket in case result == CYNARA_ADMIN_BUCKET
 */
struct cynara_admin_policy {
    char *bucket;

    char *client;
    char *user;
    char *privilege;

    int result;
    char *result_extra;
};

/**
 * \name Wildcard
 * definition of WILDCARD, that can replace client, user or privilege name.
 * WILDCARD matches any string during check procedure from libcynara-client.
 */
#define CYNARA_ADMIN_WILDCARD "*"

/**
 * \name Name of Default Bucket
 * definition of name for default bucket - the one that check starts in.
 * default bucket cannot be removed, although its default policy
 * (which originally is set to DENY) can be changed.
 */
#define CYNARA_ADMIN_DEFAULT_BUCKET ""

/**
 * \name Operation Codes
 * operation codes that define action type to be taken in below defined functions
 * they are used mostly to define policy result
 * @{
 */

/*! \brief   a policy or bucket should be removed */
#define CYNARA_ADMIN_DELETE -1

/*! \brief   set policy result or bucket's default policy to DENY */
#define CYNARA_ADMIN_DENY 0

/*! \brief   set bucket's default policy to NONE */
#define CYNARA_ADMIN_NONE 1

/*! \brief   set policy result or bucket's default policy to ALLOW */
#define CYNARA_ADMIN_ALLOW 2

/*! \brief   set policy to point into another bucket */
#define CYNARA_ADMIN_BUCKET 3
/** @}*/

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_ADMIN_TYPES_H */
