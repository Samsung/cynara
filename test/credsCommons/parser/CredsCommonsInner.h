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
 * @file        test/credsCommons/parser/CredsCommonsInner.h
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.1
 * @brief       Declaration of internal credential commons functions for gmock tests
 */

#ifndef TEST_CREDSCOMMONS_PARSER_CREDSCOMMONSINNER_H_
#define TEST_CREDSCOMMONS_PARSER_CREDSCOMMONSINNER_H_

#include <istream>
#include <fstream>
#include <map>
#include <string>

namespace Cynara {

typedef std::map<std::string, int> CredentialsMap;

class CredsCommonsInnerBackend {
    public:
        static int credsConfigurationFile(std::ifstream &f);
        virtual ~CredsCommonsInnerBackend() {};
        virtual bool getKeyAndValue(std::istream &f, const std::locale &loc, std::string &key,
                                    std::string &value);
        virtual bool interpretValue(const CredentialsMap &methodCodeMap, int &method,
                                    const std::string &value, bool &occurred);
        virtual int getMethodFromConfigurationFile(std::istream &f,
                                                   const CredentialsMap &methodCodeMap,
                                                   const std::string &methodName, int &method);
    private:
        // trim from the start
        static std::string &ltrim(std::string &s, const std::locale &loc);
        // trim from the end
        static std::string &rtrim(std::string &s, const std::locale &loc);
        static std::string &toLower(std::string &s, const std::locale &loc);
        static std::string &normalizeString(std::string &s, const std::locale &loc);
};

static CredsCommonsInnerBackend credsBackend;

} //namespace Cynara

#endif /* TEST_CREDSCOMMONS_PARSER_CREDSCOMMONSINNER_H_ */
