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
 * @file        src/common/response/CodeResponse.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file defines class for responding to a request with a code
 */

#ifndef SRC_COMMON_RESPONSE_CODERESPONSE_H_
#define SRC_COMMON_RESPONSE_CODERESPONSE_H_

#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>

namespace Cynara {

class CodeResponse : public Response {
public:
    enum Code {
        OK,
        NO_BUCKET,
        NO_POLICY_TYPE,
        NOT_ALLOWED,
        FAILED,
        DB_CORRUPTED
    };

    const Code m_code;

    CodeResponse(Code code, ProtocolFrameSequenceNumber sequenceNumber) :
        Response(sequenceNumber), m_code(code) {
    }

    virtual ~CodeResponse() {};

    virtual void execute(ResponseTaker &taker, const RequestContext &context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_CODERESPONSE_H_ */
