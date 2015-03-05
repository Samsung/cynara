/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/request/CancelRequest.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file defines cancel request class
 */

#ifndef SRC_COMMON_REQUEST_CANCELREQUEST_H_
#define SRC_COMMON_REQUEST_CANCELREQUEST_H_

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class CancelRequest : public Request {
public:
    CancelRequest(ProtocolFrameSequenceNumber sequenceNumber) : Request(sequenceNumber) {
    }

    virtual ~CancelRequest() {};

    virtual void execute(const Request &self, RequestTaker &taker,
                         const RequestContext &context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_CANCELREQUEST_H_ */
