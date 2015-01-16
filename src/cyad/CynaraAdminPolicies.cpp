/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        src/cyad/CynaraAdminPolicies.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Collection of cynara_admin_policy structs
 */

#include <algorithm>
#include <cstring>
#include <functional>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>

#include <cynara-admin-types.h>

#include "CynaraAdminPolicies.h"

namespace Cynara {

CynaraAdminPolicies::CynaraAdminPolicies() : m_sealed(false) {}

CynaraAdminPolicies::~CynaraAdminPolicies() {
    auto freePolicy = [] (cynara_admin_policy *admin_policy) {
        if (admin_policy == nullptr)
            return;

        free(admin_policy->bucket);
        free(admin_policy->client);
        free(admin_policy->user);
        free(admin_policy->privilege);
        free(admin_policy->result_extra);

        delete admin_policy;
    };

    std::for_each(m_policies.begin(), m_policies.end(), freePolicy);
}

void CynaraAdminPolicies::add(const PolicyBucketId &bucketId, const PolicyResult &policyResult,
                              const PolicyKey &policyKey) {
    if (sealed()) {
        throw std::logic_error("Collection is sealed");
    }

    // TODO: Optimize -- try not to malloc every item
    cynara_admin_policy *policy = new cynara_admin_policy();

    auto duplicateString = [] (const std::string &str) -> char * {
        auto ret = strdup(str.c_str());
        if (ret == nullptr)
            throw std::bad_alloc();
        return ret;
    };

    policy->bucket = duplicateString(bucketId);
    policy->client = duplicateString(policyKey.client().toString());
    policy->user = duplicateString(policyKey.user().toString());
    policy->privilege = duplicateString(policyKey.privilege().toString());
    policy->result = policyResult.policyType();

    if (policyResult.metadata().empty())
        policy->result_extra = nullptr;
    else
        policy->result_extra = duplicateString(policyResult.metadata());

    m_policies.push_back(policy);
}

void CynaraAdminPolicies::seal(void) {
    m_policies.push_back(nullptr);
    m_sealed = true;
}

cynara_admin_policy* const *CynaraAdminPolicies::data(void) const {
    if (sealed() == false) {
        throw std::logic_error("Collection is not sealed");
    }

    return m_policies.data();
}

cynara_admin_policy **CynaraAdminPolicies::duplicate(void) const {
    if (sealed() == false) {
        throw std::logic_error("Collection is not sealed");
    }

    auto freePolicies = [this] (cynara_admin_policy **policies) {
        for (auto i = 0u; i < m_policies.size() - 1; ++i) {
            auto p = policies[i];
            ::free(p->bucket);
            ::free(p->client);
            ::free(p->user);
            ::free(p->privilege);
            ::free(p->result_extra);
            ::free(p);
        }
        free(policies);
    };

    auto dup = static_cast<cynara_admin_policy **>(calloc(m_policies.size(),
                                                   sizeof(cynara_admin_policy *)));

    if (dup == nullptr)
        throw std::bad_alloc();

    std::unique_ptr<cynara_admin_policy *, decltype(freePolicies)> dupPtr(dup, freePolicies);

    for (auto i = 0u; i < m_policies.size() - 1; ++i) {
        dup[i] = static_cast<cynara_admin_policy *>(calloc(1, sizeof(cynara_admin_policy)));

        if (dup[i] == nullptr)
            throw std::bad_alloc();

        dup[i]->bucket = strdup(m_policies.at(i)->bucket);
        if (dup[i]->bucket == nullptr)
            throw std::bad_alloc();

        dup[i]->client = strdup(m_policies.at(i)->client);
        if (dup[i]->client == nullptr)
            throw std::bad_alloc();

        dup[i]->user = strdup(m_policies.at(i)->user);
        if (dup[i]->user == nullptr)
            throw std::bad_alloc();

        dup[i]->privilege = strdup(m_policies.at(i)->privilege);
        if (dup[i]->privilege == nullptr)
            throw std::bad_alloc();

        dup[i]->result = m_policies.at(i)->result;

        if (m_policies.at(i)->result_extra) {
            dup[i]->result_extra = strdup(m_policies.at(i)->result_extra);
            if (dup[i]->result_extra == nullptr)
                throw std::bad_alloc();
        } else
            dup[i]->result_extra = nullptr;
    }

    dup[m_policies.size() - 1] = nullptr;
    return dupPtr.release();
}

} /* namespace Cynara */
