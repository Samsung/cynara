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
#include <elfutils/libdw.h>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

#include <log/log.h>

#include "Backtrace.h"

namespace Cynara {

Backtrace &Backtrace::getInstance(void) {
    static Backtrace m_instance;
    return m_instance;
}

const Dwfl_Callbacks Backtrace::m_callbacks = {
    dwfl_linux_proc_find_elf,
    dwfl_standard_find_debuginfo,
    nullptr,
    nullptr,
};

Backtrace::Backtrace() {
    init();
}

Backtrace::~Backtrace() {
    dwfl_end(m_dwfl);
}

void Backtrace::init(void) {
    m_dwfl = dwfl_begin(&m_callbacks);
    if (m_dwfl == nullptr) {
        LOGE("dwfl_begin failed! Source info won't be available in backtrace!");
        return;
    }

    if (dwfl_linux_proc_report(m_dwfl, getpid())) {
        LOGE("dwfl_linux_proc_report failed! Source info won't be available in backtrace!");
        dwfl_end(m_dwfl);
        m_dwfl = nullptr;
    }
}

void Backtrace::getSourceInfo(unw_word_t address, std::string &fileName, int &lineNumber) {
    fileName = "??";
    lineNumber = 0;

    if (m_dwfl == nullptr)
        return;

    Dwarf_Addr addr = static_cast<Dwarf_Addr>(address);

    Dwfl_Module *module = dwfl_addrmodule(m_dwfl, addr);
    if (module == nullptr)
        return;

    Dwfl_Line *line = dwfl_module_getsrc(module, addr);
    if (line == nullptr)
        return;

    const char *src = dwfl_lineinfo(line, &addr, &lineNumber, nullptr, nullptr, nullptr);
    if (src == nullptr)
        return;

    const char *compilationDirectory = "";
    const char *compilationDirectorySeparator = "";

    if (src[0] != '/') {
        compilationDirectory = dwfl_line_comp_dir(line);
        if (compilationDirectory != NULL)
            compilationDirectorySeparator = "/";
    }

    std::ostringstream fileNameStream;
    fileNameStream << compilationDirectory << compilationDirectorySeparator << src;
    fileName = fileNameStream.str();
}

const std::string Backtrace::buildBacktrace(void) {
    std::ostringstream backtrace;
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;
    char proc_name[BUFSIZ];
    unw_word_t offp;
    int status;
    std::string fileName;
    int  lineNumber;

    unw_getcontext(&uc);
    // get rid of previous function: Backtrace::getBacktrace
    unw_init_local(&cursor, &uc);
    unw_step(&cursor);
    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        unw_get_proc_name(&cursor, proc_name, sizeof(proc_name), &offp);
        char *realname = abi::__cxa_demangle(proc_name, 0, 0, &status);
        getSourceInfo(ip, fileName, lineNumber);

        backtrace << std::hex << "ip = 0x" <<  ip << ", sp = 0x" << sp
                  << ", " << (realname ? realname : proc_name)
                  << ", " << fileName
                  << ":" << std::dec << lineNumber << std::endl;

        free(realname);
    }

    return backtrace.str();
}

const std::string Backtrace::getBacktrace(void) {
    return getInstance().buildBacktrace();
}

} /* namespace Cynara */
