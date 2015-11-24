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
 * \file        src/include/cynara-admin.h
 * \author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * \author      Zofia Abramowska <z.abramowska@samsung.com>
 * \author      Oskar Switalski <o.switalski@samsung.com>
 * \version     1.0
 * \brief       This file contains administration APIs of cynara available with libcynara-admin.
 */

#ifndef CYNARA_ADMIN_H
#define CYNARA_ADMIN_H

#include <cynara-admin-types.h>
#include <cynara-error.h>
#include <cynara-limits.h>
#include <cynara-policy-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct cynara_admin
 * Forward declaration of structure allowing initialization of library
 * and usage of all libcynara-admin API functions
 */
struct cynara_admin;

/**
 * \brief Initialize cynara-admin structure.
 *
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
 * Structure cynara_admin created by cynara_admin_initialize() call should be released with
 * cynara_admin_finish().
 *
 * \param[out] pp_cynara_admin address of pointer for created cynara_admin structure.
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_initialize(struct cynara_admin **pp_cynara_admin);

/**
 * \brief Release cynara-admin structure.
 *
 * \par Description:
 * Destroys structure created with cynara_admin_initialize() function.
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
 * This is a synchronous API.
 *
 * \par Important notes:
 * No invocations of cynara-admin library API functions are allowed after call to
 * cynara_admin_finish().
 *
 * \param[in] p_cynara_admin cynara_admin structure created in cynara_admin_initialize().
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_finish(struct cynara_admin *p_cynara_admin);

/**
 * \brief Insert, update or delete policies in cynara database.
 *
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
 * \parblock
 * Policies are arranged into buckets. Every policy is defined in context of some bucket identified
 * with bucket field (string). A bucket consists of policies identified with tripple: (client, user,
 * privilege), which is a (unique) key within considered bucket.
 *
 * Every policy can be one of two types: simple or bucket-pointing policy.
 *
 *  * Simple policies have result field with value of CYNARA_ADMIN_DENY or CYNARA_ADMIN_ALLOW.
 *    result_extra field should be NULL in this case.
 *  * Bucket-pointing policies have result field with value of CYNARA_ADMIN_BUCKET and name of
 *    bucket they point to in result_extra field.
 *
 *
 * Type of operation, which is run for every record (single policy) is defined by result field in
 * cynara_admin_policy structure.
 *
 * * In case of CYNARA_ADMIN_DENY or CYNARA_ADMIN_ALLOW a simple policy is updated or inserted into
 *   cynara database.
 * * In case of CYNARA_ADMIN_BUCKET, a bucket-pointing policy is updated or inserted into cynara
 *   database.
 * * In case of CYNARA_ADMIN_DELETE, a policy is removed from cynara database.
 *
 * One call of cynara_admin_set_policies() can manage many different policies in different buckets.
 * However, considered buckets must exist before referring to them in policies.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Important notes:
 * \parblock
 * When plugin API will be specified, there will be more valid types to pass as result.
 * Numerical values of defines CYNARA_ADMIN_... may change, so usage of defines names is strongly
 * recommended.
 *
 * Policies size cannot exceed CYNARA_MAX_VECTOR_SIZE excluding last null element and string members
 * length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be returned.
 * \endparblock
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[in] policies NULL terminated array of pointers to policy structures.
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin,
                              const struct cynara_admin_policy *const *policies);

/**
 * \brief Add, remove or update buckets in cynara database.
 *
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
 * \parblock
 * Every bucket has a default policy. During search, if no policy matches the searched key (client,
 * user, privilege), default policy is returned.

 * Operation run on a single bucket defined with bucket parameter.

 * Operation parameter defines what should happen with bucket. In case of:
 *  * CYNARA_ADMIN_DENY, a bucket is inserted or updated with CYNARA_ADMIN_DENY default policy;
 *  * CYNARA_ADMIN_ALLOW, a bucket is inserted or updated with CYNARA_ADMIN_ALLOW default policy;
 *  * CYNARA_ADMIN_DELETE, a bucket is removed with all policies that were kept in it.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Important notes:
 * \parblock
 * When plugin API will be specified, there will be more valid types to pass as operation / default
 * policy. Numerical values of defines CYNARA_ADMIN_... may change, so usages of provided consts is
 * strongly recommended.
 *
 * String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 *
 * Default bucket identified with CYNARA_ADMIN_DEFAULT_BUCKET exists always. Its default policy
 * is preset to DENY (can be altered, however). Default bucket cannot be removed.
 *
 * Extra parameter will be used to pass additional data to cynara extensions to build more complex
 * policies, such as ALLOW but for 5 minutes only, or ALLOW if user confirms.
 * \endparblock
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[in] bucket bucket name
 * \param[in] operation type of operation (default policy or CYNARA_ADMIN_DELETE)
 * \param[in] extra additional data for default policy (will be available with cynara extensions)
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket, int operation,
                            const char *extra);

/**
 * \brief Raw check client and user access for given privilege without using plugins extensions.
 *
 * \par Description:
 * Raw check client and user access for given privilege without using plugins extensions.
 *
 * \par Purpose:
 * This API should be used to check type of matching policy for check request
 *
 * \par Typical use case:
 * Administrator of cynara want to know, what would cynara return to client, if asked about given
 * access.
 *
 * \par Method of function operation:
 * \parblock
 * Function works almost the same way as cynara_check() client function.
 * The differences are:
 * * user must specify bucket, from which search would be started (in case of cynara_check()
 *   it is always the default bucket)
 * * user can specify if search should be recursive: disabling recursive check will constrain search
 *   to single bucket only, ignoring all policies leading to other buckets (in case of
 *   cynara_check() search is always recursive)
 * * when matching policy in cynara is found, its result is returned without being interpreted by
 *   plugin extensions.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Important notes:
 * \parblock
 * (*result_extra) may be set to NULL, if extra data are not used in matched policy
 * If (*result_extra) is not NULL, it contains a string allocated by cynara admin library
 * with malloc(3) function and must be released with free(3) function.
 *
 * String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 * \endparblock
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[in] start_bucket name of bucket where search would start.
 * \param[in] recursive FALSE (== 0) : single bucket search;
 *                      TRUE  (!= 0) : search does not ignore policies leading to another buckets.
 * \param[in] client application or process identifier.
 * \param[in] user user running client.
 * \param[in] privilege privilege that is a subject of a check.
 * \param[out] result placeholder for matched policy type.
 * \param[out] result_extra placeholder for matched policy additional data (see Important Notes!).
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_check(struct cynara_admin *p_cynara_admin,
                       const char *start_bucket, const int recursive,
                       const char *client, const char *user, const char *privilege,
                       int *result, char **result_extra);

/**
 * \brief Lists policies from single bucket in cynara database.
 *
 * \par Description:
 * Lists filtered cynara policies from single bucket.
 *
 * \par Purpose:
 * This API should be used to list policies from single bucket.
 *
 * \par Typical use case:
 * List all policies matching defined filter.
 *
 * \par Method of function operation:
 * \parblock
 * Policies are arranged into buckets. Every bucket contains set of policies. Each of policies are
 * identified with triple {client, user, privilege}. Function lists all policies from single bucket
 * with matching client, user and privilege names.
 *
 * CYNARA_ADMIN_ANY can be used to match any client, user or privilege, e.g.
 *
 * List with parameters: {client = CYNARA_ADMIN_ANY, user = "alice", privilege = CYNARA_ADMIN_ANY}
 * will match all policies related to "alice", so will match {"app1", "alice", "gps"} and
 * {CYNARA_ADMIN_WILDCARD, "alice", "sms"}, but won't match {"app3", CYNARA_ADMIN_WILDCARD, "call"}.
 *
 * List with parameters: {client = "calculator", user = CYNARA_ADMIN_WILDCARD,
 * privilege = CYNARA_ADMIN_ANY} will match {"calculator", CYNARA_ADMIN_WILDCARD, "sms"} but won't
 * match {CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_WILDCARD, "sms"} nor {"calculator", "bob", "sms"}
 *
 * Matching policies are returned as NULL terminated array of pointers to cynara_admin_policy
 * structures.
 *
 * If any of: bucket, client, user, privilege, policies is NULL then CYNARA_API_INVALID_PARAM
 * is returned.
 *
 * If there is no bucket with given name CYNARA_API_BUCKET_NOT_FOUND is returned.
 *
 * In case of successful call CYNARA_API_SUCCESS is returned and *policies points to newly created
 * array of pointers to struct cynara_admin_policy. It is responsibility of caller to release:
 *
 * * all non-NULL const char* pointers in all cynara_admin_policy structures;
 * * all pointers to cynara_admin_policy structures kept in *policies array;
 * * *policies array itself.
 *
 * All allocation made by cynara admin library are done with malloc(3) function and must be released
 * with free(3) function.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Important notes:
 * String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[in] bucket name.
 * \param[in] client filter for client name.
 * \param[in] user filter for user name.
 * \param[in] privilege filter for privilege.
 * \param[out] policies placeholder for NULL terminated array of pointers to policy structures.
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_list_policies(struct cynara_admin *p_cynara_admin, const char *bucket,
                               const char *client, const char *user, const char *privilege,
                               struct cynara_admin_policy ***policies);

/**
 * \brief Erase policies matching filter from cynara database.
 *
 * \par Description:
 * Erase policies matching filter from cynara database.
 *
 * \par Purpose:
 * This API should be used to erase multiple policies with some common key part,
 * e.g. all policies related to given user.
 *
 * \par Typical use case:
 * Erase all policies matching defined filter.
 *
 * \par Method of function operation:
 * \parblock
 * Policies are arranged into buckets. Every bucket contains set of policies. Each of policies are
 * identified with triple {client, user, privilege}. Function erases all policies with matching
 * client, user and privilege names.
 *
 * There are two modes:
 * * non-recursive (recursive parameter set to 0) - when policies are erased only from single bucket
 * * recursive (recursive parameter set to 1) when policies are removed from given start_bucket and
 * all nested buckets.
 *
 * CYNARA_ADMIN_ANY can be used to match any client, user or privilege, e.g.
 *
 * Erase with parameters: {client = CYNARA_ADMIN_ANY, user = "alice", privilege = CYNARA_ADMIN_ANY}
 * will match all policies related to "alice", so will match {"app1", "alice", "gps"} and
 * {CYNARA_ADMIN_WILDCARD, "alice", "sms"}, but won't match {"app3", CYNARA_ADMIN_WILDCARD, "call"}.
 *
 * Erase with parameters: {client = "calculator", user = CYNARA_ADMIN_WILDCARD,
 * privilege = CYNARA_ADMIN_ANY} will match {"calculator", CYNARA_ADMIN_WILDCARD, "sms"} but won't
 * match {CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_WILDCARD, "sms"} nor {"calculator", "bob", "sms"}
 *
 * If any of: start_bucket, client, user, privilege, policies is NULL then CYNARA_API_INVALID_PARAM
 * is returned.
 *
 * If there is no bucket with given name CYNARA_API_BUCKET_NOT_FOUND is returned.
 *
 * In case of successful call CYNARA_API_SUCCESS is returned.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Important notes:
 * String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[in] start_bucket name of bucket where erase would start.
 * \param[in] recursive FALSE (== 0) : erase is not recursive (single bucket erase); \n
 *                      TRUE  (!= 0) : erase follows all policies leading to nested buckets
 * \param[in] client filter for client name.
 * \param[in] user filter for user name.
 * \param[in] privilege filter for privilege.
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_erase(struct cynara_admin *p_cynara_admin,
                       const char *start_bucket, int recursive,
                       const char *client, const char *user, const char *privilege);

/**
 * \brief Lists available policies with their name description.
 *
 * \par Description:
 *
 * Lists available cynara policy results with name description.
 *
 * \par Purpose:
 * This API should be used to list all available policy results
 * (also from cynara extension plugins).
 *
 * \par Typical use case:
 * Gathering information about possible policy results and presenting them to user (using name
 * attribute of description). Result can be passed to cynara_admin_set_policies().
 *
 * \par Method of function operation:
 * \parblock
 * Policies are based on policy result number. Policies can be built in (like primitives: ALLOW,
 * DENY...) or can be loaded from cynara plugin extensions. This API gives possibility of checking,
 * which of these result exist in current cynara server and can be presented to user in a readable
 * way (of course additional translation may be needed).
 *
 * Descriptions of existing policy results are returned as NULL terminated array of pointers of
 * cynara_admin_policy_descr structures.
 *
 * Example output could be {{0, "Deny"}, {11, "AskUser"}, {65535, "Allow"}, NULL}
 *
 * In case of successful call CYNARA_API_SUCCESS is returned and *descriptions points
 * to newly created array of pointers to struct cynara_admin_policy_descr. It is responsibility
 * of caller to release:
 *
 * * all non-NULL char* pointers in all cynara_admin_policy_descr structures;
 * * all pointers to cynara_admin_policy_descr structures kept in *descriptions array;
 * * *descriptions array itself.
 *
 * All allocation made by cynara admin library are done with malloc(3) function and must be released
 * with free(3) function.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \param[in] p_cynara_admin cynara admin structure.
 * \param[out] descriptions placeholder for NULL terminated array of pointers of
 *             description structures.
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_admin_list_policies_descriptions(struct cynara_admin *p_cynara_admin,
                                            struct cynara_admin_policy_descr ***descriptions);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_ADMIN_H */
