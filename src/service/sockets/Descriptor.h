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
/**
 * @file        src/service/sockets/Descriptor.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines descriptor class
 */

#ifndef SRC_SERVICE_SOCKETS_DESCRIPTOR_H_
#define SRC_SERVICE_SOCKETS_DESCRIPTOR_H_

#include <memory>

#include <common.h>

#include <protocol/Protocol.h>
#include <request/Request.h>
#include <response/pointers.h>

namespace Cynara {

class Descriptor {
public:
    Descriptor();

    bool isListen(void) const {
        return m_listen;
    }

    bool isUsed(void) const {
        return m_used;
    }

    bool isClient(void) const {
        return m_client;
    }

    bool hasDataToWrite(void) const;

    const ProtocolPtr protocol(void) const {
        return m_protocol;
    }

    ResponseTakerPtr responseTaker(void) const;

    BinaryQueuePtr writeQueue(void);

    void setProtocol(ProtocolPtr protocol) {
        m_protocol = protocol;
    }

    void setListen(bool listen) {
        m_listen = listen;
    }

    void setUsed(bool used) {
        m_used = used;
    }

    void setClient(bool client) {
        m_client = client;
    }

    void pushReadBuffer(const RawBuffer &readbuffer);
    RequestPtr extractRequest(void);

    RawBuffer &prepareWriteBuffer(void);

    void clear(void);

private:
    bool m_listen;
    bool m_used;
    bool m_client;

    BinaryQueuePtr m_readQueue;
    BinaryQueuePtr m_writeQueue;
    RawBuffer m_writeBuffer;

    ProtocolPtr m_protocol;

    void checkQueues(void);
};

} // namespace Cynara

#endif /* SRC_SERVICE_SOCKETS_DESCRIPTOR_H_ */
