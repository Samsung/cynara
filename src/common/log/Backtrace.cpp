/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
/*
 * @file        Backtrace.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of backtrace utility class.
 */

#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#include <attributes/attributes.h>
#include <log/log.h>

#include "Backtrace.h"

namespace Cynara {

Backtrace &Backtrace::getInstance(void) {
    static Backtrace m_instance;
    return m_instance;
}

Backtrace::Backtrace() :
        m_fileName(NULL),
        m_functionName(NULL), m_lineNumber(0) {
}

Backtrace::~Backtrace() {
}

void Backtrace::getSourceInfo(unw_word_t proc_address UNUSED) {
    // TODO: extract filename and line number for symbol at given address
    m_fileName = "??";
    m_functionName = "??";
    m_lineNumber = 0;
}

const std::string Backtrace::buildBacktrace(void) {
    std::ostringstream backtrace;
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;
    char proc_name[BUFSIZ];
    unw_word_t offp;
    int status;

    unw_getcontext(&uc);
    // get rid of previous function: Backtrace::getBacktrace
    unw_init_local(&cursor, &uc);
    unw_step(&cursor);
    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        unw_get_proc_name(&cursor, proc_name, sizeof(proc_name), &offp);
        char *realname = abi::__cxa_demangle(proc_name, 0, 0, &status);
        getSourceInfo(ip);

        backtrace << std::hex << "ip = 0x" <<  ip << ", sp = 0x" << sp
                  << ", " << (realname ? realname : proc_name)
                  << ", " << m_fileName
                  << ":" << std::dec << m_lineNumber << std::endl;

        free(realname);
    }

    return backtrace.str();
}

const std::string Backtrace::getBacktrace(void) {
    return getInstance().buildBacktrace();
}

} /* namespace Cynara */
