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

#include "PolicyCollection.h"
#include "PolicyKey.h"
#include "Policy.h"
#include "exceptions/NotImplementedException.h"
#include "types/pointers.h"
#include "types/PolicyType.h"
#include "PolicyBucketId.h"

#include <string>
#include <memory>
#include <algorithm>

namespace Cynara {

// TODO: Move this const somewhere else
const PolicyBucketId defaultPolicyBucketId("");

class PolicyBucket {
public:

    PolicyBucket() : m_defaultPolicy(PredefinedPolicyType::DENY) {}
    PolicyBucket(const PolicyBucketId &id, const PolicyResult &defaultPolicy)
        : m_defaultPolicy(defaultPolicy), m_id(id) {}
    PolicyBucket(const PolicyCollection &policies)
        : m_policyCollection(policies),
          m_defaultPolicy(PredefinedPolicyType::DENY) {}

    PolicyBucket filtered(const PolicyKey &key) const;

private:
    PolicyCollection m_policyCollection;
    PolicyResult m_defaultPolicy;
    PolicyBucketId m_id;

public:
    const PolicyResult &defaultPolicy() const {
        return m_defaultPolicy;
    }

    const PolicyBucketId &id() const {
        return m_id;
    }

    PolicyCollection &policyCollection() {
        return m_policyCollection;
    }

    const PolicyCollection &policyCollection() const {
        return m_policyCollection;
    }

    // TODO: Consider StorageBackend to be only one to alter this property
    void setDefaultPolicy(const PolicyResult &defaultPolicy) {
        m_defaultPolicy = defaultPolicy;
    }

    void setPolicyCollection(const PolicyCollection &policies) {
        m_policyCollection = policies;
    }
};

} /* namespace Cynara */
#endif /* SRC_COMMON_TYPES_POLICYBUCKET_H_ */
