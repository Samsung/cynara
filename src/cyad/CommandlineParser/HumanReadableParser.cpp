/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/cyad/CommandlineParser/HumanReadableParser.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions to parse human readable values from command-line
 */

#include <algorithm>
#include <string>
#include <unordered_set>

#include <cynara-policy-types.h>

#include <cyad/CommandlineParser/PolicyParsingException.h>
#include "HumanReadableParser.h"

namespace Cynara {

namespace HumanReadableParser {

std::unordered_set<std::string> yeses = {
    "y", "1", "yes", "true", "on", "yeah", "yep", "of course", "sure thing"
};

bool isYes(const std::string &value) {
    std::string lower_value(value);
    std::transform(value.begin(), value.end(), lower_value.begin(), ::tolower);
    return yeses.find(value) != yeses.end();
}

PolicyType policyType(const std::string &rawPolicy) {
    if (rawPolicy == "DENY")
        return CYNARA_ADMIN_DENY;
    if (rawPolicy == "NONE")
        return CYNARA_ADMIN_NONE;
    if (rawPolicy == "BUCKET")
        return CYNARA_ADMIN_BUCKET;
    if (rawPolicy == "ALLOW")
        return CYNARA_ADMIN_ALLOW;

    try {
        return std::stoi(rawPolicy, nullptr, 0); // base of 0 means autodetection
    } catch (const std::logic_error &) {
        throw PolicyParsingException();
    }
}

} /* namespace HumanReadableParser */

} /* namespace Cynara */
