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
 * @file        RequestContext.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines request context class
 */

#ifndef SRC_COMMON_REQUEST_REQUESTCONTEXT_H_
#define SRC_COMMON_REQUEST_REQUESTCONTEXT_H_

#include <memory>

#include <containers/BinaryQueue.h>
#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>
#include <response/ResponseTaker.h>

namespace Cynara {

class RequestContext {
private:
    ResponseTakerWeakPtr m_responseTaker;
    BinaryQueue &m_responseQueue;

public:
    RequestContext(ResponseTakerPtr responseTaker, BinaryQueue &responseQueue)
        : m_responseTaker(responseTaker), m_responseQueue(responseQueue) {
    }

    void returnResponse(RequestContextPtr self, ResponsePtr response) const {
        ResponseTakerPtr taker = m_responseTaker.lock();
        if (taker)
            response->execute(response, taker, self);
    }

    BinaryQueue &responseQueue(void) const {
        return m_responseQueue;
    }
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_REQUESTCONTEXT_H_ */
