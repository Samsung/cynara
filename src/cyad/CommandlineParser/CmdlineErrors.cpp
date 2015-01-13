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
 * @file        src/cyad/CommandlineParser/CommandlineErrors.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Command-line error messages
 */

#include <cyad/CommandlineParser/CmdlineOpts.h>
#include "CmdlineErrors.h"

namespace Cynara {

namespace CmdlineErrors {

std::string inOption(CmdlineOpts::CmdlineOpt option, const std::string &prefix) {
    const auto &opt = CmdlineOpts::commandlineOptions.at(option);
    return prefix + " in --" + opt.longOption + " (-" + opt.shortOption + ")";
}

std::string unknownError(void) {
    return "Unknown error";
};

std::string noOption(void) {
    return "No option specified";
};

std::string unknownOption(void) {
    return "Unknown option";
};

std::string unknownOption(CmdlineOpts::CmdlineOpt option) {
    return inOption(option, "Unknown option");
};

std::string noType(void) {
    return "No --policy specified";
};

std::string noBucket(void) {
    return "No bucket specified";
};

std::string invalidType(void) {
    return "Invalid --policy option";
};

std::string optionMissing(CmdlineOpts::CmdlineOpt option) {
    return inOption(option, "One or more option missing");
};

std::string argumentMissing(CmdlineOpts::CmdlineOpt option) {
    return inOption(option, "One or more argument missing");
};

} /* namespace CmdlineErrors */

} /* namespace Cynara */
