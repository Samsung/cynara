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
 * @file        src/common/response/MonitorGetEntriesResponse.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines monitor get entries response
 */

#ifndef SRC_COMMON_RESPONSE_MONITORGETENTRIESRESPONSE_H_
#define SRC_COMMON_RESPONSE_MONITORGETENTRIESRESPONSE_H_

#include <vector>
#include <types/MonitorEntry.h>

#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>

namespace Cynara {

class MonitorGetEntriesResponse : public Response {
public:
    MonitorGetEntriesResponse(const std::vector<MonitorEntry> &entries,
                              ProtocolFrameSequenceNumber sequenceNumber)
        : Response(sequenceNumber), m_entries(entries)
    {}

    const std::vector<MonitorEntry> &entries(void) const {
        return m_entries;
    }

    virtual void execute(ResponseTaker &taker, const RequestContext &context) const;

private:
    const std::vector<MonitorEntry> m_entries;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_MONITORGETENTRIESRESPONSE_H_ */
