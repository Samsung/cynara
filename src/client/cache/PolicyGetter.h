/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        PolicyGetter.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains Cynara PolicyResult getter implementation.
 */

#ifndef SRC_CLIENT_CACHE_POLICYGETTER_H_
#define SRC_CLIENT_CACHE_POLICYGETTER_H_

#include <log/log.h>

#include <cache/CacheInterface.h>
#include <sockets/SocketClient.h>


namespace Cynara {

class PolicyGetter : public ResultGetterInterface {
public:
    PolicyGetter(const SocketClientPtr &socketClient) : m_socketClient(socketClient) {}
    cynara_api_result requestResult(const PolicyKey &key, PolicyResult &result) noexcept;

private:
    ProtocolFrameSequenceNumber generateSequenceNumber(void) {
        static ProtocolFrameSequenceNumber sequenceNumber = 0;
        return ++sequenceNumber;
    }

    SocketClientPtr m_socketClient;
};

} //namespace Cynara

#endif // SRC_CLIENT_CACHE_POLICYGETTER_H_




