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
 * @file        test/common/protocols/ResponseTestHelper.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Helper functions for tests of response serialization by protocols
 */

#ifndef TEST_COMMON_PROTOCOLS_RESPONSETESTHELPER_H_
#define TEST_COMMON_PROTOCOLS_RESPONSETESTHELPER_H_

#include <memory>

#include <gtest/gtest.h>

#include <containers/BinaryQueue.h>
#include <containers/RawBuffer.h>
#include <protocol/Protocol.h>
#include <request/RequestContext.h>
#include <response/pointers.h>
#include <response/ResponseTaker.h>

#include "CommonsTestHelper.h"

namespace {
namespace ResponseTestHelper {

template <typename R>
void testResponse(std::shared_ptr<R> response, Cynara::ProtocolPtr protocol) {
    auto queue = std::make_shared<Cynara::BinaryQueue>();
    auto context = Cynara::RequestContext(Cynara::ResponseTakerPtr(), queue);

    response->execute(*protocol, context);

    auto extractedResponse = protocol->extractResponseFromBuffer(queue);
    ASSERT_TRUE(bool(extractedResponse));
    ASSERT_EQ(queue->size(), static_cast<size_t>(0));

    compare(*response, dynamic_cast<R &>(*extractedResponse));
}

void binaryTestResponse(Cynara::ResponsePtr response, Cynara::ProtocolPtr protocol) {
    auto queue = std::make_shared<Cynara::BinaryQueue>();
    auto context = Cynara::RequestContext(Cynara::ResponseTakerPtr(), queue);

    response->execute(*protocol, context);
    Cynara::RawBuffer data(queue->size());
    queue->flatten(data.data(), queue->size());

    auto extractedResponse = protocol->extractResponseFromBuffer(queue);
    ASSERT_TRUE(bool(extractedResponse));
    ASSERT_EQ(queue->size(), static_cast<size_t>(0));

    extractedResponse->execute(*protocol, context);
    Cynara::RawBuffer data2(queue->size());
    queue->flatten(data2.data(), queue->size());

    ASSERT_EQ(data, data2);
}

} /* namespace ResponseTestHelper */
} /* namespace anonymous */

#endif /* TEST_COMMON_PROTOCOLS_RESPONSETESTHELPER_H_ */
