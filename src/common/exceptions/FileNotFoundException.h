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
 * @file        FileNotFoundException.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines exception thrown when database file is not found
 */

#ifndef SRC_COMMON_EXCEPTIONS_FILENOTFOUNDEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_FILENOTFOUNDEXCEPTION_H_

#include <string>

#include <exceptions/DatabaseException.h>

namespace Cynara {

class FileNotFoundException : public DatabaseException {
public:
    FileNotFoundException(const std::string &filename) : m_filename(filename) {};
    virtual ~FileNotFoundException() = default;

    const std::string message(void) const {
        if (m_message.empty()) {
            m_message = "File " + filename() + " not found or corrupted badly";
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

#endif /* SRC_COMMON_EXCEPTIONS_FILENOTFOUNDEXCEPTION_H_ */
