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
/*
 * @file        pointers.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines request classes pointers
 */

#ifndef SRC_COMMON_REQUEST_POINTERS_H_
#define SRC_COMMON_REQUEST_POINTERS_H_

#include <memory>

namespace Cynara {

class CheckRequest;
typedef std::shared_ptr<CheckRequest> CheckRequestPtr;

class Request;
typedef std::shared_ptr<Request> RequestPtr;

class RequestContext;
typedef std::shared_ptr<RequestContext> RequestContextPtr;

class RequestTaker;
typedef std::shared_ptr<RequestTaker> RequestTakerPtr;

class SignalRequest;
typedef std::shared_ptr<SignalRequest> SignalRequestPtr;

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_POINTERS_H_ */
