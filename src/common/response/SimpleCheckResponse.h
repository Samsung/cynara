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
 * @file        src/common/response/SimpleCheckResponse.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines response class for simple check request
 */

#ifndef SRC_COMMON_RESPONSE_SIMPLECHECKRESPONSE_H_
#define SRC_COMMON_RESPONSE_SIMPLECHECKRESPONSE_H_

#include <stdint.h>

#include <types/PolicyResult.h>

#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>

namespace Cynara {

class SimpleCheckResponse : public Response {
public:
    SimpleCheckResponse(int32_t retValue, const PolicyResult &result,
            ProtocolFrameSequenceNumber sequenceNumber) :
        Response(sequenceNumber), m_resultRef(result), m_retValue(retValue) {
    }

    SimpleCheckResponse(int retValue, ProtocolFrameSequenceNumber sequenceNumber);

    PolicyResult getResult(void) const {
        return m_resultRef;
    }

    int32_t getReturnValue(void) const {
        return m_retValue;
    }
    virtual ~SimpleCheckResponse() {}

    virtual void execute(ResponseTaker &taker, const RequestContext &context) const;
private:
    const PolicyResult m_resultRef;
    const int32_t m_retValue;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_SIMPLECHECKRESPONSE_H_ */
