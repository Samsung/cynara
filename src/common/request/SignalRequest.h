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
 * @file        SignalRequest.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines signal request class
 */

#ifndef SRC_COMMON_REQUEST_SIGNALREQUEST_H_
#define SRC_COMMON_REQUEST_SIGNALREQUEST_H_

#include <sys/signalfd.h>

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class SignalRequest : public Request {
private:
    struct signalfd_siginfo m_sigInfo;

public:
    SignalRequest(struct signalfd_siginfo sigInfo) : Request(0), m_sigInfo(sigInfo) {
    }

    virtual ~SignalRequest() {};

    virtual void execute(RequestPtr self, RequestTakerPtr taker, RequestContextPtr context) const;

    uint32_t signalNumber(void) {
        return m_sigInfo.ssi_signo;
    }

    struct signalfd_siginfo &sigInfo(void) {
        return m_sigInfo;
    }
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_SIGNALREQUEST_H_ */
