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
 * @file        CannotCreateFileException.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines exception thrown when database file cannot be created
 */

#ifndef SRC_COMMON_EXCEPTIONS_CANNOTCREATEFILEEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_CANNOTCREATEFILEEXCEPTION_H_

#include <string>

#include <exceptions/DatabaseException.h>

namespace Cynara {

class CannotCreateFileException : public DatabaseException {
public:
    CannotCreateFileException(const std::string &filename) : m_filename(filename) {};
    virtual ~CannotCreateFileException() = default;

    const std::string message(void) const {
        if (m_message.empty()) {
            m_message = "File " + filename() + " cannot be created";
        }
        return m_message;
    }

    const std::string &filename(void) const {
        return m_filename;
    }

private:
    mutable std::string m_message;
    std::string m_filename;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_CANNOTCREATEFILEEXCEPTION_H_ */
