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
 * @file        src/common/exceptions/BucketNotExistsException.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of BucketNotExistsException
 */

#ifndef SRC_COMMON_EXCEPTIONS_BUCKETNOTEXISTSEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_BUCKETNOTEXISTSEXCEPTION_H_

#include "Exception.h"
#include "types/PolicyBucketId.h"

#include <exception>

namespace Cynara {

class BucketNotExistsException : public Exception {
public:
    BucketNotExistsException() = delete;
    BucketNotExistsException(const PolicyBucketId &bucketId) : m_bucketId(bucketId) {}
    virtual ~BucketNotExistsException() {};

    virtual const std::string message(void) const {
        return "BucketNotExistsException";
    }

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

private:
    PolicyBucketId m_bucketId;
};

} /* namespace Cynara */


#endif /* SRC_COMMON_EXCEPTIONS_BUCKETNOTEXISTSEXCEPTION_H_ */
