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
 * @file        src/common/exceptions/FileLockAcquiringException.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines exception thrown when file lock cannot be acquired
 */

#ifndef SRC_COMMON_EXCEPTIONS_FILELOCKACQUIRINGEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_FILELOCKACQUIRINGEXCEPTION_H_

#include <cstring>
#include <string>

#include <exceptions/Exception.h>

namespace Cynara {

class FileLockAcquiringException {
public:
    FileLockAcquiringException(const int errNumber) : m_errno(errNumber) {
        m_message = "File lock acquiring error [" + std::to_string(errorNumber()) + "]"
                    + " <" + errorString() + ">";
    };
    virtual ~FileLockAcquiringException() {};

    const std::string &message(void) const {
        return m_message;
    }

    int errorNumber(void) const {
        return m_errno;
    }

    const std::string errorString(void) const {
        return strerror(m_errno);
    }

private:
    std::string m_message;
    int m_errno;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_FILELOCKACQUIRINGEXCEPTION_H_ */
