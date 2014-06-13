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
 * @file        PolicyBucket.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of Cynara::PolicyBucket methods
 */


#include "PolicyBucket.h"

namespace Cynara {

PolicyBucket PolicyBucket::filtered(const PolicyKey &key) const {
    PolicyBucket result;

    const auto &policies = policyCollection();
    std::copy_if(policies.begin(), policies.end(), std::back_inserter(result.policyCollection()),
        [=] (PolicyCollection::value_type policy) {
            return policy->key() == key;
    });

    // Inherit original policy
    result.setDefaultPolicy(defaultPolicy());
    return result;
}

}  // namespace Cynara
