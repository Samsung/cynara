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
 * @file        test/chsgen/checksumgenerator.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Tests of ChecksumGenerator
 */

#include <string>

#include <cynara-error.h>

#include <config/PathConfig.h>

#include <chsgen/ChecksumGenerator.h>

#include "ChsgenCommandlineTest.h"

namespace {

const std::string execName("./cynara-db-chsgen");
const std::string backupFilenameSuffix(Cynara::PathConfig::StoragePath::backupFilenameSuffix);
const char fieldSeparator(Cynara::PathConfig::StoragePath::fieldSeparator);

} // namespace

/**
 * @brief   Verify if passing no filenames to checksum generator returns error message
 * @test    Expected result:
 * - CYNARA_API_INVALID_COMMANDLINE_PARAM returned from checksum generator
 * - empty output stream
 * - message from caught exception in error stream
 */
TEST_F(ChsgenCommandlineTest, noFile) {
    using ::testing::StartsWith;

    std::string err;
    std::string out;

    clearOutput();
    prepare_argv({ execName });

    Cynara::ChecksumGenerator chsgen(this->argc(), this->argv());
    const auto ret = chsgen.run();
    getOutput(out, err);

    ASSERT_EQ(CYNARA_API_INVALID_COMMANDLINE_PARAM, ret);
    ASSERT_TRUE(out.empty());
    ASSERT_THAT(err, StartsWith("File " + std::string() + " not found or corrupted badly"));
}

/**
 * @brief   Verify if checksum generator returns valid records
 * @test    Expected result:
 * - CYNARA_API_SUCCESS returned from checksum generator
 * - valid record in output stream
 * - empty error stream
 */
TEST_F(ChsgenCommandlineTest, recordGeneration) {
    using ::testing::StartsWith;

    std::string err;
    std::string out;

    for (const std::string &file : { "_", "buckets" }) {
        clearOutput();
        prepare_argv({ execName, std::string(CYNARA_TESTS_DIR) + "/db3/" + file });
        SCOPED_TRACE(file);

        Cynara::ChecksumGenerator chsgen(this->argc(), this->argv());
        const auto ret = chsgen.run();
        getOutput(out, err);

        ASSERT_EQ(CYNARA_API_SUCCESS, ret);
        ASSERT_THAT(out, StartsWith(file + fieldSeparator + "$1$"));
        ASSERT_TRUE(err.empty());
    }
}

/**
 * @brief   Verify if checksum generator returns valid records for backup files
 * @test    Expected result:
 * - CYNARA_API_SUCCESS returned from checksum generator
 * - valid record in output stream
 * - empty error stream
 */
TEST_F(ChsgenCommandlineTest, suffixErasing) {
    using ::testing::StartsWith;

    std::string err;
    std::string out;

    for (const std::string &file : { "_", "_additional", "buckets" }) {
        clearOutput();
        prepare_argv({ execName,
                       std::string(CYNARA_TESTS_DIR) + "/db6/" + file + backupFilenameSuffix });
        SCOPED_TRACE(file);

        Cynara::ChecksumGenerator chsgen(this->argc(), this->argv());
        const auto ret = chsgen.run();
        getOutput(out, err);

        ASSERT_EQ(CYNARA_API_SUCCESS, ret);
        ASSERT_THAT(out, StartsWith(file + fieldSeparator + "$1$"));
        ASSERT_TRUE(err.empty());
    }
}
