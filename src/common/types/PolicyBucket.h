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
/*
 * @file        PolicyBucket.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines PolicyBucket type - a policy aggregation
                entity name
 */

#ifndef SRC_COMMON_TYPES_POLICYBUCKET_H_
#define SRC_COMMON_TYPES_POLICYBUCKET_H_

#include <algorithm>
#include <memory>
#include <string>

#include <exceptions/NotImplementedException.h>
#include <types/pointers.h>
#include <types/Policy.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyCollection.h>
#include <types/PolicyKey.h>
#include <types/PolicyType.h>

namespace Cynara {

// TODO: Move this const somewhere else
const PolicyBucketId defaultPolicyBucketId("");

class PolicyBucket {
public:

    typedef PolicyCollection::value_type value_type;
    typedef const_policy_iterator const_iterator;

    // TODO: Review usefulness of ctors
    PolicyBucket() : m_defaultPolicy(PredefinedPolicyType::DENY) {}
    PolicyBucket(const PolicyBucketId &id, const PolicyResult &defaultPolicy)
        : m_defaultPolicy(defaultPolicy), m_id(id) {}
    PolicyBucket(const PolicyCollection &policies)
        : m_policyCollection(makePolicyMap(policies)),
          m_defaultPolicy(PredefinedPolicyType::DENY) {}
    PolicyBucket(const PolicyBucketId &id,
                 const PolicyResult &defaultPolicy,
                 const PolicyCollection &policies)
        : m_policyCollection(makePolicyMap(policies)),
          m_defaultPolicy(defaultPolicy),
          m_id(id) {}

    PolicyBucket filtered(const PolicyKey &key) const;
    void insertPolicy(PolicyPtr policy);
    void deletePolicy(const PolicyKey &key);

    // TODO: Try to change interface, so this method is not needed
    void deletePolicy(std::function<bool(PolicyPtr)> predicate);

    static PolicyMap makePolicyMap(const PolicyCollection &policies);

private:
    PolicyMap m_policyCollection;
    PolicyResult m_defaultPolicy;
    PolicyBucketId m_id;

public:

    const_policy_iterator begin(void) const {
        return const_policy_iterator(m_policyCollection.begin());
    }

    const_policy_iterator end(void) const {
        return const_policy_iterator(m_policyCollection.end());
    }

    PolicyMap::size_type size(void) const noexcept {
        return m_policyCollection.size();
    }

    bool empty(void) const noexcept {
        return m_policyCollection.empty();
    }

    const PolicyResult &defaultPolicy(void) const {
        return m_defaultPolicy;
    }

    const PolicyBucketId &id(void) const {
        return m_id;
    }

    // TODO: Consider StorageBackend to be only one to alter this property
    void setDefaultPolicy(const PolicyResult &defaultPolicy) {
        m_defaultPolicy = defaultPolicy;
    }
};

} /* namespace Cynara */
#endif /* SRC_COMMON_TYPES_POLICYBUCKET_H_ */
