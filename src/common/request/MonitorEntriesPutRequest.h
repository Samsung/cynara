/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/request/MonitorEntriesPutRequest.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines monitor entries put request class
 */

#ifndef SRC_COMMON_REQUEST_MONITORENTRIESPUTREQUEST_H_
#define SRC_COMMON_REQUEST_MONITORENTRIESPUTREQUEST_H_

#include <vector>

#include <types/MonitorEntry.h>

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class MonitorEntriesPutRequest : public Request {

public:
    MonitorEntriesPutRequest(const std::vector<MonitorEntry> &monitorEntries,
                             ProtocolFrameSequenceNumber sequenceNumber)
        : Request(sequenceNumber), m_monitorEntries(monitorEntries) {}

    virtual ~MonitorEntriesPutRequest() = default;

    virtual void execute(RequestTaker &taker, const RequestContext &context) const;

    const std::vector<MonitorEntry> &monitorEntries(void) const {
        return m_monitorEntries;
    }

private:
    std::vector<MonitorEntry> m_monitorEntries;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_MONITORENTRIESPUTREQUEST_H_ */
