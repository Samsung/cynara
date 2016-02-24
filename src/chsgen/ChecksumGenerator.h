/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/chsgen/ChecksumGenerator.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       A micro-tool for computing checksums for Cynara's database contents
 */

#ifndef SRC_CHSGEN_CHECKSUMGENERATOR_H_
#define SRC_CHSGEN_CHECKSUMGENERATOR_H_

#include <fstream>
#include <sstream>
#include <string>

namespace Cynara {

class ChecksumGenerator {
public:
    ChecksumGenerator(int argc, char * const *argv);
    ~ChecksumGenerator() {}

    int run(void);

private:
    const std::string generate(const std::string &data) const;

    void openFileStream(void);
    void copyFileStream(void);
    void printRecord(void) const;
    void removeBackupSuffix(std::string &filename) const;

    std::ifstream m_inputStream;
    std::stringstream m_copyStream;
    std::string m_pathname;
    std::string m_algorithm;
    static const char m_fieldSeparator;
    static const char m_recordSeparator;
    static const std::string m_backupFilenameSuffix;
};

} /* namespace Cynara */

#endif /* SRC_CHSGEN_CHECKSUMGENERATOR_H_ */
