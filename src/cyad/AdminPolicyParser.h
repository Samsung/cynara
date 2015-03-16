/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/cyad/AdminPolicyParser.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Parses policies from input stream
 */

#ifndef SRC_CYAD_ADMINPOLICYPARSER_H_
#define SRC_CYAD_ADMINPOLICYPARSER_H_

#include <istream>
#include <memory>

#include <cyad/CynaraAdminPolicies.h>

namespace Cynara {

namespace AdminPolicyParser {

CynaraAdminPolicies parse(std::istream &input, std::function<PolicyType(const std::string &)>);

} /* namespace AdminPolicyParser */

} /* namespace Cynara */

#endif /* SRC_CYAD_ADMINPOLICYPARSER_H_ */
