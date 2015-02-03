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
 * @file        src/admin/api/admin-api.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-admin API
 */

#include <functional>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <cstring>
#include <vector>

#include <common.h>
#include <exceptions/FileLockAcquiringException.h>
#include <exceptions/TryCatch.h>
#include <log/log.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyDescription.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <cynara-admin.h>
#include <cynara-error.h>

#include <api/ApiInterface.h>
#include <logic/Logic.h>
#include <logic/OnlineLogic.h>
#include <logic/Utility.h>

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
        return CYNARA_API_INVALID_PARAM;

    init_log();

    return Cynara::tryCatch([&]() {
        try {
            Cynara::LogicUniquePtr ptr(new Cynara::Logic());
            *pp_cynara_admin = new cynara_admin(ptr.get());
            ptr.release();
        } catch (const Cynara::FileLockAcquiringException &ex) {
            LOGE("%s", ex.what());
            return CYNARA_API_OPERATION_FAILED;
        }

        LOGD("Cynara admin initialized");

        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_admin_finish(struct cynara_admin *p_cynara_admin) {
    delete p_cynara_admin;

    return CYNARA_API_SUCCESS;
}

bool int2PolicyType(int type, Cynara::PolicyType &policyType) {
    if (type < std::numeric_limits<Cynara::PolicyType>::min())
        return false;
    if (type > std::numeric_limits<Cynara::PolicyType>::max())
        return false;
    policyType = static_cast<Cynara::PolicyType>(type);
    return true;
}

CYNARA_API
int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin,
                              const struct cynara_admin_policy *const *policies) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!policies)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
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

        for (auto i = policies; *i; i++) {
            const cynara_admin_policy *policy = *i;
            if(!policy->bucket || !policy->client || !policy->user || !policy->privilege)
                return CYNARA_API_INVALID_PARAM;

            switch (policy->result) {
                case CYNARA_ADMIN_NONE:
                    return CYNARA_API_INVALID_PARAM;
                case CYNARA_ADMIN_DELETE:
                    remove[policy->bucket].push_back(key(policy));
                    break;
                case CYNARA_ADMIN_BUCKET:
                    if (!policy->result_extra)
                        return CYNARA_API_INVALID_PARAM;
                default:
                {
                    std::string extraStr = policy->result_extra ? policy->result_extra : "";
                    Cynara::PolicyType policyType;
                    if (!int2PolicyType(policy->result, policyType))
                        return CYNARA_API_INVALID_PARAM;

                    insertOrUpdate[policy->bucket].push_back(Cynara::Policy(key(policy),
                                                             Cynara::PolicyResult(
                                                             policyType,
                                                             extraStr)));
                }
            }
        }

        return p_cynara_admin->impl->setPolicies(insertOrUpdate, remove);
    });
}

CYNARA_API
int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket,
                            int operation, const char *extra) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!bucket)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {

        switch (operation) {
            case CYNARA_ADMIN_DELETE:
                return p_cynara_admin->impl->removeBucket(bucket);
            case CYNARA_ADMIN_NONE:
                if (bucket != Cynara::defaultPolicyBucketId) {
                    return p_cynara_admin->impl->insertOrUpdateBucket(bucket,
                        Cynara::PolicyResult(Cynara::PredefinedPolicyType::NONE));
                }
                return CYNARA_API_OPERATION_NOT_ALLOWED;
            case CYNARA_ADMIN_BUCKET:
                return CYNARA_API_INVALID_PARAM;
            default:
            {
                std::string extraStr = extra ? extra : "";
                Cynara::PolicyType policyType;
                if (!int2PolicyType(operation, policyType))
                    return CYNARA_API_INVALID_PARAM;

                return p_cynara_admin->impl->insertOrUpdateBucket(bucket,
                    Cynara::PolicyResult(policyType, extraStr));
            }
        }
    });
}

CYNARA_API
int cynara_admin_check(struct cynara_admin *p_cynara_admin,
                       const char *start_bucket, const int recursive,
                       const char *client, const char *user, const char *privilege,
                       int *result, char **result_extra) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!start_bucket)
        return CYNARA_API_INVALID_PARAM;
    if (!client || !user || !privilege)
        return CYNARA_API_INVALID_PARAM;
    if (!result || !result_extra)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        Cynara::PolicyResult policyResult;
        Cynara::PolicyBucketId startBucket;
        std::string clientStr;
        std::string userStr;
        std::string privilegeStr;

        try {
            startBucket = start_bucket;
            clientStr = client;
            userStr = user;
            privilegeStr = privilege;
        } catch (const std::length_error &e) {
            LOGE("%s", e.what());
            return CYNARA_API_INVALID_PARAM;
        }

        int ret = p_cynara_admin->impl->adminCheck(startBucket, recursive != 0,
                                                   Cynara::PolicyKey(clientStr, userStr,
                                                   privilegeStr), policyResult);
        if (ret != CYNARA_API_SUCCESS)
            return ret;

        char *str = nullptr;
        if (!policyResult.metadata().empty()) {
            str = strdup(policyResult.metadata().c_str());
            if (!str)
                return CYNARA_API_OUT_OF_MEMORY;
        }
        *result = static_cast<int>(policyResult.policyType());
        *result_extra = str;

        return CYNARA_API_SUCCESS;
    });
}

