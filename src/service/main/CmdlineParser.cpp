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
 * @file        src/service/main/CmdlineParser.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Helper namespace for Cynara's command-line options parsing
 */

#include <cstdlib>
#include <getopt.h>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <sstream>

#include "CmdlineParser.h"

namespace Cynara {

namespace CmdlineParser {

std::ostream &operator<<(std::ostream &os, CmdlineOpt opt) {
    return os << static_cast<char>(opt);
}

struct CmdLineOptions handleCmdlineOptions(int argc, char * const *argv) {
    const std::string execName(argv[0]);
    std::stringstream shortOpts;
    shortOpts << ":"
              << CmdlineOpt::Help
              << CmdlineOpt::Version
              << CmdlineOpt::Daemon
              << CmdlineOpt::Mask << ":"
              << CmdlineOpt::User << ":"
              << CmdlineOpt::Group << ":";

    const struct option longOpts[] = {
        { "help",       no_argument,          NULL, CmdlineOpt::Help },
        { "version",    no_argument,          NULL, CmdlineOpt::Version },
        { "daemon",     no_argument,          NULL, CmdlineOpt::Daemon },
        { "mask",       required_argument,    NULL, CmdlineOpt::Mask },
        { "user",       required_argument,    NULL, CmdlineOpt::User },
        { "group",      required_argument,    NULL, CmdlineOpt::Group },
        { NULL, 0, NULL, 0 }
    };

    struct CmdLineOptions ret = {.m_error = false,
                                 .m_exit = false,
                                 .m_daemon = false,
                                 .m_mask = static_cast<mode_t>(-1),
                                 .m_uid = static_cast<uid_t>(-1),
                                 .m_gid = static_cast<gid_t>(-1) };

    optind = 0; // On entry to `getopt', zero means this is the first call; initialize.
    int opt;
    while ((opt = getopt_long(argc, argv, shortOpts.str().c_str(), longOpts, nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Help:
                printHelp(execName);
                ret.m_error = false;
                ret.m_exit = true;
                return ret;
            case CmdlineOpt::Version:
                printVersion();
                ret.m_error = false;
                ret.m_exit = true;
                return ret;
            case CmdlineOpt::Daemon:
                ret.m_daemon = true;
                break;
            case CmdlineOpt::Mask:
                ret.m_mask = getMask(optarg);
                if (ret.m_mask == static_cast<mode_t>(-1)) {
                    printInvalidParam(execName, optarg);
                    ret.m_error = true;
                    ret.m_exit = true;
                    return ret;
                }
                break;
            case CmdlineOpt::User:
                ret.m_uid = getUid(optarg);
                if (ret.m_uid == static_cast<uid_t>(-1)) {
                    printInvalidParam(execName, optarg);
                    ret.m_error = true;
                    ret.m_exit = true;
                    return ret;
                }
                break;
            case CmdlineOpt::Group:
                ret.m_gid = getGid(optarg);
                if (ret.m_gid == static_cast<gid_t>(-1)) {
                    printInvalidParam(execName, optarg);
                    ret.m_error = true;
                    ret.m_exit = true;
                    return ret;
                }
                break;
            case ':': // Missing argument
                ret.m_error = true;
                ret.m_exit = true;
                switch (optopt) {
                    case CmdlineOpt::Mask:
                    case CmdlineOpt::User:
                    case CmdlineOpt::Group:
                        printMissingArgument(execName, argv[optind - 1]);
                        return ret;
                }
                //intentional fall to Unknown option
            case '?': // Unknown option
            default:
                printUnknownOption(execName, argv[optind - 1]);
                ret.m_error = true;
                ret.m_exit = true;
                return ret;
        }
    }

    return ret;
}

void printHelp(const std::string &execName) {
    std::cout << "Usage: " << execName << " [OPTIONS]" << std::endl << std::endl;
    std::cout << "Information mode options [program exits after printing information]:" << std::endl;
    std::cout << "  -V, --version                print version of " << execName << " and exit"
                 << std::endl;
    std::cout << "  -h, --help                   print this help message and exit" << std::endl;
    std::cout << "Normal work mode options:" << std::endl;
    std::cout << "  -d, --daemon                 daemonize "
                 "[by default " << execName << " does not daemonize]" << std::endl;
    std::cout << "  -m, --mask=MASK              set umask to MASK "
                 "[by default no umask is set]" << std::endl;
    std::cout << "  -u, --user=USER              change user to USER "
                 "[by default uid is not changed]" << std::endl;
    std::cout << "  -g, --group=GROUP            change group to GROUP "
                 "[by default gid is not changed]" << std::endl;
}

void printVersion(void) {
    std::cout << std::string(CYNARA_VERSION) << std::endl;
}

void printUnknownOption(const std::string &execName, const std::string &option) {
    std::cerr << "Unknown option: " << option << std::endl;
    printHelp(execName);
}

void printInvalidParam(const std::string &execName, const std::string &param) {
    std::cerr << "Invalid param: " << param << std::endl;
    printHelp(execName);
}

void printMissingArgument(const std::string &execName, const std::string &option) {
    std::cerr << "Missing argument for option: " << option << std::endl;
    printHelp(execName);
}

mode_t getMask(const char *mask) {
    mode_t ret = static_cast<mode_t>(-1);
    if (!mask)
        return ret;
    try {
        ret = static_cast<mode_t>(std::stoi(mask, 0, 0));
    } catch (...) {
    }
    return ret;
}

uid_t getUid(const char *user) {
    uid_t ret = static_cast<uid_t>(-1);
    if (!user)
        return ret;
    try {
        ret = static_cast<uid_t>(std::stoi(user));
    } catch (...) {
        struct passwd *pwd = getpwnam(user);
        if (pwd)
            ret = pwd->pw_uid;
    }
    return ret;
}

gid_t getGid(const char *group) {
    gid_t ret = static_cast<gid_t>(-1);
    if (!group)
        return ret;
    try {
        ret = static_cast<gid_t>(std::stoi(group));
    } catch (...) {
        struct group *grp = getgrnam(group);
        if (grp)
            ret = grp->gr_gid;
    }
    return ret;
}

} /* namespace CmdlineOpts */

} /* namespace Cynara */
