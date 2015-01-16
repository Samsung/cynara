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
#include <stdexcept>
#include <unistd.h>

#include <cynara-error.h>

#include <config/PathConfig.h>
#include <exceptions/FileNotFoundException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>

#include "ChecksumGenerator.h"

namespace Cynara {

namespace StorageConfig = PathConfig::StoragePath;

const char ChecksumGenerator::m_fieldSeparator(StorageConfig::fieldSeparator);
const char ChecksumGenerator::m_recordSeparator(StorageConfig::recordSeparator);
const std::string ChecksumGenerator::m_backupFilenameSuffix(StorageConfig::backupFilenameSuffix);

ChecksumGenerator::ChecksumGenerator(int argc, char * const *argv) : m_copyStream(std::string()) {
    m_filename = (1 < argc ? argv[1] : std::string());
}

int ChecksumGenerator::run(void) {
    try {
        openFileStream();
        copyFileStream();
        printRecord();
        return CYNARA_API_SUCCESS;
    } catch (const FileNotFoundException &ex) {
        std::cerr << ex.message() << std::endl;
        return CYNARA_API_INVALID_COMMANDLINE_PARAM;
    } catch (const UnexpectedErrorException &ex) {
        std::cerr << ex.message() << std::endl;
        return CYNARA_API_UNKNOWN_ERROR;
    } catch (const std::ios_base::failure &ex) {
        std::cerr << ex.what() << std::endl;
        return CYNARA_API_UNKNOWN_ERROR;
    } catch (const std::length_error &ex) {
        std::cerr << ex.what() << std::endl;
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

const std::string ChecksumGenerator::generate(const std::string &data) {
    auto checksum = crypt(data.c_str(), "$1$");

    if (nullptr != checksum) {
        return std::string(checksum);
    } else {
        int err = errno;
        if (err == ENOSYS) {
            LOGE("'crypt' function was not implemented; error [%d] : <%s>", err, strerror(err));
        } else {
            LOGE("'crypt' function error [%d] : <%s>", err, strerror(err));
        }
        throw UnexpectedErrorException(err, strerror(err));
    }
};

void ChecksumGenerator::openFileStream(void) {
    m_inputStream.open(m_filename);

    if (!m_inputStream.is_open()) {
        throw FileNotFoundException(m_filename);
    }
}

void ChecksumGenerator::copyFileStream(void) {
    std::copy(std::istreambuf_iterator<char>(m_inputStream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(m_copyStream));
    m_inputStream.seekg(0);
}

void ChecksumGenerator::printRecord(void) const {
    std::string filename(basename(m_filename.c_str()));
    getBasicFilename(filename);

    std::cout << filename << m_fieldSeparator << generate(m_copyStream.str())
              << m_recordSeparator;
}

void ChecksumGenerator::getBasicFilename(std::string &filename) const {
    auto backupSuffixPos = filename.rfind(m_backupFilenameSuffix);

    if (std::string::npos != backupSuffixPos &&
        filename.size() == backupSuffixPos + m_backupFilenameSuffix.size()) {
        filename.erase(backupSuffixPos);
    }
}

} /* namespace Cynara */
