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
 * @file        test/cyad/helpers.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions, matchers and operators
 */

#include <cstdlib>
#include <cstring>

#include <cynara-admin-types.h>

#include "helpers.h"

bool operator==(const cynara_admin_policy &lhs, const cynara_admin_policy &rhs) {
    auto strEq = [] (const char *lhs, const char *rhs) -> bool {
        if (lhs != nullptr && rhs != nullptr)
            return strcmp(lhs, rhs) == 0;
        else
            return lhs == rhs;
    };

    return lhs.result == rhs.result
            && strEq(lhs.bucket, rhs.bucket)
            && strEq(lhs.client, rhs.client)
            && strEq(lhs.user, rhs.user)
            && strEq(lhs.privilege, rhs.privilege)
            && strEq(lhs.result_extra, rhs.result_extra);
}

bool operator!=(const cynara_admin_policy &lhs, const cynara_admin_policy &rhs) {
    return !(lhs == rhs);
}

namespace Cynara {

namespace Helpers {

void freeAdminPolicyMembers(cynara_admin_policy *admin_policy) {
    free(admin_policy->bucket);
    free(admin_policy->client);
    free(admin_policy->user);
    free(admin_policy->privilege);
    free(admin_policy->result_extra);
}

}  /* namespace Helpers */

}  /* namespace Cynara */
