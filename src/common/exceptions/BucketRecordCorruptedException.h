/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/exceptions/BucketRecordCorruptedException.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Implementation of BucketRecordCorruptedException
 */

#ifndef SRC_COMMON_EXCEPTIONS_BUCKETRECORDCORRUPTEDEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_BUCKETRECORDCORRUPTEDEXCEPTION_H_

#include <string>

#include <exceptions/RecordCorruptedException.h>

namespace Cynara {

class BucketRecordCorruptedException : public RecordCorruptedException {
public:
    BucketRecordCorruptedException() = delete;
    virtual ~BucketRecordCorruptedException() {};

    BucketRecordCorruptedException(const std::string &line) : RecordCorruptedException(line) {
        setMessage(subtype());
    }

    BucketRecordCorruptedException withLineNumber(const size_t &lineNumber) const {
        BucketRecordCorruptedException copy(*this);
        copy.m_lineNumber = lineNumber;
        copy.setMessage(copy.subtype());
        return copy;
    }

    BucketRecordCorruptedException withFilename(const std::string &filename) const {
        BucketRecordCorruptedException copy(*this);
        copy.m_filename = filename;
        copy.setMessage(copy.subtype());
        return copy;
    }

private:
    const std::string &subtype(void) {
        static const std::string subtype("Bucket");
        return subtype;
    }
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_BUCKETRECORDCORRUPTEDEXCEPTION_H_ */
