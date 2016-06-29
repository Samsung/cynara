/*
 * Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/cyad/main.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Oskar Świtalski <o.switalski@samsung.com>
 * @version     1.0
 * @brief       A command-line tool for managing Cynara's database
 */

#include <exception>
#include <iostream>

#include <cyad/Cyad.h>
#include <cynara-error.h>

int main(int argc, char **argv) {
    int result = CYNARA_API_UNKNOWN_ERROR;

    try {
        Cynara::Cyad cyad(argc, argv);
        result = cyad.run();
    } catch (const std::exception &e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Caught unknow exception" << std::endl;
    }

    return result;
}
