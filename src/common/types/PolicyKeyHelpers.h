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
 * @file        src/common/types/PolicyKeyHelpers.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions to manage Cynara::PolicyKey
 */
#ifndef SRC_COMMON_TYPES_POLICYKEYHELPERS_H_
#define SRC_COMMON_TYPES_POLICYKEYHELPERS_H_

#include <string>
#include <vector>

#include <types/PolicyKey.h>

namespace Cynara {

class PolicyKeyHelpers {
public:
    static std::string glueKey(const PolicyKey &client);
    static std::string glueKey(const PolicyKeyFeature &client, const PolicyKeyFeature &user,
                               const PolicyKeyFeature &privilege);
    static std::vector<std::string> keyVariants(const PolicyKey &key);
};

} /* namespace Cynara */

#endif /* SRC_COMMON_TYPES_POLICYKEYHELPERS_H_ */
