/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/helpers/creds-commons/CredsCommonsInner.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.1
 * @brief       Implementation of internal credential commons functions
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <fstream>
#include <sstream>
#include <string>

#include <log/log.h>

#include <config/PathConfig.h>
#include <cynara-creds-commons.h>
#include <cynara-error.h>

#include <CredsCommonsInner.h>

namespace Cynara {

int CredsCommonsInnerBackend::credsConfigurationFile(std::ifstream &f) {
    f.open(Cynara::PathConfig::confPath + "/creds.conf", std::fstream::in);
    if (!f.is_open())
        return CYNARA_API_CONFIGURATION_ERROR;
    return CYNARA_API_SUCCESS;
}
// parses stream with configuration skipping comments (from # till the end of line)
// untill a line of form <non empty key>=<empty or no empty value>
// is found. Returns true when key and value FOUND, false when no more lines
bool CredsCommonsInnerBackend::getKeyAndValue(std::istream &f, const std::locale &loc,
                                              std::string &key, std::string &value) {
    std::string rawLine;
    while (std::getline(f, rawLine)) {
        std::istringstream rawLineStream(rawLine);
        std::string uncommentedLine;
        if (std::getline(rawLineStream, uncommentedLine, '#')) {
            size_t found = uncommentedLine.find_first_of("=");
            if (found != std::string::npos) {
                std::string tempKey = uncommentedLine.substr(0, found);
                std::string tempValue = uncommentedLine.substr(found + 1);
                if (!tempKey.empty()) {
                    key = normalizeString(tempKey, loc);
                    value = normalizeString(tempValue, loc);
                    return true;
                }
            }
       }
   }
   return false;
}

bool CredsCommonsInnerBackend::interpretValue(const CredentialsMap &methodCodeMap, int &method,
                                              const std::string &value, bool &occurred) {
    if (occurred) //two entries in conf file are treated as error
        return false;
    occurred = true;
    auto it = methodCodeMap.find(value);
    if (it == methodCodeMap.end()) //value is not valid
        return false;

    method = it->second;
    return true;
}

int CredsCommonsInnerBackend::getMethodFromConfigurationFile(std::istream &f,
                                                             const CredentialsMap &methodCodeMap,
                                                             const std::string &methodName,
                                                             int &method) {
    std::locale loc = f.getloc();
    bool occurred = false;
    std::string key, value;
    int tmpMethod;
    while (getKeyAndValue(f, loc, key, value))
        if (key == methodName && !interpretValue(methodCodeMap, tmpMethod, value, occurred))
            return CYNARA_API_CONFIGURATION_ERROR;
    if (occurred) {
        method = tmpMethod;
        return CYNARA_API_SUCCESS;
    }
    return CYNARA_API_CONFIGURATION_ERROR;
}

// trim from the start
inline std::string &CredsCommonsInnerBackend::ltrim(std::string &s, const std::locale &loc) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [&loc](char c) { return not(std::isspace(c, loc)); }));
    return s;
}

// trim from the end
inline std::string &CredsCommonsInnerBackend::rtrim(std::string &s, const std::locale &loc) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [&loc](char c) { return not(std::isspace(c, loc)); }).base(),
                         s.end());
    return s;
}

inline std::string &CredsCommonsInnerBackend::toLower(std::string &s, const std::locale &loc) {
    std::transform(s.begin(), s.end(), s.begin(), [&loc](char c) { return std::tolower(c, loc); });
    return s;
}

inline std::string &CredsCommonsInnerBackend::normalizeString(std::string &s,
                                                              const std::locale &loc) {
    return toLower(ltrim(rtrim(s, loc), loc), loc);
}

} // namespace Cynara
