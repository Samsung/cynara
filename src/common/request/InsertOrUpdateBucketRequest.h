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
 * @file        src/common/request/InsertOrUpdateBucketRequest.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines request class for inserting or updating policy bucket
 */

#ifndef SRC_COMMON_REQUEST_INSERTORUPDATEBUCKETREQUEST_H_
#define SRC_COMMON_REQUEST_INSERTORUPDATEBUCKETREQUEST_H_

#include <types/PolicyBucketId.h>
#include <types/PolicyResult.h>

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class InsertOrUpdateBucketRequest : public Request {
private:
    PolicyBucketId m_bucketId;
    PolicyResult m_result;

public:
    InsertOrUpdateBucketRequest(const PolicyBucketId &bucketId, const PolicyResult &result,
                                ProtocolFrameSequenceNumber sequenceNumber) :
        Request(sequenceNumber), m_bucketId(bucketId), m_result(result) {
    }

    virtual ~InsertOrUpdateBucketRequest() {};

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

    const PolicyResult &result(void) const {
        return m_result;
    }

    virtual void execute(RequestTaker &taker, const RequestContext &context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_INSERTORUPDATEBUCKETREQUEST_H_ */
