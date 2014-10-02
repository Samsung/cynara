/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/agent/api/ApiInterface.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains libcynara-agent API interface definition.
 */

#ifndef SRC_AGENT_API_APIINTERFACE_H_
#define SRC_AGENT_API_APIINTERFACE_H_

#include <containers/RawBuffer.h>
#include <response/pointers.h>
#include <types/Agent.h>
#include <types/ProtocolFields.h>

namespace Cynara {

class ApiInterface {
public:
    ApiInterface() = default;
    virtual ~ApiInterface() {};

    virtual int getRequest(AgentActionResponsePtr &actionResponsePtr) = 0;

    virtual int putResponse(AgentRequestType requestType,
                            ProtocolFrameSequenceNumber sequenceNumber,
                            const RawBuffer &pluginData) = 0;
};

} // namespace Cynara

#endif /* SRC_AGENT_API_APIINTERFACE_H_ */
