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
 * @file        src/common/response/ResponseTaker.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines ResponseTaker class
 */

#ifndef SRC_COMMON_RESPONSE_RESPONSETAKER_H_
#define SRC_COMMON_RESPONSE_RESPONSETAKER_H_

#include <request/pointers.h>
#include <response/pointers.h>

namespace Cynara {

class ResponseTaker {
public:
    ResponseTaker() = default;
    virtual ~ResponseTaker() {};

    virtual void execute(const RequestContext &context, const AdminCheckResponse &response);
    virtual void execute(const RequestContext &context, const AgentActionResponse &response);
    virtual void execute(const RequestContext &context, const AgentRegisterResponse &response);
    virtual void execute(const RequestContext &context, const CancelResponse &response);
    virtual void execute(const RequestContext &context, const CheckResponse &response);
    virtual void execute(const RequestContext &context, const CodeResponse &response);
    virtual void execute(const RequestContext &context, const DescriptionListResponse &response);
    virtual void execute(const RequestContext &context, const ListResponse &response);
    virtual void execute(const RequestContext &context, const MonitorGetEntriesResponse &response);
    virtual void execute(const RequestContext &context, const SimpleCheckResponse &response);
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_RESPONSETAKER_H_ */