static int copyPolicy(const char *bucket, const Cynara::Policy &from, cynara_admin_policy *&to) {
    to = reinterpret_cast<cynara_admin_policy*>(calloc(1, sizeof(cynara_admin_policy)));
    if (!to)
        return CYNARA_API_OUT_OF_MEMORY;
    to->bucket = strdup(bucket);
    if (!to->bucket)
        return CYNARA_API_OUT_OF_MEMORY;
    to->client = strdup(from.key().client().value().c_str());
    if (!to->client)
            return CYNARA_API_OUT_OF_MEMORY;
    to->user = strdup(from.key().user().value().c_str());
    if (!to->user)
            return CYNARA_API_OUT_OF_MEMORY;
    to->privilege = strdup(from.key().privilege().value().c_str());
    if (!to->privilege)
            return CYNARA_API_OUT_OF_MEMORY;
    to->result = static_cast<int>(from.result().policyType());
    if (!from.result().metadata().empty()) {
        to->result_extra = strdup(from.result().metadata().c_str());
        if (!to->result_extra)
            return CYNARA_API_OUT_OF_MEMORY;
    }
    return CYNARA_API_SUCCESS;
}

static void freeElem(struct cynara_admin_policy *policyPtr) {
    free(policyPtr->bucket);
    free(policyPtr->client);
    free(policyPtr->user);
    free(policyPtr->privilege);
    free(policyPtr->result_extra);
    free(policyPtr);
}

CYNARA_API
int cynara_admin_list_policies(struct cynara_admin *p_cynara_admin, const char *bucket,
                               const char *client, const char *user, const char *privilege,
                               struct cynara_admin_policy ***policies) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!bucket || !client || !user || !privilege)
        return CYNARA_API_INVALID_PARAM;
    if (!policies)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        Cynara::PolicyKeyFeature::ValueType clientStr;
        Cynara::PolicyKeyFeature::ValueType userStr;
        Cynara::PolicyKeyFeature::ValueType privilegeStr;
        Cynara::PolicyBucketId bucketId;
        try {
            clientStr = client;
            userStr = user;
            privilegeStr = privilege;
            bucketId = bucket;
        } catch (const std::length_error &e) {
            LOGE("%s", e.what());
            return CYNARA_API_INVALID_PARAM;
        }

        std::vector<Cynara::Policy> policiesVector;
        int ret = p_cynara_admin->impl->listPolicies(bucketId, Cynara::PolicyKey(clientStr, userStr,
                                                     privilegeStr), policiesVector);
        if (ret != CYNARA_API_SUCCESS)
            return ret;

        auto copyFun = std::bind(copyPolicy, bucket, std::placeholders::_1, std::placeholders::_2);
        return Cynara::createNullTerminatedArray<Cynara::Policy, cynara_admin_policy>
                            (policiesVector, policies, copyFun);
    });
}

static int copyDescr(const Cynara::PolicyDescription &from, cynara_admin_policy_descr *&to) {
    to = reinterpret_cast<cynara_admin_policy_descr*>(calloc(1, sizeof(cynara_admin_policy_descr)));
    if (!to)
        return CYNARA_API_OUT_OF_MEMORY;
    to->result = static_cast<int>(from.type);
    to->name = strdup(from.name.c_str());
    if (!to->name)
        return CYNARA_API_OUT_OF_MEMORY;
    return CYNARA_API_SUCCESS;
}

static void freeElem(struct cynara_admin_policy_descr *descrPtr) {
    free(descrPtr->name);
    free(descrPtr);
}

CYNARA_API
int cynara_admin_list_policies_descriptions(struct cynara_admin *p_cynara_admin,
                                            struct cynara_admin_policy_descr ***descriptions) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!descriptions)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&p_cynara_admin, &descriptions] () {
        std::vector<Cynara::PolicyDescription> descriptionsVector;
        int ret = p_cynara_admin->impl->listDescriptions(descriptionsVector);

        if (ret != CYNARA_API_SUCCESS)
            return ret;
        return Cynara::createNullTerminatedArray<Cynara::PolicyDescription,
                                                 cynara_admin_policy_descr>
                (descriptionsVector, descriptions, copyDescr);
    });
}

CYNARA_API
int cynara_admin_erase(struct cynara_admin *p_cynara_admin,
                       const char *start_bucket, const int recursive,
                       const char *client, const char *user, const char *privilege) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!start_bucket || !client || !user || !privilege)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        Cynara::PolicyKeyFeature::ValueType clientStr;
        Cynara::PolicyKeyFeature::ValueType userStr;
        Cynara::PolicyKeyFeature::ValueType privilegeStr;
        Cynara::PolicyBucketId startBucket;
        try {
            clientStr = client;
            userStr = user;
            privilegeStr = privilege;
            startBucket = start_bucket;
        } catch (const std::length_error &e) {
            LOGE("%s", e.what());
            return CYNARA_API_INVALID_PARAM;
        }

        return p_cynara_admin->impl->erasePolicies(startBucket, recursive != 0,
                                                   Cynara::PolicyKey(clientStr, userStr,
                                                   privilegeStr));
    });
}
