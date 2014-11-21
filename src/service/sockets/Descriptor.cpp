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
 * @file        src/service/sockets/Descriptor.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements descriptor class
 */

#include "Descriptor.h"

namespace Cynara {

Descriptor::Descriptor() : m_listen(false), m_used(false), m_client(false), m_protocol(nullptr) {
}

void Descriptor::checkQueues(void) {
    if (!m_writeQueue)
        m_writeQueue = std::make_shared<BinaryQueue>();
    if (!m_readQueue)
        m_readQueue = std::make_shared<BinaryQueue>();
}

BinaryQueuePtr Descriptor::writeQueue(void) {
    checkQueues();
    return m_writeQueue;
}

bool Descriptor::hasDataToWrite(void) const {
    if (m_writeQueue)
        return !(m_writeQueue->empty() && m_writeBuffer.empty());
    return false;
}

ResponseTakerPtr Descriptor::responseTaker(void) const {
    return std::static_pointer_cast<ResponseTaker>(m_protocol);
}

void Descriptor::pushReadBuffer(const RawBuffer &readbuffer) {
    checkQueues();
    m_readQueue->appendCopy(readbuffer.data(), readbuffer.size());
}

RequestPtr Descriptor::extractRequest(void) {
    checkQueues();
    return m_protocol->extractRequestFromBuffer(m_readQueue);
}

RawBuffer &Descriptor::prepareWriteBuffer(void) {
    checkQueues();
    size_t queuedDataSize = m_writeQueue->size();
    size_t bufferDataSize = m_writeBuffer.size();

    m_writeBuffer.resize(queuedDataSize + bufferDataSize);
    m_writeQueue->flattenConsume(m_writeBuffer.data() + bufferDataSize, queuedDataSize);

    return m_writeBuffer;
}

void Descriptor::clear(void) {
    m_listen = false;
    m_used = false;
    m_client = false;
    m_readQueue.reset();
    m_writeQueue.reset();
    m_writeBuffer.clear();
    m_protocol.reset();
}

} // namespace Cynara
