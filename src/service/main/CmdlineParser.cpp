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

#include <getopt.h>
#include <iostream>
#include <sstream>

#include "CmdlineParser.h"

namespace Cynara {

namespace CmdlineParser {

std::ostream &operator<<(std::ostream &os, CmdlineOpt opt) {
    return os << static_cast<char>(opt);
}

bool handleCmdlineOptions(int argc, char * const *argv) {
    const std::string execName(argv[0]);
    std::stringstream shortOpts;
    shortOpts << ":" << CmdlineOpt::Help << CmdlineOpt::Version;

    const struct option longOpts[] = {
        { "help",       no_argument,    NULL, CmdlineOpt::Help },
        { "version",    no_argument,    NULL, CmdlineOpt::Version },
        { NULL, 0, NULL, 0 }
    };


    optind = 0; // On entry to `getopt', zero means this is the first call; initialize.
    int opt;
    while ((opt = getopt_long(argc, argv, shortOpts.str().c_str(), longOpts, nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Help:
                printHelp(execName);
                return true;
            case CmdlineOpt::Version:
                printVersion();
                return true;
            case '?': // Unknown option
            default:
                printUnknownOption(execName);
                return false;
        }
    }

    printNoOptions(execName);
    return false;
}

void printHelp(const std::string &execName) {
    std::cout << "Usage: " << execName << " [OPTIONS]" << std::endl << std::endl;
    std::cout << "  -V, --version                  print version of " << execName << " and exit"
              << std::endl;
    std::cout << "  -h, --help                     print this help message and exit" << std::endl;
}

void printVersion(void) {
    std::cout << std::string(CYNARA_VERSION) << std::endl;
}

void printUnknownOption(const std::string &execName) {
    std::cerr << "Unknown option" << std::endl;
    printHelp(execName);
}

void printNoOptions(const std::string &execName) {
    std::cerr << "No options given" << std::endl;
    printHelp(execName);
}

} /* namespace CmdlineOpts */

} /* namespace Cynara */
