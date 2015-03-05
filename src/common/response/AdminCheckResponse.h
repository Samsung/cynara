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
 * @file        src/common/response/AdminCheckResponse.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file defines response class for admin check request
 */

#ifndef SRC_COMMON_RESPONSE_ADMINCHECKRESPONSE_H_
#define SRC_COMMON_RESPONSE_ADMINCHECKRESPONSE_H_

#include <types/PolicyResult.h>

#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>

namespace Cynara {

class AdminCheckResponse : public Response {
public:
    AdminCheckResponse(const PolicyResult &result, bool bucketValid, bool dbCorrupted,
                       ProtocolFrameSequenceNumber sequenceNumber) :
        Response(sequenceNumber), m_result(result), m_bucketValid(bucketValid),
        m_dbCorrupted(dbCorrupted) {
    }

    virtual ~AdminCheckResponse() {}

    virtual void execute(const Response &self, ResponseTaker &taker,
                         const RequestContext &context) const;

    const PolicyResult &result(void) const {
        return m_result;
    }

    bool isBucketValid(void) const {
        return m_bucketValid;
    }

    bool isDbCorrupted(void) const {
        return m_dbCorrupted;
    }

private:
    const PolicyResult m_result;
    bool m_bucketValid;
    bool m_dbCorrupted;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_ADMINCHECKRESPONSE_H_ */
