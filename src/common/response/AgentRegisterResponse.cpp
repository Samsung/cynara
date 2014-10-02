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
 * @file        src/common/response/AgentRegisterResponse.cpp
 * @author      Adam  Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements class for responding to agent register request
 */

#include <memory>

#include <request/RequestTaker.h>

#include "AgentRegisterResponse.h"

namespace Cynara {

void AgentRegisterResponse::execute(ResponsePtr self, ResponseTakerPtr taker,
                                    RequestContextPtr context) const {
    taker->execute(context, std::dynamic_pointer_cast<AgentRegisterResponse>(self));
}

} // namespace Cynara
