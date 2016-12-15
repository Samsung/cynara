/*
 *  Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/client-async/logic/Logic.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains declaration of Logic class - main
 *              libcynara-client-async class
 */

#ifndef SRC_CLIENT_ASYNC_LOGIC_LOGIC_H_
#define SRC_CLIENT_ASYNC_LOGIC_LOGIC_H_

#include <memory>

#include <cache/CacheInterface.h>
#include <cache/MonitorCache.h>
#include <configuration/Configuration.h>
#include <types/ProtocolFields.h>

#include <api/ApiInterface.h>
#include <cache/CapacityCache.h>
#include <callback/StatusCallback.h>
#include <check/CheckData.h>
#include <cynara-client-async.h>
#include <sequence/SequenceContainer.h>
#include <sockets/SocketClientAsync.h>

namespace Cynara {
class Logic;
typedef std::unique_ptr<Logic> LogicUniquePtr;

class Logic : public ApiInterface {
public:
    Logic(cynara_status_callback callback, void *userStatusData,
          const Configuration &conf = Configuration());
    virtual ~Logic();

    virtual int checkCache(const std::string &client, const std::string &session,
                           const std::string &user, const std::string &privilege);
    virtual int createCheckRequest(const std::string &client, const std::string &session,
                                   const std::string &user, const std::string &privilege,
                                   cynara_check_id &checkId, cynara_response_callback callback,
                                   void *userResponseData);
    virtual int createSimpleRequest(const std::string &client, const std::string &session,
                                    const std::string &user, const std::string &privilege,
                                    cynara_check_id &checkId, cynara_response_callback callback,
                                    void *userResponseData);
    virtual int process(void);
    virtual int cancelRequest(cynara_check_id checkId);
    virtual bool isFinishPermitted(void);
    virtual void tryFlushMonitor(void);

private:
    typedef std::map<ProtocolFrameSequenceNumber, CheckData> CheckMap;
    typedef std::pair<ProtocolFrameSequenceNumber, CheckData> CheckPair;

    StatusCallback m_statusCallback;
    CapacityCache m_cache;
    SocketClientAsync m_socketClient;
    CheckMap m_checks;
    SequenceContainer m_sequenceContainer;
    bool m_operationPermitted;
    bool m_inAnswerCancelResponseCallback;
    MonitorCache m_monitorCache;
    const bool m_monitoringEnabled;

    bool checkCacheValid(void);
    int createRequest(bool simple, const std::string &client, const std::string &session,
                      const std::string &user, const std::string &privilege,
                      cynara_check_id &checkId, cynara_response_callback callback,
                      void *userResponseData);
    void prepareRequestsToSend(void);
    cynara_async_status socketDataStatus(void);
    bool processOut(void);
    CheckMap::iterator checkResponseValid(const Response &response);
    void releaseRequest(CheckMap::iterator reqIt);
    void processCheckResponse(const CheckResponse &checkResponse);
    void processCancelResponse(const CancelResponse &cancelResponse);
    void processSimpleCheckResponse(const SimpleCheckResponse &response);
    void processResponses(void);
    bool processIn(void);
    bool ensureConnection(void);
    bool connect(void);
    int completeConnection(bool &completed);
    void onStatusChange(int sock, cynara_async_status status);
    void onServiceNotAvailable(void);
    void onDisconnected(void);

    void updateMonitor(const PolicyKey &policyKey, int result);
    void flushMonitor();
};

} // namespace Cynara

#endif /* SRC_CLIENT_ASYNC_LOGIC_LOGIC_H_ */
