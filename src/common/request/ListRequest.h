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
 * @file        src/common/request/ListRequest.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines policies list request class
 */

#ifndef SRC_COMMON_REQUEST_LISTREQUEST_H_
#define SRC_COMMON_REQUEST_LISTREQUEST_H_

#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class ListRequest : public Request {
private:
    PolicyBucketId m_bucket;
    PolicyKey m_filter;

public:
    ListRequest(const PolicyBucketId &bucket, const PolicyKey &filter,
                ProtocolFrameSequenceNumber sequenceNumber) :
        Request(sequenceNumber), m_bucket(bucket), m_filter(filter) {
    }

    virtual ~ListRequest() {};

    const PolicyBucketId &bucket(void) const {
        return m_bucket;
    }

    const PolicyKey &filter(void) const {
        return m_filter;
    }

    virtual void execute(const Request &self, RequestTaker &taker,
                         const RequestContext &context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_LISTREQUEST_H_ */
