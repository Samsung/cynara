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
 * @file        src/storage/BucketDeserializer.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Headers for Cynara::BucketDeserializer
 */
#ifndef SRC_STORAGE_BUCKETDESERIALIZER_H_
#define SRC_STORAGE_BUCKETDESERIALIZER_H_

#include <fstream>
#include <memory>
#include <string>

#include <types/PolicyCollection.h>
#include <types/PolicyKey.h>

namespace Cynara {

class BucketDeserializer {

public:
    BucketDeserializer(std::shared_ptr<std::istream> inStream) : m_inStream(inStream) {
    }

    PolicyCollection loadPolicies(void);
    static PolicyKey parseKey(const std::string &line, std::size_t &beginToken);

private:
    std::shared_ptr<std::istream> m_inStream;
};

} /* namespace Cynara */

#endif /* SRC_STORAGE_BUCKETDESERIALIZER_H_ */
