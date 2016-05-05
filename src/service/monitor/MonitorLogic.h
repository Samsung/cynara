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
 * @file        src/service/monitor/MonitorLogic.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines logic class of monitor part of cynara service
 */

#ifndef SRC_SERVICE_MONITOR_MONITORLOGIC_H_
#define SRC_SERVICE_MONITOR_MONITORLOGIC_H_

#include <map>
#include <list>
#include <vector>

#include <request/RequestContext.h>
#include <types/ProtocolFields.h>

#include "EntriesManager.h"

namespace Cynara {

class MonitorLogic {
public:
    MonitorLogic() {}
    struct MonitorResponseInfo {
        MonitorResponseInfo() : seq(0) {}
        MonitorResponseInfo(RequestContext _context, ProtocolFrameSequenceNumber _seq)
            : context(_context), seq(_seq) {}
        RequestContext context;
        ProtocolFrameSequenceNumber seq;
    };

    struct MonitorResponse {
        MonitorResponse() {}
        MonitorResponse(RequestContext context, ProtocolFrameSequenceNumber seq,
                        std::vector<MonitorEntry> &&_entries) : info(context, seq),
                                entries(std::move(_entries)) {}
        MonitorResponse(MonitorResponseInfo _info, std::vector<MonitorEntry> &&_entries)
            : info(_info), entries(std::move(_entries)) {}
        MonitorResponseInfo info;
        std::vector<MonitorEntry> entries;
    };

    void addClient(const RequestContext &context, ProtocolFrameSequenceNumber seq, int bufferSize);
    void flushClient(const RequestContext &context);
    void addEntry(const MonitorEntry &e);
    void removeClient(const RequestContext &context);
    bool shouldSend(void);
    std::vector<MonitorResponse> getResponses(void);
private:
    EntriesManager m_manager;
    std::map<RequestContext::ClientId, MonitorResponse> m_responseCache;
    std::map<RequestContext::ClientId, MonitorResponseInfo> m_clients;
};

} /* namespace Cynara */

#endif /* SRC_SERVICE_MONITOR_MONITORLOGIC_H_ */
