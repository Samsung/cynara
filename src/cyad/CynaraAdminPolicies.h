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
 * @file        src/cyad/CynaraAdminPolicies.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Collection of cynara_admin_policy structs
 */

#ifndef SRC_CYAD_CYNARAADMINPOLICIES_H_
#define SRC_CYAD_CYNARAADMINPOLICIES_H_

#include <vector>

#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

struct cynara_admin_policy;

namespace Cynara {

class CynaraAdminPolicies {
public:
    CynaraAdminPolicies();
    virtual ~CynaraAdminPolicies();

    void add(const PolicyBucketId &bucketId, const PolicyResult &policyResult,
             const PolicyKey &policyKey);

    void seal(void);

    cynara_admin_policy* const *data(void) const;

    bool sealed(void) const {
        return m_sealed;
    }

private:
    std::vector<cynara_admin_policy *> m_policies;
    bool m_sealed;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_CYNARAADMINPOLICIES_H_ */
