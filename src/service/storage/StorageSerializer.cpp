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
/*
 * @file        StorageSerializer.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of Cynara::StorageSerializer methods
 */

#include "StorageSerializer.h"

#include "types/PolicyBucket.h"
#include "types/PolicyCollection.h"

#include <algorithm>

namespace Cynara {

char StorageSerializer::m_fieldSeparator = ';';
char StorageSerializer::m_recordSeparator = '\n';

StorageSerializer::StorageSerializer(std::ostream &os) : m_outStream(os) {}

void StorageSerializer::dump(const PolicyBucket& bucket) {
    const auto &policies = bucket.policyCollection();

    for (auto it = std::begin(policies); it != std::end(policies); ++it) {
        const auto &policy = *it;
        dump(policy);
    }
}

void StorageSerializer::dump(const PolicyKey &key) {
    outStream() << key.toString();
}

void StorageSerializer::dump(const PolicyType &policyType) {
    outStream() << policyType;
}

void StorageSerializer::dump(const PolicyResult::PolicyMetadata &metadata) {
    outStream() << metadata;
}

void StorageSerializer::dump(const PolicyCollection::value_type &policy) {
    const auto &key = policy->key();
    const auto &result = policy->result();

    dumpFields(key, result.policyType(), result.metadata());
}

} /* namespace Cynara */
