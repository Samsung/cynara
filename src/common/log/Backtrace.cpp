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

#include "Backtrace.h"

#include <cxxabi.h>
#include <log/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace Cynara {

Backtrace &Backtrace::getInstance(void) {
    static Backtrace m_instance(NULL);
    return m_instance;
}

Backtrace::Backtrace(bfd *abfd) :
        m_bfd(abfd), m_found(false), m_pc(0), m_fileName(NULL),
        m_functionName(NULL), m_lineNumber(0) {
}

Backtrace::~Backtrace() {
    if (m_bfd) {
        bfd_close(m_bfd);
        LOGD("Binary file closed.");
    }
}

bool Backtrace::init(void) {
    char exePath[BUFSIZ];
    readlink("/proc/self/exe", exePath, BUFSIZ);

    bfd_init();
    m_bfd = bfd_openr(exePath, NULL);
    if (m_bfd) {
        m_bfd->flags |= BFD_DECOMPRESS;
        if (bfd_check_format_matches(m_bfd, bfd_object, 0)) {
            return true;
        }
        bfd_close(m_bfd);
        m_bfd = NULL;
        LOGE("Binary file check format failed.");
    } else {
        LOGE("Failed to open file: %s", exePath);
    }

    return false;
}

void Backtrace::findAddressInSection(bfd *abfd, asection *section, void *data) {
    bfd_vma vma;
    bfd_size_type size;

    Backtrace *backtrace = static_cast<Backtrace*>(data);

    if (backtrace->m_found) {
        return;
    }

    if ((bfd_get_section_flags (abfd, section) & SEC_ALLOC) == 0) {
        return;
    }

    vma = bfd_get_section_vma(abfd, section);
    if (backtrace->m_pc < vma) {
        return;
    }

    size = bfd_get_section_size(section);
    if (backtrace->m_pc >= vma + size) {
        return;
    }

    backtrace->m_found = bfd_find_nearest_line(abfd, section, NULL,
            backtrace->m_pc - vma, &backtrace->m_fileName,
            &backtrace->m_functionName, &backtrace->m_lineNumber);
}

void Backtrace::getSourceInfo(unw_word_t proc_address) {
    char addr[64];

    sprintf(addr, "0x%lx", static_cast<long unsigned int>(proc_address));
    m_pc = bfd_scan_vma(addr, NULL, 16);
    m_found = false;
    bfd_map_over_sections(m_bfd, findAddressInSection, this);

    if (m_found) {
        while (true) {
            m_found = bfd_find_inliner_info(m_bfd, &m_fileName, &m_functionName,
                    &m_lineNumber);
            if (!m_found)
                break;
        }
    } else {
        m_fileName = "??";
        m_functionName = "??";
        m_lineNumber = 0;
    }
}

const std::string Backtrace::buildBacktrace(void) {
    std::string backtrace;
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;
    char proc_name[BUFSIZ];
    char btstr[BUFSIZ];
    unw_word_t offp;
    char *realname;
    int status;

    if (m_bfd == NULL) {
        init();
    }

    unw_getcontext(&uc);
    // get rid of previous function: Backtrace::getBacktrace
    unw_init_local(&cursor, &uc);
    unw_step(&cursor);
    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
        unw_get_proc_name(&cursor, proc_name, sizeof(proc_name), &offp);
        realname = abi::__cxa_demangle(proc_name, 0, 0, &status);
        getSourceInfo(ip);

        snprintf(btstr, sizeof(btstr), "ip = %lx, sp = %lx, %s, %s:%d\n",
                (long) ip, (long) sp, realname ? realname : proc_name,
                m_fileName, m_lineNumber);
        backtrace += btstr;
    }

    return backtrace;
}

const std::string Backtrace::getBacktrace(void) {
    return getInstance().buildBacktrace();
}

} /* namespace Cynara */
