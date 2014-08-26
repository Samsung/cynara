/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        NaiveInterpreter.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains PolicyType naive interpreter implementation.
 */
#ifndef SRC_CLIENT_COMMON_PLUGINS_NAIVEINTERPRETER_H_
#define SRC_CLIENT_COMMON_PLUGINS_NAIVEINTERPRETER_H_

#include <attributes/attributes.h>
#include <cache/CacheInterface.h>
#include <cynara-client-error.h>

namespace Cynara {

class NaiveInterpreter : public InterpreterInterface {
    bool isUsable(const PolicyResult &result UNUSED) noexcept {
        return true;
    }
    bool isCacheable(const PolicyResult &result UNUSED) noexcept {
        return true;
    }
    int toResult(const PolicyResult &result) noexcept {
        if (result.policyType() == PredefinedPolicyType::ALLOW)
            return CYNARA_API_SUCCESS;
        else
            return CYNARA_API_ACCESS_DENIED;
    }
};

} // namespace Cynara

#endif // SRC_CLIENT_COMMON_PLUGINS_NAIVEINTERPRETER_H_


