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
 * @file        src/common/types/ProtocolFields.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Types definition for protocol frame fields.
 */

#ifndef SRC_COMMON_TYPES_PROTOCOLFIELDS_H_
#define SRC_COMMON_TYPES_PROTOCOLFIELDS_H_

#include <string>
#include <cstddef>

namespace Cynara {

typedef uint16_t ProtocolFrameFieldsCount;
typedef uint32_t ProtocolFrameLength;
typedef uint16_t ProtocolFrameSequenceNumber;
typedef std::string ProtocolFrameSignature;
typedef uint16_t ProtocolResponseCode;

} /* namespace Cynara */

#endif /* SRC_COMMON_TYPES_PROTOCOLFIELDS_H_ */
