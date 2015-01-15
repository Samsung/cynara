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
 * @file        src/storage/ChecksumStream.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains ChecksumStream header.
 */

#ifndef SRC_STORAGE_CHECKSUMSTREAM_H_
#define SRC_STORAGE_CHECKSUMSTREAM_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace Cynara {

class ChecksumStream {
public:
    ChecksumStream(const std::string &filename, const std::shared_ptr<std::ofstream> &stream)
    : m_chsStream(stream), m_filename(filename) {
    }
    ~ChecksumStream();

    template<typename Type>
    ChecksumStream& operator<<(const Type &item);
    ChecksumStream& operator<<(std::ostream& (*manip)(std::ostream&));

    void open(const std::string &filename, std::ios_base::openmode mode);
    bool is_open(void) const;
    std::ios_base::fmtflags flags(void) const;
    std::ios_base::fmtflags flags(std::ios_base::fmtflags flags);

private:
    void save(void);

    std::shared_ptr<std::ofstream> m_chsStream;
    std::ofstream m_outStream;
    std::stringstream m_bufStream;
    const std::string m_filename;
    static const char m_fieldSeparator;
    static const char m_recordSeparator;
};

template<typename Type>
ChecksumStream& ChecksumStream::operator<<(const Type& item) {
    m_bufStream << item;
    m_outStream << item;
    return *this;
}

} // namespace Cynara

#endif // SRC_STORAGE_CHECKSUMSTREAM_H_
