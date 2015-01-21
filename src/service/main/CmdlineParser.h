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
 * @file        src/service/main/CmdlineParser.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Helper namespace for Cynara's command-line options parsing
 */

#ifndef SRC_SERVICE_MAIN_CMDLINEPARSER_H_
#define SRC_SERVICE_MAIN_CMDLINEPARSER_H_

#include <ostream>
#include <string>

namespace Cynara {

namespace CmdlineParser {

enum CmdlineOpt {
    Help = 'h',
    Version = 'V'
};

std::ostream &operator<<(std::ostream &os, CmdlineOpt opt);

bool handleCmdlineOptions(int argc, char * const *argv);
void printHelp(const std::string &execName);
void printVersion(void);
void printUnknownOption(const std::string &execName);
void printNoOptions(const std::string &execName);

} /* namespace CmdlineOpts */

} /* namespace Cynara */

#endif /* SRC_SERVICE_MAIN_CMDLINEPARSER_H_ */
