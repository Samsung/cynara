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

    extern const char SET_BUCKET;
    extern const char * const SET_BUCKET_LONG;

    extern const char DELETE_BUCKET;
    extern const char * const DELETE_BUCKET_LONG;

    extern const char TYPE;
    extern const char * const TYPE_LONG;

    extern const char METADATA;
    extern const char * const METADATA_LONG;

    extern const char BUCKET;
    extern const char * const BUCKET_LONG;

    extern const char CLIENT;
    extern const char * const CLIENT_LONG;

    extern const char USER;
    extern const char * const USER_LONG;

    extern const char PRIVILEGE;
    extern const char * const PRIVILEGE_LONG;

    extern const char BULK;
    extern const char * const BULK_LONG;
}

namespace CyadCmdlineErrors {
    extern const char * const UNKNOWN_ERROR;
    extern const char * const NO_OPTION;
    extern const char * const UNKNOWN_OPTION;
    extern const char * const UNKNOWN_OPTION_SET_BUCKET;
    extern const char * const UNKNOWN_OPTION_DELETE_BUCKET;
    extern const char * const UNKNOWN_OPTION_SET_POLICY;
    extern const char * const NO_POLICY;
    extern const char * const NO_BUCKET;
    extern const char * const INVALID_POLICY;
    extern const char * const OPTION_MISSING_SET_POLICY;
    extern const char * const ARGUMENT_MISSING_SET_POLICY;
}

class CyadCommandlineParser {
public:
    CyadCommandlineParser(int argc, char * const *argv);
    virtual ~CyadCommandlineParser();

    std::shared_ptr<CyadCommand> parseMain(void);
    static PolicyType parsePolicyType(const std::string &rawPolicy);

protected:
    std::shared_ptr<CyadCommand> parseSetBucket(const std::string &bucketId);
    std::shared_ptr<CyadCommand> parseDeleteBucket(const std::string &bucketId);
    std::shared_ptr<CyadCommand> parseSetPolicy(void);

private:
    int m_argc;
    char * const *m_argv;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_CYADCOMMANDLINEPARSER_H_ */
