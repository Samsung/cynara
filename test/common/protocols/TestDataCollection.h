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
 * @file        test/common/protocols/TestDataCollection.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Collection of sample data for protocol tests
 */

#ifndef TEST_COMMON_PROTOCOLS_TESTDATACOLLECTION_H_
#define TEST_COMMON_PROTOCOLS_TESTDATACOLLECTION_H_

#include <cstdint>

#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>
#include <types/ProtocolFields.h>

namespace {
namespace TestDataCollection {

typedef Cynara::PolicyKeyFeature PKF;

namespace Keys {
    static const Cynara::PolicyKey k_nun(PKF::create(""), PKF::create("u"), PKF::create(""));
    static const Cynara::PolicyKey k_cup(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    static const Cynara::PolicyKey k_cup2(PKF::create("amanda"), PKF::create("to"),
                                          PKF::create("troll"));
    static const Cynara::PolicyKey k_wup(PKF::createWildcard(), PKF::create("u"), PKF::create("p"));
    static const Cynara::PolicyKey k_cwp(PKF::create("c"), PKF::createWildcard(), PKF::create("p"));
    static const Cynara::PolicyKey k_cuw(PKF::create("c"), PKF::create("u"), PKF::createWildcard());
    static const Cynara::PolicyKey k_cww(PKF::create("c"), PKF::createWildcard(),
                                         PKF::createWildcard());
    static const Cynara::PolicyKey k_wuw(PKF::createWildcard(), PKF::create("u"),
                                         PKF::createWildcard());
    static const Cynara::PolicyKey k_wwp(PKF::createWildcard(), PKF::createWildcard(),
                                         PKF::create("p"));
    static const Cynara::PolicyKey k_www(PKF::createWildcard(), PKF::createWildcard(),
                                         PKF::createWildcard());
    static const Cynara::PolicyKey k_aaa(PKF::createAny(), PKF::createAny(), PKF::createAny());
    static const Cynara::PolicyKey k_wua(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    static const Cynara::PolicyKey k_nua(PKF::create(""), PKF::create("u"), PKF::createAny());
    static const Cynara::PolicyKey k_wwa(PKF::createWildcard(), PKF::createWildcard(),
                                         PKF::createAny());
    static const Cynara::PolicyKey k_aup(PKF::createAny(), PKF::create("u"), PKF::create("p"));
} /* namespace Keys */

namespace SN {
    static const Cynara::ProtocolFrameSequenceNumber min(0);
    static const Cynara::ProtocolFrameSequenceNumber min_1(min + 1);
    static const Cynara::ProtocolFrameSequenceNumber min_2(min + 2);
    static const Cynara::ProtocolFrameSequenceNumber max(UINT16_MAX);
    static const Cynara::ProtocolFrameSequenceNumber max_1(max - 1);
    static const Cynara::ProtocolFrameSequenceNumber max_2(max - 2);
    static const Cynara::ProtocolFrameSequenceNumber mid((min + max) / 2);
} /* namespace SN */

namespace Buckets {
    static const Cynara::PolicyBucketId empty("");
    static const Cynara::PolicyBucketId not_empty("testBucket_1");
} /* namespace Buckets */

namespace Types {
    static const Cynara::PolicyType allow(Cynara::PredefinedPolicyType::ALLOW);
    static const Cynara::PolicyType deny(Cynara::PredefinedPolicyType::DENY);
    static const Cynara::PolicyType bucket(Cynara::PredefinedPolicyType::BUCKET);
    static const Cynara::PolicyType none(Cynara::PredefinedPolicyType::NONE);
    static const Cynara::PolicyType plugin_type(100);
} /* namespace Types */

namespace Results {
    static const Cynara::PolicyResult allow(Types::allow, "");
    static const Cynara::PolicyResult deny(Types::deny, "");
    static const Cynara::PolicyResult bucket_empty(Types::bucket, Buckets::empty);
    static const Cynara::PolicyResult bucket_not_empty(Types::bucket, Buckets::not_empty);
    static const Cynara::PolicyResult none(Types::none, "");
    static const Cynara::PolicyResult plugin_1(Types::plugin_type, "");
    static const Cynara::PolicyResult plugin_2(Types::plugin_type, "meta_data_2");
} /* namespace Results */
} /* namespace TestDataCollection */
} /* namespace anonymous */

#endif /* TEST_COMMON_PROTOCOLS_TESTDATACOLLECTION_H_ */
