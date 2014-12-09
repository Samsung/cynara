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
 * @file        test/cyad/helpers.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions, matchers and operators
 */

#ifndef TEST_CYAD_HELPERS_H_
#define TEST_CYAD_HELPERS_H_

struct cynara_admin_policy;

bool operator==(const cynara_admin_policy &lhs, const cynara_admin_policy &rhs);
bool operator!=(const cynara_admin_policy &lhs, const cynara_admin_policy &rhs);

namespace Cynara {

namespace Helpers {

void freeAdminPolicyMembers(cynara_admin_policy *admin_policy);

}  /* namespace Helpers */

}  /* namespace Cynara */

#endif /* TEST_CYAD_HELPERS_H_ */
