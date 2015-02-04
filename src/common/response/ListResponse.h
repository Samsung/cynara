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
 * @file        src/common/response/ListResponse.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file defines response class for policies list request
 */

#ifndef SRC_COMMON_RESPONSE_LISTRESPONSE_H_
#define SRC_COMMON_RESPONSE_LISTRESPONSE_H_

#include <vector>

#include <types/Policy.h>

#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>

namespace Cynara {

class ListResponse : public Response {
public:
    ListResponse(const std::vector<Policy> &policies, bool bucketValid, bool dbCorrupted,
                 ProtocolFrameSequenceNumber sequenceNumber) :
        Response(sequenceNumber), m_policies(policies), m_bucketValid(bucketValid),
        m_dbCorrupted(dbCorrupted) {
    }

    virtual ~ListResponse() {};

    virtual void execute(ResponsePtr self, ResponseTakerPtr taker, RequestContextPtr context) const;

    bool isBucketValid(void) const {
        return m_bucketValid;
    }

    bool isDbCorrupted(void) const {
        return m_dbCorrupted;
    }

    const std::vector<Policy> &policies(void) const {
        return m_policies;
    }

private:
    std::vector<Policy> m_policies;
    bool m_bucketValid;
    bool m_dbCorrupted;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_LISTRESPONSE_H_ */
