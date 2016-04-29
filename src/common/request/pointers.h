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
 * @file        src/common/request/pointers.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines request classes pointers
 */

#ifndef SRC_COMMON_REQUEST_POINTERS_H_
#define SRC_COMMON_REQUEST_POINTERS_H_

#include <memory>

namespace Cynara {

class AdminCheckRequest;
typedef std::shared_ptr<AdminCheckRequest> AdminCheckRequestPtr;

class AgentActionRequest;
typedef std::shared_ptr<AgentActionRequest> AgentActionRequestPtr;

class AgentRegisterRequest;
typedef std::shared_ptr<AgentRegisterRequest> AgentRegisterRequestPtr;

class CancelRequest;
typedef std::shared_ptr<CancelRequest> CancelRequestPtr;

class CheckRequest;
typedef std::shared_ptr<CheckRequest> CheckRequestPtr;

class DescriptionListRequest;
typedef std::shared_ptr<DescriptionListRequest> DescriptionListRequestPtr;

class EraseRequest;
typedef std::shared_ptr<EraseRequest> EraseRequestPtr;

class InsertOrUpdateBucketRequest;
typedef std::shared_ptr<InsertOrUpdateBucketRequest> InsertOrUpdateBucketRequestPtr;

class ListRequest;
typedef std::shared_ptr<ListRequest> ListRequestPtr;

class MonitorGetEntriesRequest;
typedef std::shared_ptr<MonitorGetEntriesRequest> MonitorGetEntriesRequestPtr;

class MonitorGetFlushRequest;
typedef std::shared_ptr<MonitorGetFlushRequest> MonitorGetFlushRequestPtr;

class MonitorEntriesPutRequest;
typedef std::shared_ptr<MonitorEntriesPutRequest> MonitorEntriesPutRequestPtr;

class RemoveBucketRequest;
typedef std::shared_ptr<RemoveBucketRequest> RemoveBucketRequestPtr;

class Request;
typedef std::shared_ptr<Request> RequestPtr;

class RequestContext;
typedef std::shared_ptr<RequestContext> RequestContextPtr;

class RequestTaker;
typedef std::shared_ptr<RequestTaker> RequestTakerPtr;

class SetPoliciesRequest;
typedef std::shared_ptr<SetPoliciesRequest> SetPoliciesRequestPtr;

class SignalRequest;
typedef std::shared_ptr<SignalRequest> SignalRequestPtr;

class SimpleCheckRequest;
typedef std::shared_ptr<SimpleCheckRequest> SimpleCheckRequestPtr;

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_POINTERS_H_ */
