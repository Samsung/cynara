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
/**
 * @file        src/client-common/plugins/NaiveInterpreter.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains PolicyType naive interpreter implementation.
 */
#ifndef SRC_CLIENT_COMMON_PLUGINS_NAIVEINTERPRETER_H_
#define SRC_CLIENT_COMMON_PLUGINS_NAIVEINTERPRETER_H_

#include <attributes/attributes.h>
#include <cache/CacheInterface.h>
#include <cynara-error.h>

namespace Cynara {

class NaiveInterpreter : public ClientPluginInterface {
public:
    bool isUsable(const ClientSession &session UNUSED, const ClientSession &prevSession UNUSED,
                  bool &updateSession UNUSED, PolicyResult &result UNUSED) {
        return true;
    }
    bool isCacheable(const ClientSession &session UNUSED,
                     const PolicyResult &result UNUSED) {
        return true;
    }
    int toResult(const ClientSession &session UNUSED, PolicyResult &result) {
        if (result.policyType() == PredefinedPolicyType::ALLOW)
            return CYNARA_API_ACCESS_ALLOWED;
        else
            return CYNARA_API_ACCESS_DENIED;
    }

    const std::vector<PolicyType> &getSupportedPolicyTypes(void) {
        return s_supportedTypes;
    }

    void invalidate(void) {}
private:
    static const std::vector<PolicyType> s_supportedTypes;
};

} // namespace Cynara

#endif // SRC_CLIENT_COMMON_PLUGINS_NAIVEINTERPRETER_H_


