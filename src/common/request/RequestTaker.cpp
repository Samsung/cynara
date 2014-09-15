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
 * @file        src/common/request/RequestTaker.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements RequestTaker class
 */

#include <attributes/attributes.h>
#include <exceptions/NotImplementedException.h>
#include <request/RequestContext.h>

#include "RequestTaker.h"

namespace Cynara {

void RequestTaker::execute(RequestContextPtr context UNUSED, AdminCheckRequestPtr request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(RequestContextPtr context UNUSED, CheckRequestPtr request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(RequestContextPtr context UNUSED,
                           InsertOrUpdateBucketRequestPtr request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(RequestContextPtr context UNUSED,
                           RemoveBucketRequestPtr request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(RequestContextPtr context UNUSED, SetPoliciesRequestPtr request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(RequestContextPtr context UNUSED, SignalRequestPtr request UNUSED) {
    throw NotImplementedException();
}

} // namespace Cynara
