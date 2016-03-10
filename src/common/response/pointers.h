/*
 * Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/response/pointers.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines response classes pointers
 */

#ifndef SRC_COMMON_RESPONSE_POINTERS_H_
#define SRC_COMMON_RESPONSE_POINTERS_H_

#include <memory>

namespace Cynara {

class AdminCheckResponse;
typedef std::shared_ptr<AdminCheckResponse> AdminCheckResponsePtr;

class AgentActionResponse;
typedef std::shared_ptr<AgentActionResponse> AgentActionResponsePtr;

class AgentRegisterResponse;
typedef std::shared_ptr<AgentRegisterResponse> AgentRegisterResponsePtr;

class CancelResponse;
typedef std::shared_ptr<CancelResponse> CancelResponsePtr;

class CheckResponse;
typedef std::shared_ptr<CheckResponse> CheckResponsePtr;

class CodeResponse;
typedef std::shared_ptr<CodeResponse> CodeResponsePtr;

class DescriptionListResponse;
typedef std::shared_ptr<DescriptionListResponse> DescriptionListResponsePtr;

class ListResponse;
typedef std::shared_ptr<ListResponse> ListResponsePtr;

class MonitorGetEntriesResponse;
typedef std::shared_ptr<MonitorGetEntriesResponse> MonitorGetEntriesResponsePtr;

class Response;
typedef std::shared_ptr<Response> ResponsePtr;

class SimpleCheckResponse;
typedef std::shared_ptr<SimpleCheckResponse> SimpleCheckResponsePtr;

class ResponseTaker;
typedef std::shared_ptr<ResponseTaker> ResponseTakerPtr;
typedef std::weak_ptr<ResponseTaker> ResponseTakerWeakPtr;

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_POINTERS_H_ */
