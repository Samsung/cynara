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
 * @file        src/common/list/ListRequest.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements policies list request class
 */

#include <memory>

#include <request/RequestTaker.h>

#include "ListRequest.h"

namespace Cynara {

void ListRequest::execute(RequestPtr self, RequestTakerPtr taker, RequestContextPtr context) const {
    taker->execute(context, std::dynamic_pointer_cast<ListRequest>(self));
}

} // namespace Cynara
