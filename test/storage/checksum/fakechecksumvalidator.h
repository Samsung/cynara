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
 * @file        test/storage/checksum/fakechecksumvalidator.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Mock of ChecksumValidator
 */

#ifndef FAKECHECKSUMVALIDATOR_H_
#define FAKECHECKSUMVALIDATOR_H_

#include <storage/ChecksumValidator.h>

class FakeChecksumValidator : public Cynara::ChecksumValidator {
public:
    using Cynara::ChecksumValidator::ChecksumValidator;

    const Checksums &sums(void) {
        return m_sums;
    }
};

#endif /* FAKECHECKSUMVALIDATOR_H_ */
