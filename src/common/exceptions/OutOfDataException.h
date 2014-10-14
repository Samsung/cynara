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
 * @file        src/common/exceptions/OutOfDataException.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of OutOfDataException
 */

#ifndef SRC_COMMON_EXCEPTIONS_OUTOFDATAEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_OUTOFDATAEXCEPTION_H_

#include <exception>
#include <sstream>
#include <string>

#include "Exception.h"

namespace Cynara {

class OutOfDataException : public Exception {
public:
    OutOfDataException() = delete;
    OutOfDataException(size_t dataRange, size_t accessTry) {
        std::ostringstream stream;
        stream << "trying access [" << accessTry << "]";
        stream << " which exceeds data range [" << dataRange << "]";
        m_whatMsg = stream.str();
    }

    virtual ~OutOfDataException() {};

    virtual const std::string message(void) const {
        return m_whatMsg;
    }

private:
    std::string m_whatMsg;
};

} // namespace Cynara

#endif /* SRC_COMMON_EXCEPTIONS_OUTOFDATAEXCEPTION_H_ */
