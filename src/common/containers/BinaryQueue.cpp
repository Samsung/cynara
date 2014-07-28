/*
 * Copyright (c) 2011-2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        binary_queue.cpp
 * @author      Przemyslaw Dobrowolski <p.dobrowolsk@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file is the implementation file of binary queue
 */

#include <algorithm>
#include <cstring>
#include <malloc.h>
#include <new>
#include <stddef.h>

#include <attributes/attributes.h>
#include <exceptions/NullPointerException.h>
#include <exceptions/OutOfDataException.h>
#include "BinaryQueue.h"

namespace Cynara {
BinaryQueue::BinaryQueue() : m_size(0) {
}

BinaryQueue::BinaryQueue(const BinaryQueue &other) : m_size(0) {
    appendCopyFrom(other);
}

BinaryQueue::~BinaryQueue() {
    // Remove all remaining buckets
    clear();
}

const BinaryQueue &BinaryQueue::operator=(const BinaryQueue &other) {
    if (this != &other) {
        clear();
        appendCopyFrom(other);
    }

    return *this;
}

void BinaryQueue::appendCopyFrom(const BinaryQueue &other) {
    // To speed things up, always copy as one bucket
    void *bufferCopy = malloc(other.m_size);

    if (bufferCopy == nullptr) {
        throw std::bad_alloc();
    }

    try {
        other.flatten(bufferCopy, other.m_size);
        appendUnmanaged(bufferCopy, other.m_size, &bufferDeleterFree, nullptr);
    } catch (const std::bad_alloc &) {
        // Free allocated memory
        free(bufferCopy);
        throw;
    }
}

void BinaryQueue::appendMoveFrom(BinaryQueue &other) {
    // Copy all buckets
    std::copy(other.m_buckets.begin(),
              other.m_buckets.end(), std::back_inserter(m_buckets));
    m_size += other.m_size;

    // Clear other, but do not free memory
    other.m_buckets.clear();
    other.m_size = 0;
}

void BinaryQueue::appendCopyTo(BinaryQueue &other) const {
    other.appendCopyFrom(*this);
}

void BinaryQueue::appendMoveTo(BinaryQueue &other) {
    other.appendMoveFrom(*this);
}

void BinaryQueue::clear() {
    std::for_each(m_buckets.begin(), m_buckets.end(), &deleteBucket);
    m_buckets.clear();
    m_size = 0;
}

void BinaryQueue::appendCopy(const void* buffer, size_t bufferSize) {
    // Create data copy with malloc/free
    void *bufferCopy = malloc(bufferSize);

    // Check if allocation succeded
    if (bufferCopy == nullptr) {
        throw std::bad_alloc();
    }

    // Copy user data
    memcpy(bufferCopy, buffer, bufferSize);

    try {
        // Try to append new bucket
        appendUnmanaged(bufferCopy, bufferSize, &bufferDeleterFree, nullptr);
    } catch (const std::bad_alloc &) {
        // Free allocated memory
        free(bufferCopy);
        throw;
    }
}

void BinaryQueue::appendUnmanaged(const void* buffer,
                                  size_t bufferSize,
                                  BufferDeleter deleter,
                                  void* userParam) {
    // Do not attach empty buckets
    if (bufferSize == 0) {
        deleter(buffer, bufferSize, userParam);
        return;
    }

    // Just add new bucket with selected deleter
    Bucket *bucket = new Bucket(buffer, bufferSize, deleter, userParam);
    try {
        m_buckets.push_back(bucket);
    } catch (const std::bad_alloc &) {
        delete bucket;
        throw;
    }

    // Increase total queue size
    m_size += bufferSize;
}

size_t BinaryQueue::size() const {
    return m_size;
}

bool BinaryQueue::empty() const {
    return m_size == 0;
}

void BinaryQueue::consume(size_t size) {
    // Check parameters
    if (size > m_size) {
        throw OutOfDataException(m_size, size);
    }

    size_t bytesLeft = size;

    // Consume data and/or remove buckets
    while (bytesLeft > 0) {
        // Get consume size
        size_t count = std::min(bytesLeft, m_buckets.front()->left);

        m_buckets.front()->ptr =
            static_cast<const char *>(m_buckets.front()->ptr) + count;
        m_buckets.front()->left -= count;
        bytesLeft -= count;
        m_size -= count;

        if (m_buckets.front()->left == 0) {
            deleteBucket(m_buckets.front());
            m_buckets.pop_front();
        }
    }
}

void BinaryQueue::flatten(void *buffer, size_t bufferSize) const {
    // Check parameters
    if (bufferSize == 0) {
        return;
    }

    if (bufferSize > m_size) {
        throw OutOfDataException(m_size, bufferSize);
    }

    size_t bytesLeft = bufferSize;
    void *ptr = buffer;
    BucketList::const_iterator bucketIterator = m_buckets.begin();

    // Flatten data
    while (bytesLeft > 0) {
        // Get consume size
        // todo a check is needed if bucketIterator doesn't point to m_buckets.end()
        size_t count = std::min(bytesLeft, (*bucketIterator)->left);

        // Copy data to user pointer
        memcpy(ptr, (*bucketIterator)->ptr, count);

        // Update flattened bytes count
        bytesLeft -= count;
        ptr = static_cast<char *>(ptr) + count;

        // Take next bucket
        ++bucketIterator;
    }
}

void BinaryQueue::flattenConsume(void *buffer, size_t bufferSize) {
    // FIXME: Optimize
    flatten(buffer, bufferSize);
    consume(bufferSize);
}

void BinaryQueue::deleteBucket(BinaryQueue::Bucket *bucket) {
    delete bucket;
}

void BinaryQueue::bufferDeleterFree(const void* data,
                                    size_t dataSize UNUSED,
                                    void* userParam UNUSED) {
    // Default free deleter
    free(const_cast<void *>(data));
}

BinaryQueue::Bucket::Bucket(const void* data,
                            size_t dataSize,
                            BufferDeleter dataDeleter,
                            void* userParam) :
    buffer(data),
    ptr(data),
    size(dataSize),
    left(dataSize),
    deleter(dataDeleter),
    param(userParam) {

    if(data == nullptr)
        throw NullPointerException("data");

    if(dataDeleter == nullptr)
        throw NullPointerException("dataDeleter");
}

BinaryQueue::Bucket::~Bucket() {
    // Invoke deleter on bucket data
    deleter(buffer, size, param);
}

} // namespace Cynara
