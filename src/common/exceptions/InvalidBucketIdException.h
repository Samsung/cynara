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
 * @file        src/common/exceptions/InvalidBucketIdException.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     0.1
 * @brief       This file defines exception thrown when bucket ID contains forbidden characters
 */

#ifndef SRC_COMMON_EXCEPTIONS_INVALIDBUCKETIDEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_INVALIDBUCKETIDEXCEPTION_H_

#include <string>

#include <exceptions/Exception.h>

namespace Cynara {

class InvalidBucketIdException : public Exception {
public:
    InvalidBucketIdException(const std::string &bucket) : m_bucketId(bucket) {
        m_message = "Bucket ID " + bucketId() + " contains forbidden characters";
    };
    virtual ~InvalidBucketIdException() {};

    const std::string &message(void) const {
        return m_message;
    }

    const std::string &bucketId(void) const {
        return m_bucketId;
    }

private:
    std::string m_message;
    std::string m_bucketId;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_INVALIDBUCKETIDEXCEPTION_H_ */
