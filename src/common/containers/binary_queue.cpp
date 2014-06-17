/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of binary queue
 */
#include <stddef.h>
#include <dpl/binary_queue.h>
#include <dpl/assert.h>
#include <algorithm>
#include <malloc.h>
#include <cstring>
#include <new>

namespace SecurityServer {
BinaryQueue::BinaryQueue() :
    m_size(0)
{}

BinaryQueue::BinaryQueue(const BinaryQueue &other) :
    m_size(0)
{
    AppendCopyFrom(other);
}

BinaryQueue::~BinaryQueue()
{
    // Remove all remainig buckets
    Clear();
}

const BinaryQueue &BinaryQueue::operator=(const BinaryQueue &other)
{
    if (this != &other) {
        Clear();
        AppendCopyFrom(other);
    }

    return *this;
}

void BinaryQueue::AppendCopyFrom(const BinaryQueue &other)
{
    // To speed things up, always copy as one bucket
    void *bufferCopy = malloc(other.m_size);

    if (bufferCopy == NULL) {
        throw std::bad_alloc();
    }

    try {
        other.Flatten(bufferCopy, other.m_size);
        AppendUnmanaged(bufferCopy, other.m_size, &BufferDeleterFree, NULL);
    } catch (const std::bad_alloc &) {
        // Free allocated memory
        free(bufferCopy);
        throw;
    }
}

void BinaryQueue::AppendMoveFrom(BinaryQueue &other)
{
    // Copy all buckets
    std::copy(other.m_buckets.begin(),
              other.m_buckets.end(), std::back_inserter(m_buckets));
    m_size += other.m_size;

    // Clear other, but do not free memory
    other.m_buckets.clear();
    other.m_size = 0;
}

void BinaryQueue::AppendCopyTo(BinaryQueue &other) const
{
    other.AppendCopyFrom(*this);
}

void BinaryQueue::AppendMoveTo(BinaryQueue &other)
{
    other.AppendMoveFrom(*this);
}

void BinaryQueue::Clear()
{
    std::for_each(m_buckets.begin(), m_buckets.end(), &DeleteBucket);
    m_buckets.clear();
    m_size = 0;
}

void BinaryQueue::AppendCopy(const void* buffer, size_t bufferSize)
{
    // Create data copy with malloc/free
    void *bufferCopy = malloc(bufferSize);

    // Check if allocation succeded
    if (bufferCopy == NULL) {
        throw std::bad_alloc();
    }

    // Copy user data
    memcpy(bufferCopy, buffer, bufferSize);

    try {
        // Try to append new bucket
        AppendUnmanaged(bufferCopy, bufferSize, &BufferDeleterFree, NULL);
    } catch (const std::bad_alloc &) {
        // Free allocated memory
        free(bufferCopy);
        throw;
    }
}

void BinaryQueue::AppendUnmanaged(const void* buffer,
                                  size_t bufferSize,
                                  BufferDeleter deleter,
                                  void* userParam)
{
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

size_t BinaryQueue::Size() const
{
    return m_size;
}

bool BinaryQueue::Empty() const
{
    return m_size == 0;
}

void BinaryQueue::Consume(size_t size)
{
    // Check parameters
    if (size > m_size) {
        Throw(Exception::OutOfData);
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
            DeleteBucket(m_buckets.front());
            m_buckets.pop_front();
        }
    }
}

void BinaryQueue::Flatten(void *buffer, size_t bufferSize) const
{
    // Check parameters
    if (bufferSize == 0) {
        return;
    }

    if (bufferSize > m_size) {
        Throw(Exception::OutOfData);
    }

    size_t bytesLeft = bufferSize;
    void *ptr = buffer;
    BucketList::const_iterator bucketIterator = m_buckets.begin();
    Assert(m_buckets.end() != bucketIterator);

    // Flatten data
    while (bytesLeft > 0) {
        // Get consume size
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

void BinaryQueue::FlattenConsume(void *buffer, size_t bufferSize)
{
    // FIXME: Optimize
    Flatten(buffer, bufferSize);
    Consume(bufferSize);
}

void BinaryQueue::DeleteBucket(BinaryQueue::Bucket *bucket)
{
    delete bucket;
}

void BinaryQueue::BufferDeleterFree(const void* data,
                                    size_t dataSize,
                                    void* userParam)
{
    (void)dataSize;
    (void)userParam;

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
    param(userParam)
{
    Assert(data != NULL);
    Assert(deleter != NULL);
}

BinaryQueue::Bucket::~Bucket()
{
    // Invoke deleter on bucket data
    deleter(buffer, size, param);
}

BinaryQueue::BucketVisitor::~BucketVisitor()
{}

BinaryQueue::BucketVisitorCall::BucketVisitorCall(BucketVisitor *visitor) :
    m_visitor(visitor)
{}

BinaryQueue::BucketVisitorCall::~BucketVisitorCall()
{}

void BinaryQueue::BucketVisitorCall::operator()(Bucket *bucket) const
{
    m_visitor->OnVisitBucket(bucket->ptr, bucket->left);
}

void BinaryQueue::VisitBuckets(BucketVisitor *visitor) const
{
    Assert(visitor != NULL);

    // Visit all buckets
    std::for_each(m_buckets.begin(), m_buckets.end(), BucketVisitorCall(visitor));
}

BinaryQueueAutoPtr BinaryQueue::Read(size_t size)
{
    // Simulate input stream
    size_t available = std::min(size, m_size);

    std::unique_ptr<void, std::function<void(void*)>>
        bufferCopy(malloc(available), free);

    if (!bufferCopy.get()) {
        throw std::bad_alloc();
    }

    BinaryQueueAutoPtr result(new BinaryQueue());

    Flatten(bufferCopy.get(), available);
    result->AppendUnmanaged(
        bufferCopy.release(), available, &BufferDeleterFree, NULL);
    Consume(available);

    return result;
}

size_t BinaryQueue::Write(const BinaryQueue &buffer, size_t bufferSize)
{
    // Simulate output stream
    AppendCopyFrom(buffer);
    return bufferSize;
}
} // namespace SecurityServer
