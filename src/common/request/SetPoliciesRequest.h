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
 * @file        SetPoliciesRequest.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines request class for modifying policies
 */

#ifndef SRC_COMMON_REQUEST_SETPOLICIESREQUEST_H_
#define SRC_COMMON_REQUEST_SETPOLICIESREQUEST_H_

#include <map>
#include <vector>

#include <types/Policy.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>

#include <request/pointers.h>
#include <request/Request.h>
#include <request/RequestTaker.h>

namespace Cynara {

class SetPoliciesRequest : public Request {
private:
    std::map<PolicyBucketId, std::vector<Policy>> m_insertOrUpdatePolicies;
    std::map<PolicyBucketId, std::vector<PolicyKey>> m_removePolicies;

public:
    SetPoliciesRequest(const std::map<PolicyBucketId, std::vector<Policy>> &insertOrUpdatePolicies,
                       const std::map<PolicyBucketId, std::vector<PolicyKey>> &removePolicies,
                       ProtocolFrameSequenceNumber sequenceNumber) :
        Request(sequenceNumber), m_insertOrUpdatePolicies(insertOrUpdatePolicies),
        m_removePolicies(removePolicies) {
    }

    virtual ~SetPoliciesRequest() {};

    const std::map<PolicyBucketId, std::vector<Policy>> &policiesToBeInsertedOrUpdated(void) const {
        return m_insertOrUpdatePolicies;
    }

    const std::map<PolicyBucketId, std::vector<PolicyKey>> &policiesToBeRemoved(void) const {
        return m_removePolicies;
    }

    virtual void execute(RequestPtr self, RequestTakerPtr taker, RequestContextPtr context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_SETPOLICIESREQUEST_H_ */
