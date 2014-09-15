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
 * @file        PolicyKeyHelpers.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions to manage Cynara::PolicyKey
 */

#include <sstream>

#include "PolicyKeyHelpers.h"

namespace Cynara {

std::string PolicyKeyHelpers::glueKey(const PolicyKey &key) {
    return glueKey(key.client(), key.user(), key.privilege());
}

std::string PolicyKeyHelpers::glueKey(const PolicyKeyFeature &client,
                                      const PolicyKeyFeature &user,
                                      const PolicyKeyFeature &privilege) {
    const std::string sep = ";";
    const std::string c = client.toString();
    const std::string u = user.toString();
    const std::string p = privilege.toString();

    std::ostringstream glued;
    glued << c << sep << u << sep << p << sep << c.size() << sep << u.size() << sep << p.size();
    return glued.str();
};

std::vector<std::string> PolicyKeyHelpers::keyVariants(const PolicyKey &key) {
    const auto &client = key.client();
    const auto &user = key.user();
    const auto &privilege = key.privilege();
    const auto w = PolicyKeyFeature::createWildcard();

    return {
        glueKey(client, user, privilege),
        glueKey(w, user, privilege),
        glueKey(client, w, privilege),
        glueKey(client, user, w),
        glueKey(w, w, privilege),
        glueKey(w, user, w),
        glueKey(client, w, w),
        glueKey(w, w, w),
    };
}

} /* namespace Cynara */
