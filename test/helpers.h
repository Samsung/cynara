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
 * @file        test/helpers.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Helper functions for tests
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <vector>

#include "types/PolicyKey.h"
#include "types/PolicyBucketId.h"

namespace Cynara {

namespace Helpers {

PolicyKey generatePolicyKey(const PolicyKeyFeature::ValueType &sufix = "");
PolicyBucketId generateBucketId(const PolicyBucketId &sufix = "");

template <class Collection>
Collection pickFromCollection(const Collection &original, const std::vector<unsigned> &idx) {
    Collection filtered;
    filtered.reserve(idx.size());
    for (const auto &i : idx) {
        filtered.push_back(original.at(i));
    }
    return filtered;
}

} // namespace Helpers

} // namespace Cynara

#endif // HELPERS_H
