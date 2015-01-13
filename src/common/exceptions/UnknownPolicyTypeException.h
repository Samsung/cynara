/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/exceptions/UnknownPolicyTypeException.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of UnknownPolicyTypeException
 */

#ifndef SRC_COMMON_EXCEPTIONS_UNKNOWNPOLICYTYPEEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_UNKNOWNPOLICYTYPEEXCEPTION_H_

#include <exception>

#include "Exception.h"
#include "types/PolicyType.h"

namespace Cynara {

class UnknownPolicyTypeException : public Exception {
public:
    UnknownPolicyTypeException() = delete;
    UnknownPolicyTypeException(const PolicyType &policyType)
        : m_policyType(policyType), m_message("UnknownPolicyTypeException") {
    }
    virtual ~UnknownPolicyTypeException() {};

    virtual const std::string &message(void) const {
        return m_message;
    }

    const PolicyType &policyType(void) const {
        return m_policyType;
    }

private:
    PolicyType m_policyType;
    std::string m_message;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_UNKNOWNPOLICYTYPEEXCEPTION_H_ */
