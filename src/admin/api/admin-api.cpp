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
 * @file        src/admin/api/admin-api.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-admin API
 */

#include <map>
#include <new>
#include <stdexcept>
#include <string>
#include <string.h>
#include <vector>

#include <common.h>
#include <log/log.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <cynara-admin.h>
#include <cynara-admin-error.h>

#include <api/ApiInterface.h>
#include <logic/Logic.h>

struct cynara_admin {
    Cynara::ApiInterface *impl;

    cynara_admin(Cynara::ApiInterface *_impl) : impl(_impl) {
    }
    ~cynara_admin() {
        delete impl;
    }
};

CYNARA_API
int cynara_admin_initialize(struct cynara_admin **pp_cynara_admin) {
    if (!pp_cynara_admin)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    try {
        *pp_cynara_admin = new cynara_admin(new Cynara::Logic);
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }

    init_log();

    LOGD("Cynara admin initialized");

    return CYNARA_ADMIN_API_SUCCESS;
}

CYNARA_API
int cynara_admin_finish(struct cynara_admin *p_cynara_admin) {
    delete p_cynara_admin;

    return CYNARA_ADMIN_API_SUCCESS;
}

CYNARA_API
int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin,
                              const struct cynara_admin_policy *const *policies) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!policies)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    std::map<Cynara::PolicyBucketId, std::vector<Cynara::Policy>> insertOrUpdate;
    std::map<Cynara::PolicyBucketId, std::vector<Cynara::PolicyKey>> remove;

    auto key = ([](const cynara_admin_policy *policy)->Cynara::PolicyKey {
        std::string wildcard(CYNARA_ADMIN_WILDCARD);

        auto feature = ([&wildcard] (const char *str)->Cynara::PolicyKeyFeature {
            if (wildcard.compare(str))
                return Cynara::PolicyKeyFeature::create(str);
            else
                return Cynara::PolicyKeyFeature::createWildcard();
        });

        return Cynara::PolicyKey(feature(policy->client), feature(policy->user),
                                 feature(policy->privilege));
    });

    try {
        for (auto i = policies; *i; i++) {
            const cynara_admin_policy *policy = *i;
            if(!policy->bucket || !policy->client || !policy->user || !policy->privilege)
                return CYNARA_ADMIN_API_INVALID_PARAM;

            switch (policy->result) {
                case CYNARA_ADMIN_DELETE:
                    remove[policy->bucket].push_back(key(policy));
                    break;
                case CYNARA_ADMIN_DENY:
                    insertOrUpdate[policy->bucket].push_back(Cynara::Policy(key(policy),
                                                        Cynara::PredefinedPolicyType::DENY));
                    break;
                case CYNARA_ADMIN_ALLOW:
                    insertOrUpdate[policy->bucket].push_back(Cynara::Policy(key(policy),
                                                        Cynara::PredefinedPolicyType::ALLOW));
                    break;
                case CYNARA_ADMIN_BUCKET:
                    if (!policy->result_extra)
                        return CYNARA_ADMIN_API_INVALID_PARAM;
                    insertOrUpdate[policy->bucket].push_back(Cynara::Policy(key(policy),
                                                        Cynara::PolicyResult(
                                                        Cynara::PredefinedPolicyType::BUCKET,
                                                        policy->result_extra)));
                    break;
                case CYNARA_ADMIN_NONE:
                default:
                    return CYNARA_ADMIN_API_INVALID_PARAM;
            }
        }
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }

    return p_cynara_admin->impl->setPolicies(insertOrUpdate, remove);
}

CYNARA_API
int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket,
                            int operation, const char *extra) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!bucket)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    std::string extraStr;
    try {
         extraStr = extra ? extra : "";
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }
    switch (operation) {
        case CYNARA_ADMIN_DELETE:
            return p_cynara_admin->impl->removeBucket(bucket);
        case CYNARA_ADMIN_DENY:
            return p_cynara_admin->impl->insertOrUpdateBucket(bucket,
                Cynara::PolicyResult(Cynara::PredefinedPolicyType::DENY, extraStr));
        case CYNARA_ADMIN_ALLOW:
            return p_cynara_admin->impl->insertOrUpdateBucket(bucket,
                Cynara::PolicyResult(Cynara::PredefinedPolicyType::ALLOW, extraStr));
        case CYNARA_ADMIN_NONE:
            if (bucket != Cynara::defaultPolicyBucketId) {
                return p_cynara_admin->impl->insertOrUpdateBucket(bucket,
                    Cynara::PolicyResult(Cynara::PredefinedPolicyType::NONE));
            }
            return CYNARA_ADMIN_API_OPERATION_NOT_ALLOWED;
        case CYNARA_ADMIN_BUCKET:
        default:
            return CYNARA_ADMIN_API_INVALID_PARAM;
    }
}

CYNARA_API
int cynara_admin_check(struct cynara_admin *p_cynara_admin,
                       const char *start_bucket, const int recursive,
                       const char *client, const char *user, const char *privilege,
                       int *result, char **result_extra) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!start_bucket)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!client || !user || !privilege)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!result || !result_extra)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    Cynara::PolicyResult policyResult;

    try {
        int ret = p_cynara_admin->impl->adminCheck(start_bucket, recursive != 0,
                                                   Cynara::PolicyKey(client, user, privilege),
                                                   policyResult);
        if (ret != CYNARA_ADMIN_API_SUCCESS)
            return ret;
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    } catch (const std::length_error &ex) {
        return CYNARA_ADMIN_API_INVALID_PARAM;
    }

    char *str = nullptr;
    if (!policyResult.metadata().empty()) {
        str = strdup(policyResult.metadata().c_str());
        if (!str)
            return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }
    *result = static_cast<int>(policyResult.policyType());
    *result_extra = str;

    return CYNARA_ADMIN_API_SUCCESS;
}
