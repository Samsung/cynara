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
 * @file        src/common/protocol/ProtocolClient.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines protocol class for communication with client
 */

#ifndef SRC_COMMON_PROTOCOL_PROTOCOLCLIENT_H_
#define SRC_COMMON_PROTOCOL_PROTOCOLCLIENT_H_

#include <protocol/ProtocolFrameHeader.h>
#include <request/pointers.h>
#include <response/pointers.h>

#include "Protocol.h"

namespace Cynara {

class ProtocolClient : public Protocol {
public:
    ProtocolClient();
    virtual ~ProtocolClient();

    virtual ProtocolPtr clone(void);

    virtual RequestPtr extractRequestFromBuffer(BinaryQueuePtr bufferQueue);
    virtual ResponsePtr extractResponseFromBuffer(BinaryQueuePtr bufferQueue);

    virtual void execute(const RequestContext &context, const CancelRequest &request);
    virtual void execute(const RequestContext &context, const CheckRequest &request);
    virtual void execute(const RequestContext &context, const SimpleCheckRequest &request);

    virtual void execute(const RequestContext &context, const CancelResponse &response);
    virtual void execute(const RequestContext &context, const CheckResponse &response);
    virtual void execute(const RequestContext &context, const SimpleCheckResponse &request);

private:
    RequestPtr deserializeCancelRequest(void);
    RequestPtr deserializeCheckRequest(void);
    RequestPtr deserializeSimpleCheckRequest(void);

    ResponsePtr deserializeCancelResponse(void);
    ResponsePtr deserializeCheckResponse(void);
    ResponsePtr deserializeSimpleCheckResponse(void);
};

} // namespace Cynara

#endif /* SRC_COMMON_PROTOCOL_PROTOCOLCLIENT_H_ */
