/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/common/protocols/RequestTestHelper.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Helper functions for tests of request serialization by protocols
 */

#ifndef TEST_COMMON_PROTOCOLS_REQUESTTESTHELPER_H_
#define TEST_COMMON_PROTOCOLS_REQUESTTESTHELPER_H_

#include <memory>

#include <gtest/gtest.h>

#include <containers/BinaryQueue.h>
#include <containers/RawBuffer.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <request/RequestContext.h>
#include <response/ResponseTaker.h>

#include "CommonsTestHelper.h"

namespace {
namespace RequestTestHelper {

template <typename R>
void testRequest(std::shared_ptr<R> request, Cynara::ProtocolPtr protocol) {
    auto queue = std::make_shared<Cynara::BinaryQueue>();
    Cynara::RequestContext context(Cynara::ResponseTakerPtr(), queue);

    request->execute(*request, *protocol, context);

    auto extractedRequest = protocol->extractRequestFromBuffer(queue);
    ASSERT_TRUE(bool(extractedRequest));
    ASSERT_EQ(queue->size(), 0);

    compare(*request, dynamic_cast<R &>(*extractedRequest));
}

void binaryTestRequest(Cynara::RequestPtr request, Cynara::ProtocolPtr protocol) {
    auto queue = std::make_shared<Cynara::BinaryQueue>();
    Cynara::RequestContext context(Cynara::ResponseTakerPtr(), queue);

    request->execute(*request, *protocol, context);
    Cynara::RawBuffer data(queue->size());
    queue->flatten(data.data(), queue->size());

    auto extractedRequest = protocol->extractRequestFromBuffer(queue);
    ASSERT_TRUE(bool(extractedRequest));
    ASSERT_EQ(queue->size(), 0);

    extractedRequest->execute(*extractedRequest, *protocol, context);
    Cynara::RawBuffer data2(queue->size());
    queue->flatten(data2.data(), queue->size());

    ASSERT_EQ(data, data2);
}

} /* namespace RequestTestHelper */
} /* namespace anonymous */

#endif /* TEST_COMMON_PROTOCOLS_REQUESTTESTHELPER_H_ */
