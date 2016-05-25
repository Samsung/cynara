/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/exceptions/AccessDeniedException.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Implementation of AccessDeniedException
 */
#ifndef SRC_COMMON_EXCEPTIONS_ACCESSDENIEDEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_ACCESSDENIEDEXCEPTION_H_

#include "Exception.h"

namespace Cynara {

class AccessDeniedException : public Exception {
public:
    AccessDeniedException(const std::string &where) {
        m_message = "Access denied from: " + where;
    }
    virtual ~AccessDeniedException() {};

    const std::string &message(void) const {
        return m_message;
    }

private:
    std::string m_message;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_ACCESSDENIEDEXCEPTION_H_ */
