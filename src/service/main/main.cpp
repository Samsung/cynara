/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        main.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Main Cynara daemon file
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <log/log.h>

#include <systemd/sd-journal.h>
#include <systemd/sd-daemon.h>

// Temporary "exit tool"
// After implementing the real main we must provide better way to inform daemon
// about our will.
static int goodbye = 0;

// Handle kill message from systemd
// Typically signal SIGTERM is used - depends on configuration in service file
void kill_handler(int sig) {
    (void) sig;
    LOGD("Cynara service is going down now");
    // As said above, this exitting way should be changed
    goodbye = 1;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    int ret;
    struct sigaction act;
    int it = 0; // TODO: remove me soon

    LOGI("Cynara service is started");

    // Install kill handler - TERM signal will be delivered form systemd to kill this service
    memset(&act, 0, sizeof(act));
    act.sa_handler = &kill_handler;
    if ((ret = sigaction(SIGTERM, &act, NULL)) < 0) {
        LOGE("sigaction failed [%d]", ret);
        return 1;
    }

    init_log();

    // Do some initialization here
    // Now notify systemd that cynara is ready to fulfil its destiny
    ret = sd_notify(0, "READY=1");
    if (ret == 0) {
        LOGW("Cynara was not configured to notify its status");
    } else if (ret < 0) {
        LOGE("sd_notify failed [%d]", ret);
    }

    LOGD("Starting the real job");

    while (!goodbye) {
        TEMP_FAILURE_RETRY(sleep(1));

        LOGD("Iteration: % 5d", it++);
        // TODO: remove me soon
    }

    LOGD("Cynara service is stopped");

    return 0;
}

