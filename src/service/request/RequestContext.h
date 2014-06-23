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
 * @file        RequestContext.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines request context class
 */

#ifndef SRC_SERVICE_REQUEST_REQUESTCONTEXT_H_
#define SRC_SERVICE_REQUEST_REQUESTCONTEXT_H_

#include <common.h>

#include <protocol/ResponseTaker.h>

namespace Cynara {

class RequestContext {
private:
    int m_desc;
    ResponseTaker &m_responseTaker;
    BinaryQueue &m_responseQueue;

public:
    RequestContext(int desc, ResponseTaker &responseTaker, BinaryQueue &responseQueue)
        : m_desc(desc), m_responseTaker(responseTaker), m_responseQueue(responseQueue) {
    }

    template <class T>
    void returnResponse(T &&response) const {
        m_responseTaker.appendResponseToBuffer(m_responseQueue, std::move(response));
    }
};

} // namespace Cynara

#endif /* SRC_SERVICE_REQUEST_REQUESTCONTEXT_H_ */
