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
 * @file        ProtocolClient.h
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

    virtual RequestPtr extractRequestFromBuffer(BinaryQueue &bufferQueue);
    virtual ResponsePtr extractResponseFromBuffer(BinaryQueue &bufferQueue);

    virtual void execute(RequestContextPtr context, CheckRequestPtr request);
    virtual void execute(RequestContextPtr context, CheckResponsePtr response);

private:
    RequestPtr deserializeCheckRequest(ProtocolFrameHeader &frame);
    ResponsePtr deserializeCheckResponse(ProtocolFrameHeader &frame);
};

} // namespace Cynara

#endif /* SRC_COMMON_PROTOCOL_PROTOCOLCLIENT_H_ */
