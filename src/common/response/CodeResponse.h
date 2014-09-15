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
 * @file        CodeResponse.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
        NOT_ALLOWED
    };

    const Code m_code;

    CodeResponse(Code code, ProtocolFrameSequenceNumber sequenceNumber) :
        Response(sequenceNumber), m_code(code) {
    }

    virtual ~CodeResponse() {};

    virtual void execute(ResponsePtr self, ResponseTakerPtr taker,
                         RequestContextPtr context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_CODERESPONSE_H_ */
