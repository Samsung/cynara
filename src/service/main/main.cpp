/*
 *  Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/service/main/main.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Main Cynara daemon file
 */

#include <exception>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef BUILD_WITH_SYSTEMD_DAEMON
#include <systemd/sd-daemon.h>
#endif

#include <common.h>
#include <log/log.h>

#include "CmdlineParser.h"
#include "Cynara.h"

void daemonize(void) {
    switch (fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
        exit(EXIT_FAILURE);

    switch (fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }

    if (chdir("/") == -1)
        exit(EXIT_FAILURE);

    int fd = TEMP_FAILURE_RETRY(open("/dev/null", O_RDWR));
    if (fd < 0)
        exit(EXIT_FAILURE);

    for (auto stdFd : {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO})
        if (TEMP_FAILURE_RETRY(dup2(fd, stdFd)) != stdFd)
            exit(EXIT_FAILURE);

    switch (fd) {
        case STDIN_FILENO:
        case STDOUT_FILENO:
        case STDERR_FILENO:
            break;
        default:
            close(fd);
    }
}

int main(int argc, char **argv) {
    try {
        Cynara::CmdlineParser::CmdLineOptions options
            = Cynara::CmdlineParser::handleCmdlineOptions(argc, argv);
        if (options.m_error)
            return EXIT_FAILURE;
        if (options.m_exit)
            return EXIT_SUCCESS;

        if (options.m_daemon)
            daemonize();
        if (options.m_mask != static_cast<mode_t>(-1))
            umask(options.m_mask);
        if (options.m_uid != static_cast<uid_t>(-1))
            if (setuid(options.m_uid) == -1)
                return EXIT_FAILURE;
        if (options.m_gid != static_cast<gid_t>(-1))
            if (setgid(options.m_gid) == -1)
                return EXIT_FAILURE;

        init_log();

        Cynara::Cynara cynara;
        LOGI("Cynara service is starting ...");
        cynara.init();
        LOGI("Cynara service is started");

#ifdef BUILD_WITH_SYSTEMD_DAEMON
        int ret = sd_notify(0, "READY=1");
        if (ret == 0) {
            LOGW("Cynara was not configured to notify its status");
        } else if (ret < 0) {
            LOGE("sd_notify failed [%d]", ret);
        }
#endif

        LOGD("Starting the real job");
        cynara.run();

        LOGD("Time to clean up.");
        cynara.finalize();
        LOGD("Cynara service is stopped");
    } catch (std::exception &e) {
        LOGC("Cynara stoped because of unhandled exception: %s", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        LOGC("Cynara stoped because of unknown unhanndled exception.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
