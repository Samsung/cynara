/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/response/SimpleCheckResponse.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements simple check response class
 */

#include <attributes/attributes.h>
#include <response/ResponseTaker.h>

#include "SimpleCheckResponse.h"

namespace Cynara {

void SimpleCheckResponse::execute(const Response &self UNUSED, ResponseTaker &taker,
                                  const RequestContext &context) const {
    taker.execute(context, *this);
}

} // namespace Cynara
