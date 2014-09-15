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
 * @file        CheckRequest.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines check request class
 */

#ifndef SRC_COMMON_REQUEST_CHECKREQUEST_H_
#define SRC_COMMON_REQUEST_CHECKREQUEST_H_

#include <types/PolicyKey.h>

#include <request/pointers.h>
#include <request/Request.h>
#include <request/RequestTaker.h>

namespace Cynara {

class CheckRequest : public Request {
private:
    PolicyKey m_key;

public:
    CheckRequest(const PolicyKey &key, ProtocolFrameSequenceNumber sequenceNumber) :
        Request(sequenceNumber), m_key(key) {
    }

    virtual ~CheckRequest() {};

    const PolicyKey &key(void) const {
        return m_key;
    }

    virtual void execute(RequestPtr self, RequestTakerPtr taker, RequestContextPtr context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_CHECKREQUEST_H_ */
