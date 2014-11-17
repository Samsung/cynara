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
 * @file        src/common/exceptions/InvalidProtocolException.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of InvalidProtocolException
 */

#ifndef SRC_COMMON_EXCEPTIONS_INVALIDPROTOCOLEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_INVALIDPROTOCOLEXCEPTION_H_

#include <exception>
#include <sstream>

#include "Exception.h"

namespace Cynara {

class InvalidProtocolException : public Exception {
public:
    enum ExceptionType {
        InvalidSignature,
        WrongOpCode,
        Other
    };

    InvalidProtocolException(ExceptionType exceptionType) :
        m_exceptionType(exceptionType) {
        switch(m_exceptionType) {
            case InvalidSignature:
                m_message = "No valid signature found";
                break;
            case WrongOpCode:
                m_message = "Wrong request code";
                break;
            case Other:
                m_message = "Unknown problem";
                break;
        }
    }

    virtual ~InvalidProtocolException() {};

    virtual const std::string &message(void) const {
        return m_message;
    }

    ExceptionType exceptionType(void) const {
        return m_exceptionType;
    }

private:
    std::string m_message;
    ExceptionType m_exceptionType;
};

} // namespace Cynara

#endif /* SRC_COMMON_EXCEPTIONS_INVALIDPROTOCOLEXCEPTION_H_ */
