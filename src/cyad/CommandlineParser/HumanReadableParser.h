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
 * @file        src/cyad/CommandlineParser/HumanReadableParser.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions to parse human readable values from command-line
 */

#ifndef SRC_CYAD_COMMANDLINEPARSER_HUMANREADABLEPARSER_H_
#define SRC_CYAD_COMMANDLINEPARSER_HUMANREADABLEPARSER_H_

#include <string>

#include <types/PolicyType.h>

namespace Cynara {

namespace HumanReadableParser {
    bool isYes(const std::string &value);
    PolicyType policyType(const std::string &rawPolicy);
} /* namespace HumanReadableParser */

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_HUMANREADABLEPARSER_H_ */
