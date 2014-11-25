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
 * @file        src/common/types/PolicyKey.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of Cynara:PolicyKey methods
 */


#include  "PolicyKey.h"

#include <sstream>

#include <cynara-admin-types.h>

namespace Cynara {

const std::string PolicyKeyFeature::m_wildcardValue = CYNARA_ADMIN_WILDCARD;
const std::string PolicyKeyFeature::m_anyValue = CYNARA_ADMIN_ANY;

const std::string &PolicyKeyFeature::toString(void) const {
    return value();
}

std::string PolicyKey::toString(void) const {
    std::stringstream ss;
    ss << client().toString() << "\t"
       << user().toString() << "\t"
       << privilege().toString();
    return ss.str();
}

bool operator ==(const PolicyKeyFeature::ValueType &pkf1, const PolicyKeyFeature &pkf2) {
    return pkf2 == pkf1;
}

} /* namespace Cynara */
