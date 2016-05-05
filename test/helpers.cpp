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
 * @file        test/helpers.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions for tests
 */


#include "helpers.h"
#include "types/MonitorEntry.h"
#include "types/PolicyKey.h"

namespace Cynara {
namespace Helpers {

PolicyKey generatePolicyKey(const PolicyKeyFeature::ValueType &sufix) {
    auto createPKF = [&sufix](const PolicyKeyFeature::ValueType &value) -> PolicyKeyFeature {
        return PolicyKeyFeature::create(value + sufix);
    };

    return PolicyKey(createPKF("c"), createPKF("u"), createPKF("p"));
}

PolicyBucketId generateBucketId(const PolicyBucketId &sufix) {
    return "bucket" + sufix;
}

} // namespace Helpers

bool operator ==(const MonitorEntry &me1, const MonitorEntry &me2) {
    return me1.key() == me2.key()
           && me1.result() == me2.result()
           && me1.timestamp() == me2.timestamp();
}

} // namespace Cynara

bool operator ==(const timespec &t1, const timespec &t2) {
    return t1.tv_sec == t2.tv_sec && t1.tv_nsec == t2.tv_nsec;
}
