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
 * @file        src/cyad/CommandlineParser/CmdlineOpts.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Command-line structs and helpers
 */

#ifndef SRC_CYAD_COMMANDLINEPARSER_CMDLINEOPTS_H_
#define SRC_CYAD_COMMANDLINEPARSER_CMDLINEOPTS_H_

#include <functional>
#include <getopt.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace Cynara {

namespace CmdlineOpts {

enum CmdlineOpt {
    SetBucket = 'b',
    DeleteBucket = 'd',
    SetPolicy = 's',
    Erase = 'e',
    Check = 'a',

    Type = 't',
    Metadata = 'm',
    Client = 'c',
    User = 'u',
    Privilege = 'p',
    Bulk = 'f',
    Bucket = 'k',
    Recursive = 'r',

    Help = 'h'
};

enum OptHasArg {
    NoArgument = no_argument,
    RequiredArgument = required_argument
};

struct CmdlineOptDesc {
    const char *longOption;
    char shortOption;
    const char *helpArgument;
    const char *helpDescription;
    OptHasArg hasArg;
};

typedef std::unordered_map<CmdlineOpt, const CmdlineOptDesc, std::hash<int>> OptionsMap;
extern const OptionsMap commandlineOptions;

typedef std::vector<option> LongOptions;
LongOptions makeLongOptions(const std::vector<CmdlineOpt> &opts);

typedef std::string ShortOptions;
ShortOptions makeShortOptions(const std::vector<CmdlineOpt> &opts);

std::string makeHelp(void);

} /* namespace CmdlineOpts */

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_CMDLINEOPTS_H_ */
