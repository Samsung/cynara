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
 * @file        src/common/types/PolicyType.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines PolicyType e.g. ALLOW or DENY
 */

#ifndef SRC_COMMON_TYPES_POLICYTYPE_H_
#define SRC_COMMON_TYPES_POLICYTYPE_H_

#include <cstdint>
#include <string>

namespace Cynara {

typedef std::uint16_t PolicyType;

namespace PredefinedPolicyType  {
    const PolicyType DENY = 0;
    const PolicyType NONE = 1;
    const PolicyType BUCKET = 0xFFFE;
    const PolicyType ALLOW = 0xFFFF;
};

class PolicyResult;
bool operator ==(const PolicyType &policyType, const PolicyResult &policyResult);
bool operator !=(const PolicyType &policyType, const PolicyResult &policyResult);

}  // namespace Cynara

#endif /* SRC_COMMON_TYPES_POLICYTYPE_H_ */
