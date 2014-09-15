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
 * @file        src/common/exceptions/UnexpectedErrorException.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of UnexpectedErrorException
 */

#ifndef SRC_COMMON_EXCEPTIONS_UNEXPECTEDERROREXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_UNEXPECTEDERROREXCEPTION_H_

#include <exception>
#include <sstream>

#include "Exception.h"

namespace Cynara {

class UnexpectedErrorException : public Exception {
private:
    std::string m_whatMessage;

public:
    UnexpectedErrorException() = delete;
    UnexpectedErrorException(int errorCode, const char *errorMsg) {
        std::ostringstream stream;
        stream << "UnexpectedErrorException with errorCode =[" << errorCode << "] and message <";
        stream << errorMsg << ">";
        m_whatMessage = stream.str();
    }
    UnexpectedErrorException(const char *errorMsg) {
        std::ostringstream stream;
        stream << "UnexpectedErrorException with message <";
        stream << errorMsg << ">";
        m_whatMessage = stream.str();
    }

    virtual ~UnexpectedErrorException() noexcept {};

    virtual const std::string message(void) const {
        return m_whatMessage;
    }
};

} // namespace Cynara

#endif /* SRC_COMMON_EXCEPTIONS_UNEXPECTEDERROREXCEPTION_H_ */
