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
 * @file        test/credsCommons/parser/FakeCredsCommonsInnerBackend.h
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Mock of CredsCommonsInnerBackend
 */

#ifndef TEST_CREDSCOMMONS_PARSER_FAKECREDSCOMMONSINNERBACKEND_H_
#define TEST_CREDSCOMMONS_PARSER_FAKECREDSCOMMONSINNERBACKEND_H_

#include "CredsCommonsInner.h"

class FakeCredsCommonsInnerBackend : public Cynara::CredsCommonsInnerBackend {
public:

    MOCK_METHOD4(getKeyAndValue, bool(std::istream &f, const std::locale &loc, std::string &key,
                 std::string &value));
    MOCK_METHOD4(interpretValue, bool(const Cynara::CredentialsMap &methodCodeMap, int &method,
                                      const std::string &value, bool &occurred));
};

class FakeGetKeyAndValueBackend : public Cynara::CredsCommonsInnerBackend {
public:
    MOCK_METHOD4(getKeyAndValue, bool(std::istream &f, const std::locale &loc, std::string &key,
                                      std::string &value));
};
#endif /* TEST_CREDSCOMMONS_PARSER_FAKECREDSCOMMONSINNERBACKEND_H_ */
