/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @version     1.0
 * @brief       Implementation of BucketRecordCorruptedException
 */
#ifndef SRC_COMMON_EXCEPTIONS_BUCKETRECORDCORRUPTEDEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_BUCKETRECORDCORRUPTEDEXCEPTION_H_

#include <string>

#include <exceptions/DatabaseException.h>

namespace Cynara {

class BucketRecordCorruptedException : public DatabaseException {
public:
    BucketRecordCorruptedException(void) = delete;
    virtual ~BucketRecordCorruptedException() {};

    BucketRecordCorruptedException(const std::string &line)
        : m_lineNumber(0), m_line(line)  {}

    BucketRecordCorruptedException withLineNumber(const size_t &lineNumber) const {
        BucketRecordCorruptedException copy(*this);
        copy.m_lineNumber = lineNumber;
        copy.m_whatMsg.clear();
        return copy;
    }

    BucketRecordCorruptedException withFilename(const std::string &filename) const {
        BucketRecordCorruptedException copy(*this);
        copy.m_filename = filename;
        copy.m_whatMsg.clear();
        return copy;
    }

    virtual const std::string message(void) const {
        if (m_whatMsg.empty()) {
            m_whatMsg = "Bucket record corrupted at"
                + formatedFilename()
                + formatedLineNumber()
                + ": <" + slicedLine() + ">";
        }
        return m_whatMsg;
    }

    const std::string &filename(void) const {
        return m_filename;
    }

    const std::string &line(void) const {
        return m_line;
    }

    size_t lineNumber(void) const {
        return m_lineNumber;
    }

protected:
    inline std::string slicedLine(void) const {
        return m_line.substr(0, 50) + (m_line.size() > 50 ? "..." : "");
    }

    inline std::string formatedFilename(void) const {
        return m_filename.empty() ? " line" : " " + m_filename;
    }

    inline std::string formatedLineNumber(void) const {
        return m_lineNumber <= 0 ? ""
                : (m_filename.empty() ? " " : ":")
                  + std::to_string(static_cast<long int>(m_lineNumber));
    }

private:
    size_t m_lineNumber;
    std::string m_line;
    std::string m_filename;
    mutable std::string m_whatMsg;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_BUCKETRECORDCORRUPTEDEXCEPTION_H_ */
