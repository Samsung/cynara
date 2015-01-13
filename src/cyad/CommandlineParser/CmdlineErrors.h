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
 * @file        src/cyad/CommandlineParser/CmdlineErrors.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Command-line error messages
 */

#ifndef SRC_CYAD_COMMANDLINEPARSER_CMDLINEERRORS_H_
#define SRC_CYAD_COMMANDLINEPARSER_CMDLINEERRORS_H_

#include <string>

#include <cyad/CommandlineParser/CmdlineOpts.h>

namespace Cynara {

namespace CmdlineErrors {

std::string unknownError(void);
std::string noOption(void);
std::string unknownOption(void);
std::string unknownOption(CmdlineOpts::CmdlineOpt option);
std::string noType(void);
std::string noBucket(void);
std::string invalidType(void);
std::string optionMissing(CmdlineOpts::CmdlineOpt option);
std::string argumentMissing(CmdlineOpts::CmdlineOpt option);

} /* namespace CmdlineErrors */

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_CMDLINEERRORS_H_ */
