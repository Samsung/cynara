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
 * @file        src/chsgen/ChecksumGenerator.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       A micro-tool for computing checksums for Cynara's database contents
 */

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include <cynara-error.h>

#include "ChecksumGenerator.h"

namespace Cynara {

const char ChecksumGenerator::m_fieldSeparator(';');
const char ChecksumGenerator::m_recordSeparator('\n');
const std::string ChecksumGenerator::m_backupFilenameSuffix("~");

ChecksumGenerator::ChecksumGenerator(int argc, char * const *argv) {
    if (argc > 1) {
        m_pathname = argv[1];
    }
}

int ChecksumGenerator::run(void) {
    try {
        openFileStream();
        copyFileStream();
        printRecord();
        return CYNARA_API_SUCCESS;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

const std::string ChecksumGenerator::generate(const std::string &data) {
    const char *checksum = crypt(data.c_str(), "$1$");

    if (nullptr != checksum) {
        return std::string(checksum);
    } else {
        int err = errno;
        if (err == ENOSYS) {
            std::cerr << "'crypt' function was not implemented; error [" << err << "] : <"
                << strerror(err) << ">" << std::endl;
        } else {
            std::cerr << "'crypt' function error [" << err << "] : <" << strerror(err) << ">"
                << std::endl;
        }
        throw std::runtime_error(strerror(err));
    }
};

void ChecksumGenerator::openFileStream(void) {
    m_inputStream.open(m_pathname);

    if (!m_inputStream.is_open()) {
        throw std::runtime_error(std::string("File not found: ") + m_pathname);
    }
}

void ChecksumGenerator::copyFileStream(void) {
    std::copy(std::istreambuf_iterator<char>(m_inputStream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(m_copyStream));
    m_inputStream.seekg(0);
}

void ChecksumGenerator::printRecord(void) const {
    std::unique_ptr<char, decltype(free)*> pathnameDuplicate(strdup(m_pathname.c_str()), free);
    if (pathnameDuplicate == nullptr) {
        std::cerr << "Insufficient memory available to allocate duplicate filename: <"
            << m_pathname << ">" << std::endl;
        throw std::bad_alloc();
    }

    std::string basename(::basename(pathnameDuplicate.get()));
    removeBackupSuffix(basename);

    std::cout << basename << m_fieldSeparator << generate(m_copyStream.str())
              << m_recordSeparator;
}

void ChecksumGenerator::removeBackupSuffix(std::string &filename) const {
    auto backupSuffixPos = filename.rfind(m_backupFilenameSuffix);

    if (std::string::npos != backupSuffixPos &&
        filename.size() == backupSuffixPos + m_backupFilenameSuffix.size()) {
        filename.erase(backupSuffixPos);
    }
}

} /* namespace Cynara */
