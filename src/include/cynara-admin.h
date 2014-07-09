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
 * \file        cynara-admin.h
 * \author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * \version     1.0
 * \brief       This file contains administration APIs of cynara available with libcynara-admin.
 */


#ifndef CYNARA_ADMIN_H
#define CYNARA_ADMIN_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define CYNARA_ADMIN_API_SUCCESS 0

/*! \brief   indicating system is running out of memory state */
#define CYNARA_ADMIN_API_OUT_OF_MEMORY -1

/*! \brief   indicating the API's parameter is malformed */
#define CYNARA_ADMIN_API_INVALID_PARAM -2

/*! \brief   service not available */
#define CYNARA_ADMIN_API_SERVICE_NOT_AVAILABLE -3
/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

//todo comment
#define CYNARA_ADMIN_WILDCARD "*";

//todo comment
#define CYNARA_ADMIN_DEFAULT_BUCKET "";

//todo comments
#define CYNARA_ADMIN_DELETE -1
#define CYNARA_ADMIN_DENY 0
#define CYNARA_ADMIN_ALLOW 1
#define CYNARA_ADMIN_BUCKET 2

//todo comments
struct cynara_admin_policy {
    const char *bucket;

    const char *client;
    const char *user;
    const char *privilege;

    int result;
    const char *result_extra;
};

/**
 * \par Description:
 * Initialize cynara-admin library.
 * Creates structure used in following API calls.
 *
 * \par Purpose:
 * This function must be invoked prior to other admin API calls. It creates structure needed by
 * other cynara-admin library API functions.
 *
 * \par Typical use case:
 * Once before a service can call other cynara-admin library functions.
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success creates cynara_admin
 * structure and stores pointer to this structure at memory address passed in pp_cynara_admin
 * parameter.
 *
 * \par Sync (or) async:
 * This is a synchronous API.
 *
 * \par Important notes:
 * Structure cynara_admin created by cynara_admin_initialize call should be released with
 * cynara_admin_finish.
 *
 * \param[out] pp_cynara_admin address of pointer for created cynara_admin structure.
 *
 * \return CYNARA_ADMIN_API_SUCCESS on success, or error code otherwise.
 *
 * \brief Initialize cynara-admin library.
 */
int cynara_admin_initialize(struct cynara_admin **pp_cynara_admin);

/**
 * \par Description:
 * Releases cynara-admin library and destroys structure created with cynara_admin_initialize
 * function.
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-admin library.
 *
 * \par Typical use case:
 * Function should be called once, when done with cynara-admin library API usage.
 *
 * \par Method of function operation:
 * This API releases inner library structures and destroys cynara_admin structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Important notes:
 * No invocations of cynara-admin library API functions are allowed after call to
 * cynara_admin_finish.
 *
 * \param[in] p_cynara_admin cynara_admin structure created in cynara_admin_initialize.
 *
 * \return CYNARA_ADMIN_API_SUCCESS on success, or error code otherwise.
 *
 * \brief Release cynara-admin library.
 */
int cynara_admin_finish(struct cynara_admin *p_cynara_admin);

/**
 * \par Description:
 * Manages policies in cynara.
 *
 * \par Purpose:
 * This API should be used to insert, update or delete policies in cynara.
 *
 * \par Typical use case:
 * Enables privileged services to alter policies by adding, updating or removing records.
 *
 * \par Method of function operation:
 * Policies are arranged into buckets. Every policy is defined in context of some bucket identified
 * with bucket field (string). A bucket consists of policies identified with tripple: (client, user,
 * privilege), which is a (unique) key within considered bucket.
 *
 * Every policy can be one of two types: simple or bucket-pointing policy.
 * Simple policies have result field with value of CYNARA_ADMIN_DENY or CYNARA_ADMIN_ALLOW.
 * result_extra field should be NULL in this case.
 * Bucket-pointing policies have result field with value of CYNARA_ADMIN_BUCKET and name of bucket
 * they point to in result_extra field.
 *
 * Type of operation, which is run for every record (single policy) is defined by result field in
 * cynara_admin_policy structure.
 * In case of CYNARA_ADMIN_DENY or CYNARA_ADMIN_ALLOW a simple policy is updated or inserted into
 * cynara database.
 * In case of CYNARA_ADMIN_BUCKET, a bucket-pointing policy is updated or inserted into cynara
 * database.
 * In case of CYNARA_ADMIN_DELETE, a policy is removed from cynara database.
 * One call of cynara_admin_set_policies can manage many different policies in different buckets.
 *
 * However, considered buckets must exist before referring to them in policies.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Important notes:
 * When plugin API will be specified, there will be more valid types to pass as result.
 * Numerical values of defines CYNARA_ADMIN_... may change, so usage of defines names is strongly
 * recommended.
 *
 * In case of error, database may end up in a state, where changes are partially applied. This is
 * going to be fixed along with introduction of transactions in future releases.
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[in] policies NULL terminated array of pointers to policy structures.
 *
 * \return CYNARA_ADMIN_API_SUCCESS on success, or error code otherwise.
 *
 * \brief Insert, update or delete policies in cynara database.
 */
int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin,
                              const cynara_admin_policy *const *policies);

/**
 * \par Description:
 * Adds new, updates or removes existing bucket for policies in cynara.
 *
 * \par Purpose:
 * This API should be used to add, remove or update buckets.
 *
 * \par Typical use case:
 * Enables privileged services to alter policies database by adding, updating or removing buckets.
 *
 * \par Method of function operation:
 * Every bucket has a default policy. During search, if no policy matches the searched key (client,
 * user, privilege), default policy is returned.

 * Operation run on a single bucket defined with bucket parameter.

 * Operation parameter defines what should happen with bucket. In case of:
 * CYNARA_ADMIN_DENY, a bucket is inserted or updated with CYNARA_ADMIN_DENY default policy;
 * CYNARA_ADMIN_ALLOW, a bucket is inserted or updated with CYNARA_ADMIN_ALLOW default policy;
 * CYNARA_ADMIN_DELETE, a bucket is removed with all policies that were kept in it.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Important notes:
 * When plugin API will be specified, there will be more valid types to pass as operation / default
 * policy. Numerical values of defines CYNARA_ADMIN_... may change, so usages of provided consts is
 * strongly recommended.
 *
 * Default bucket identified with CYNARA_ADMIN_DEFAULT_BUCKET exists always. Its default policy
 * is preset to DENY (can be altered, however). Default bucket cannot be removed.
 *
 * Extra parameter will be used to pass additional data to cynara extensions to build more complex
 * policies, such as ALLOW but for 5 minutes only, or ALLOW if user confirms.
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[in] bucket bucket name
 * \param[in] operation type of operation (default policy or CYNARA_ADMIN_DELETE)
 * \param[in] extra additional data for default policy (will be available with cynara extensions)
 *
 * \return CYNARA_ADMIN_API_SUCCESS on success, or error code otherwise.
 *
 * \brief Add, remove or update buckets in cynara database.
 */
int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket, int operation,
                            const char *extra);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_ADMIN_H */
