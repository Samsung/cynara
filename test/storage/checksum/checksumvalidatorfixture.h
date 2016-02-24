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
 * @file        test/storage/checksum/checksumvalidatorfixture.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Fixture for ChecksumValidator tests
 */

#ifndef CHECKSUMVALIDATORFIXTURE_H_
#define CHECKSUMVALIDATORFIXTURE_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <sstream>
#include <string>

class ChecksumValidatorFixture : public ::testing::Test {
protected:
    void simpleChecksumsContents(std::string &contents) {
        std::stringstream buffer;
        buffer << "_" << m_fieldSeparator << "d41d8cd98f00b204e9800998ecf8427e" << m_recordSeparator
               << "buckets" << m_fieldSeparator << "4f15a73541bf5a52e25ef4c2b990d224"
               << m_recordSeparator;

        contents = buffer.str();
    }

    static const std::string m_dbPath;
    static const std::string m_filename;
    static const std::string m_checksum;
    static const char m_fieldSeparator;
    static const char m_recordSeparator;
    static const size_t m_firstLine;
};

#endif /* CHECKSUMVALIDATORFIXTURE_H_ */
