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
/*
 * @file        DescriptorNotExistsException.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of DescriptorNotExistsException
 */

#ifndef SRC_COMMON_EXCEPTIONS_DESCRIPTORNOTEXISTSEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_DESCRIPTORNOTEXISTSEXCEPTION_H_

#include <exception>
#include <sstream>
#include <string>

#include "Exception.h"

namespace Cynara {

class DescriptorNotExistsException : public Exception {
private:
    std::string m_whatMsg;

public:
    DescriptorNotExistsException() = delete;
    DescriptorNotExistsException(int desc) {
        std::ostringstream stream;
        stream << "trying access not existing descriptor [" << desc << "]";
        m_whatMsg = stream.str();
    }

    virtual ~DescriptorNotExistsException() = default;

    virtual const std::string message(void) const {
        return m_whatMsg;
    }
};

} // namespace Cynara

#endif /* SRC_COMMON_EXCEPTIONS_DESCRIPTORNOTEXISTSEXCEPTION_H_ */
