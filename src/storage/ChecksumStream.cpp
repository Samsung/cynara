/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/storage/ChecksumStream.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains ChecksumStream implementation.
 */

#include <exception>
#include <unistd.h>

#include <config/PathConfig.h>

#include <storage/ChecksumValidator.h>

#include "ChecksumStream.h"

namespace Cynara {

const char ChecksumStream::m_fieldSeparator(PathConfig::StoragePath::fieldSeparator);
const char ChecksumStream::m_recordSeparator(PathConfig::StoragePath::recordSeparator);

ChecksumStream::~ChecksumStream() {
    if (!std::uncaught_exception()) {
        save();
    }
}

ChecksumStream& ChecksumStream::operator<<(std::ostream& (*manip)(std::ostream&)) {
    m_bufStream << manip;
    m_outStream << manip;
    return *this;
}

void ChecksumStream::open(const std::string &filename, std::ios_base::openmode mode) {
    m_outStream.open(filename, mode);
}

bool ChecksumStream::is_open(void) const {
    return m_outStream.is_open();
}

std::ios_base::fmtflags ChecksumStream::flags(void) const {
    return m_outStream.flags();
}

std::ios_base::fmtflags ChecksumStream::flags(std::ios_base::fmtflags flags) {
    return m_outStream.flags(flags);
}

void ChecksumStream::save() {
    m_outStream.close();
    *m_chsStream << m_filename << m_fieldSeparator << ChecksumValidator::generate(m_bufStream.str())
                 << m_recordSeparator;
}

} // namespace Cynara
