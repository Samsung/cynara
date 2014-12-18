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
 * @file        src/common/types/PolicyBucket.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines PolicyBucket type - a policy aggregation
                entity name
 */

#ifndef SRC_COMMON_TYPES_POLICYBUCKET_H_
#define SRC_COMMON_TYPES_POLICYBUCKET_H_

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

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
    typedef std::vector<Policy> Policies;
    typedef std::set<PolicyBucketId> BucketIds;

    // TODO: Review usefulness of ctors
    //delete default constructor in order to prevent creation of buckets with no id
    PolicyBucket() = delete;
    PolicyBucket(const PolicyBucketId &id,
                 const PolicyResult &defaultPolicy = PredefinedPolicyType::DENY);
    PolicyBucket(const PolicyBucketId &id,
                 const PolicyCollection &policies);
    PolicyBucket(const PolicyBucketId &id,
                 const PolicyResult &defaultPolicy,
                 const PolicyCollection &policies);

    PolicyBucket filtered(const PolicyKey &key) const;
    void insertPolicy(PolicyPtr policy);
    void deletePolicy(const PolicyKey &key);
    Policies listPolicies(const PolicyKey &filter) const;
    BucketIds getSubBuckets(void) const;

    // TODO: Try to change interface, so this method is not needed
    void deletePolicy(std::function<bool(PolicyPtr)> predicate);

    static PolicyMap makePolicyMap(const PolicyCollection &policies);

    const_policy_iterator begin(void) const {
        return const_policy_iterator(m_policyCollection.begin());
    }

    const_policy_iterator end(void) const {
        return const_policy_iterator(m_policyCollection.end());
    }

    PolicyMap::size_type size(void) const {
        return m_policyCollection.size();
    }

    bool empty(void) const {
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

private:
    static void idValidator(const PolicyBucketId &id);
    static bool isIdSeparator(char c);

    PolicyMap m_policyCollection;
    PolicyResult m_defaultPolicy;
    PolicyBucketId m_id;
    static const std::string m_idSeparators;
};

} /* namespace Cynara */
#endif /* SRC_COMMON_TYPES_POLICYBUCKET_H_ */
