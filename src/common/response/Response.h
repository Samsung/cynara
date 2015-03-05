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
 * @file        src/common/response/Response.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines response base class
 */

#ifndef SRC_COMMON_RESPONSE_RESPONSE_H_
#define SRC_COMMON_RESPONSE_RESPONSE_H_

#include <response/pointers.h>
#include <types/ProtocolFields.h>

namespace Cynara {

class Response {
public:
    Response(ProtocolFrameSequenceNumber sequenceNumber) : m_sequenceNumber(sequenceNumber) {
    };
    virtual ~Response() {};

    virtual void execute(const Response &self, ResponseTaker &taker,
                         const RequestContext &context) const = 0;

    ProtocolFrameSequenceNumber sequenceNumber(void) const {
        return m_sequenceNumber;
    }

private:
    ProtocolFrameSequenceNumber m_sequenceNumber;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_RESPONSE_H_ */
