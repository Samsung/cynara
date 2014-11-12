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
 * @file        src/common/protocol/ProtocolAdmin.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines protocol class for administration
 */

#ifndef SRC_COMMON_PROTOCOL_PROTOCOLADMIN_H_
#define SRC_COMMON_PROTOCOL_PROTOCOLADMIN_H_

#include "Protocol.h"

namespace Cynara {

class ProtocolAdmin : public Protocol {
public:
    ProtocolAdmin();
    virtual ~ProtocolAdmin();

    virtual ProtocolPtr clone(void);

    virtual RequestPtr extractRequestFromBuffer(BinaryQueue &bufferQueue);
    virtual ResponsePtr extractResponseFromBuffer(BinaryQueue &bufferQueue);

    virtual void execute(RequestContextPtr context, AdminCheckRequestPtr request);
    virtual void execute(RequestContextPtr context, InsertOrUpdateBucketRequestPtr request);
    virtual void execute(RequestContextPtr context, RemoveBucketRequestPtr request);
    virtual void execute(RequestContextPtr context, SetPoliciesRequestPtr request);

    virtual void execute(RequestContextPtr context, CheckResponsePtr response);
    virtual void execute(RequestContextPtr context, CodeResponsePtr response);

private:
    RequestPtr deserializeAdminCheckRequest(void);
    RequestPtr deserializeInsertOrUpdateBucketRequest(void);
    RequestPtr deserializeRemoveBucketRequest(void);
    RequestPtr deserializeSetPoliciesRequest(void);

    ResponsePtr deserializeCheckResponse(void);
    ResponsePtr deserializeCodeResponse(void);
};

} // namespace Cynara

#endif /* SRC_COMMON_PROTOCOL_PROTOCOLADMIN_H_ */
