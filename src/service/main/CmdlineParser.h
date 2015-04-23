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
#include <sys/types.h>

namespace Cynara {

namespace CmdlineParser {

enum CmdlineOpt {
    Help = 'h',
    Version = 'V',
    Daemon = 'd',
    Mask = 'm',
    User = 'u',
    Group = 'g',
};

struct CmdLineOptions {
    bool m_error;
    bool m_exit;

    bool m_daemon;
    mode_t m_mask;
    uid_t m_uid;
    gid_t m_gid;
};

std::ostream &operator<<(std::ostream &os, CmdlineOpt opt);

struct CmdLineOptions handleCmdlineOptions(int argc, char * const *argv);
void printHelp(const std::string &execName);
void printVersion(void);
void printUnknownOption(const std::string &execName, const std::string &option);
void printInvalidParam(const std::string &execName, const std::string &param);
void printMissingArgument(const std::string &execName, const std::string &option);
mode_t getMask(const char *mask);
uid_t getUid(const char *user);
gid_t getGid(const char *group);

} /* namespace CmdlineOpts */

} /* namespace Cynara */

#endif /* SRC_SERVICE_MAIN_CMDLINEPARSER_H_ */
