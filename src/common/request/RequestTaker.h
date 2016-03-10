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
 * @file        src/common/request/RequestTaker.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines RequestTaker class
 */

#ifndef SRC_COMMON_REQUEST_REQUESTTAKER_H_
#define SRC_COMMON_REQUEST_REQUESTTAKER_H_

#include <request/pointers.h>

namespace Cynara {

class RequestTaker {
public:
    RequestTaker() = default;
    virtual ~RequestTaker() {};

    virtual void execute(const RequestContext &context, const AdminCheckRequest &request);
    virtual void execute(const RequestContext &context, const AgentActionRequest &request);
    virtual void execute(const RequestContext &context, const AgentRegisterRequest &request);
    virtual void execute(const RequestContext &context, const CancelRequest &request);
    virtual void execute(const RequestContext &context, const CheckRequest &request);
    virtual void execute(const RequestContext &context, const DescriptionListRequest &request);
    virtual void execute(const RequestContext &context, const EraseRequest &request);
    virtual void execute(const RequestContext &context, const InsertOrUpdateBucketRequest &request);
    virtual void execute(const RequestContext &context, const ListRequest &request);
    virtual void execute(const RequestContext &context, const MonitorGetEntriesRequest &request);
    virtual void execute(const RequestContext &context, const MonitorGetFlushRequest &request);
    virtual void execute(const RequestContext &context, const RemoveBucketRequest &request);
    virtual void execute(const RequestContext &context, const SetPoliciesRequest &request);
    virtual void execute(const RequestContext &context, const SignalRequest &request);
    virtual void execute(const RequestContext &context, const SimpleCheckRequest &request);

    virtual void contextClosed(const RequestContext &context);
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_REQUESTTAKER_H_ */
