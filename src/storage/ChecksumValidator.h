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
 * @file        src/storage/ChecksumValidator.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains ChecksumValidator header.
 */

#ifndef SRC_STORAGE_CHECKSUMVALIDATOR_H_
#define SRC_STORAGE_CHECKSUMVALIDATOR_H_

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

namespace Cynara {

class ChecksumValidator;
typedef std::unique_ptr<ChecksumValidator> ChecksumValidatorUniquePtr;

class ChecksumValidator {
public:
    ChecksumValidator(const std::string &path) : m_dbPath(path) {}

    void load(std::istream &stream);
    void compare(std::istream &stream, const std::string &pathname, bool isBackupValid);

    void clear(void) {
        m_sums.clear();
    }

    static const std::string generate(const std::string &data);

protected:
    typedef std::unordered_map<std::string, std::string> Checksums;

    bool isChecksumIndex(const std::string &pathname) const;

    static const std::string parseFilename(const std::string &line, std::size_t &beginToken);
    static const std::string parseChecksum(const std::string &line, std::size_t &beginToken);

    Checksums m_sums;
    const std::string m_dbPath;
};

} // namespace Cynara

#endif // SRC_STORAGE_CHECKSUMVALIDATOR_H_
