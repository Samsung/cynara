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
/**
 * @file        src/service/main/main.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Main Cynara daemon file
 */

#include <exception>
#include <iostream>
#include <stdlib.h>

#include <systemd/sd-journal.h>
#include <systemd/sd-daemon.h>

#include <common.h>
#include <log/log.h>

#include "CmdlineParser.h"
#include "Cynara.h"

int main(int argc, char **argv) {
    init_log();

    try {
        if (1 < argc) {
            auto handlingSuccess = Cynara::CmdlineParser::handleCmdlineOptions(argc, argv);
            return (handlingSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
        }

        Cynara::Cynara cynara;
        LOGI("Cynara service is starting ...");
        cynara.init();
        LOGI("Cynara service is started");

        int ret = sd_notify(0, "READY=1");
        if (ret == 0) {
            LOGW("Cynara was not configured to notify its status");
        } else if (ret < 0) {
            LOGE("sd_notify failed [%d]", ret);
        }

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
