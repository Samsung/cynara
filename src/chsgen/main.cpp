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
 * @file        src/chsgen/main.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       A micro-tool for computing checksums for Cynara's database contents
 */

#include <iostream>
#include <new>

#include <cynara-error.h>

#include <chsgen/ChecksumGenerator.h>

int main(int argc, char **argv) {
    if (2 != argc) {
        std::cerr << "Invalid commandline parameters for chsgen" << std::endl;
        return CYNARA_API_INVALID_COMMANDLINE_PARAM;
    }

    try {
        Cynara::ChecksumGenerator chsgen(argc, argv);
        return chsgen.run();
    } catch (const std::bad_alloc &) {
        std::cerr << "Chsgen could not allocate memory" << std::endl;
        return CYNARA_API_OUT_OF_MEMORY;
    }
}
