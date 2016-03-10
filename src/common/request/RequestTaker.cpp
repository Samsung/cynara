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
 * @file        src/common/request/RequestTaker.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements RequestTaker class
 */

#include <attributes/attributes.h>
#include <exceptions/NotImplementedException.h>

#include "RequestTaker.h"

namespace Cynara {

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const AdminCheckRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const AgentActionRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const AgentRegisterRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const CancelRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const CheckRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const EraseRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const DescriptionListRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const InsertOrUpdateBucketRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const ListRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const MonitorGetEntriesRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const MonitorGetFlushRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const RemoveBucketRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const SetPoliciesRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const SignalRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::execute(const RequestContext &context UNUSED,
                           const SimpleCheckRequest &request UNUSED) {
    throw NotImplementedException();
}

void RequestTaker::contextClosed(const RequestContext &context UNUSED) {
    throw NotImplementedException();
}

} // namespace Cynara
