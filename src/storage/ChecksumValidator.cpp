/*
 *  Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/storage/ChecksumValidator.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains ChecksumValidator implementation.
 */

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <new>
#include <sstream>

#include <config/PathConfig.h>
#include <exceptions/ChecksumRecordCorruptedException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <md5wrapper.h>

#include "ChecksumValidator.h"

namespace Cynara {

void ChecksumValidator::load(std::istream &stream) {
    m_sums.clear();

    std::string line;
    std::size_t lineNum = 1;
    while (std::getline(stream, line, PathConfig::StoragePath::recordSeparator)) {
        try {
            std::size_t beginToken = 0;
            std::string filename = parseFilename(line, beginToken);
            std::string checksum = parseChecksum(line, beginToken);

            m_sums.insert({ filename, checksum });
            ++lineNum;
        } catch (const ChecksumRecordCorruptedException &ex) {
            throw ex.withLineNumber(lineNum);
        }
    }
};

const std::string ChecksumValidator::generate(const std::string &data) {
    return generateMD5(data);
}

void ChecksumValidator::compare(std::istream &stream, const std::string &pathname,
                                bool isBackupValid) {
    if (isChecksumIndex(pathname)) {
        return;
    }

    std::unique_ptr<char, decltype(free)*> pathnameDuplicate(strdup(pathname.c_str()), free);
    if (pathnameDuplicate == nullptr) {
        LOGE("Insufficient memory available to allocate duplicate filename: <%s>",
             pathname.c_str());
        throw std::bad_alloc();
    }

    std::string filename(::basename(pathnameDuplicate.get()));
    std::stringstream copyStream;

    if (isBackupValid) {
        auto backupSuffixPos = filename.rfind(PathConfig::StoragePath::backupFilenameSuffix);
        size_t suffixSize = PathConfig::StoragePath::backupFilenameSuffix.size();

        if ((std::string::npos != backupSuffixPos) &&
            (filename.size() == (backupSuffixPos + suffixSize))) {
            filename.erase(backupSuffixPos);
        }
    }

    std::copy(std::istreambuf_iterator<char>(stream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(copyStream));
    stream.seekg(0);

    if (m_sums[filename] != generate(copyStream.str())) {
        throw ChecksumRecordCorruptedException(m_sums[filename]);
    }
};

const std::string ChecksumValidator::parseFilename(const std::string &line,
                                                   std::size_t &beginToken) {
    std::size_t filenameEndToken = line.find(PathConfig::StoragePath::fieldSeparator, beginToken);
    if (filenameEndToken != beginToken && filenameEndToken != std::string::npos) {
        auto filename = line.substr(beginToken, filenameEndToken - beginToken);
        beginToken = filenameEndToken + 1;
        return filename;
    }
    throw ChecksumRecordCorruptedException(line);
}

const std::string ChecksumValidator::parseChecksum(const std::string &line,
                                                   std::size_t &beginToken) {
    if (beginToken >= line.size()) {
        throw ChecksumRecordCorruptedException(line);
    }

    auto checksum = line.substr(beginToken);
    beginToken = line.size();
    return checksum;
}

bool ChecksumValidator::isChecksumIndex(const std::string &filename) const {
    auto checksum = m_dbPath + PathConfig::StoragePath::checksumFilename;
    return (filename == checksum ||
            filename == checksum + PathConfig::StoragePath::backupFilenameSuffix);
}

} // namespace Cynara
