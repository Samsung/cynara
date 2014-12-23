/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
 * @file        src/common/protocol/ProtocolOpCode.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Declaration of protocol frame operation codes.
 */

#ifndef SRC_COMMON_TYPES_PROTOCOLOPCODE_H_
#define SRC_COMMON_TYPES_PROTOCOLOPCODE_H_

#include <cstddef>

namespace Cynara {

enum ProtocolOpCode : uint8_t {
    /** Client operations */
    OpCheckPolicyRequest = 0,
    OpCheckPolicyResponse,
    OpCancelRequest,
    OpCancelResponse,

    /** Opcodes 4 - 19 are reserved for future use */

    /** Admin operations */
    OpInsertOrUpdateBucket = 20,
    OpRemoveBucket,
    OpSetPolicies,
    OpCodeResponse,
    OpAdminCheckRequest,
    OpListRequest,
    OpListResponse,
    OpAdminCheckPolicyResponse,
    OpEraseRequest,
    OpDescriptionListRequest,
    OpDescriptionListResponse,

    /** Opcodes 31 - 39 are reserved for future use */

    /** Agent operations */
    OpAgentRegisterRequest = 40,
    OpAgentRegisterResponse,
    OpAgentActionRequest,
    OpAgentActionResponse,
};

} /* namespace Cynara */

#endif /* SRC_COMMON_TYPES_PROTOCOLOPCODE_H_ */
