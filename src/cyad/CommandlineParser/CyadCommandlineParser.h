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
 * @file        src/cyad/CommandlineParser/CyadCommandlineParser.h
 * @author      Aleksander Zdyb <a.zdybsamsung.com>
 * @version     1.0
 * @brief       Commandline parser for Cyad
 */

#ifndef SRC_CYAD_COMMANDLINEPARSER_CYADCOMMANDLINEPARSER_H_
#define SRC_CYAD_COMMANDLINEPARSER_CYADCOMMANDLINEPARSER_H_

#include <memory>

#include <cyad/CommandlineParser/CyadCommand.h>

namespace Cynara {

namespace CyadCmdlineArgs {
    extern const char HELP;
    extern const char * const HELP_LONG;
}

namespace CyadCmdlineErrors {
    extern const char * const UNKNOWN_ERROR;
    extern const char * const NO_OPTION;
    extern const char * const UNKNOWN_OPTION;
}

class CyadCommandlineParser {
public:
    CyadCommandlineParser(int argc, char * const *argv);
    virtual ~CyadCommandlineParser();

    std::shared_ptr<CyadCommand> parseMain(void);

private:
    int m_argc;
    char * const *m_argv;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_CYADCOMMANDLINEPARSER_H_ */
