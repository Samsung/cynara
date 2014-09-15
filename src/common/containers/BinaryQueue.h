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
/**
 * @file        BinaryQueue.h
 * @author      Przemyslaw Dobrowolski <p.dobrowolsk@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file is the header file of binary queue
 */

#ifndef SRC_COMMON_CONTAINERS_BINARYQUEUE_H_
#define SRC_COMMON_CONTAINERS_BINARYQUEUE_H_

#include <memory>
#include <list>

namespace Cynara {
/**
 * Binary queue auto pointer
 */
class BinaryQueue;
typedef std::shared_ptr<BinaryQueue> BinaryQueuePtr;

/**
 * Binary stream implemented as constant size bucket list
 *
 * @todo Add optimized implementation for FlattenConsume
 */
class BinaryQueue
{
  public:
    typedef void (*BufferDeleter)(const void *buffer, size_t bufferSize,
                                  void *userParam);
    static void bufferDeleterFree(const void *buffer,
                                  size_t bufferSize,
                                  void *userParam);

  private:
    struct Bucket
    {
        const void *buffer;
        const void *ptr;
        size_t size;
        size_t left;

        BufferDeleter deleter;
        void *param;

        Bucket(const void *buffer,
               size_t bufferSize,
               BufferDeleter deleter,
               void *userParam);
        ~Bucket();
        // make it noncopyable
        Bucket(const Bucket &) = delete;
        const Bucket &operator=(const Bucket &) = delete;
        // make it nonmoveable
        Bucket(Bucket &&) = delete;
        Bucket &operator=(Bucket &&) = delete;
    };

    typedef std::list<Bucket *> BucketList;
    BucketList m_buckets;
    size_t m_size;

    static void deleteBucket(Bucket *bucket);

  public:
    /**
     * Construct empty binary queue
     */
    BinaryQueue();

    /**
     * Construct binary queue via bare copy of other binary queue
     *
     * @param[in] other Other binary queue to copy from
     * @warning One cannot assume that bucket structure is preserved during copy
     */
    BinaryQueue(const BinaryQueue &other);

    /**
     * Destructor
     */
    ~BinaryQueue();

    /**
     * Construct binary queue via bare copy of other binary queue
     *
     * @param[in] other Other binary queue to copy from
     * @warning One cannot assume that bucket structure is preserved during copy
     */
    const BinaryQueue &operator=(const BinaryQueue &other);

    /**
     * Append copy of @a bufferSize bytes from memory pointed by @a buffer
     * to the end of binary queue. Uses default deleter based on free.
     *
     * @return none
     * @param[in] buffer Pointer to buffer to copy data from
     * @param[in] bufferSize Number of bytes to copy
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     * @see BinaryQueue::BufferDeleterFree
     */
    void appendCopy(const void *buffer, size_t bufferSize);

    /**
     * Append @a bufferSize bytes from memory pointed by @a buffer
     * to the end of binary queue. Uses custom provided deleter.
     * Responsibility for deleting provided buffer is transfered to BinaryQueue.
     *
     * @return none
     * @param[in] buffer Pointer to data buffer
     * @param[in] bufferSize Number of bytes available in buffer
     * @param[in] deleter Pointer to deleter procedure used to free provided
     * buffer
     * @param[in] userParam User parameter passed to deleter routine
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     * @exception Cynara::NullPointerException buffer or deleter are nullptr
     */
    void appendUnmanaged(
        const void *buffer,
        size_t bufferSize,
        BufferDeleter deleter =
            &BinaryQueue::bufferDeleterFree,
        void *userParam = nullptr);

    /**
     * Append copy of other binary queue to the end of this binary queue
     *
     * @return none
     * @param[in] other Constant reference to other binary queue to copy data
     * from
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     * @warning One cannot assume that bucket structure is preserved during copy
     */
    void appendCopyFrom(const BinaryQueue &other);

    /**
     * Move bytes from other binary queue to the end of this binary queue.
     * This also removes all bytes from other binary queue.
     * This method is designed to be as fast as possible (only pointer swaps)
     * and is suggested over making copies of binary queues.
     * Bucket structure is preserved after operation.
     *
     * @return none
     * @param[in] other Reference to other binary queue to move data from
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     */
    void appendMoveFrom(BinaryQueue &other);

    /**
     * Append copy of binary queue to the end of other binary queue
     *
     * @return none
     * @param[in] other Constant reference to other binary queue to copy data to
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     * @warning One cannot assume that bucket structure is preserved during copy
     */
    void appendCopyTo(BinaryQueue &other) const;

    /**
     * Move bytes from binary queue to the end of other binary queue.
     * This also removes all bytes from binary queue.
     * This method is designed to be as fast as possible (only pointer swaps)
     * and is suggested over making copies of binary queues.
     * Bucket structure is preserved after operation.
     *
     * @return none
     * @param[in] other Reference to other binary queue to move data to
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     */
    void appendMoveTo(BinaryQueue &other);

    /**
     * Retrieve total size of all data contained in binary queue
     *
     * @return Number of bytes in binary queue
     */
    size_t size() const;

    /**
     * Remove all data from binary queue
     *
     * @return none
     */
    void clear();

    /**
     * Check if binary queue is empty
     *
     * @return true if binary queue is empty, false otherwise
     */
    bool empty() const;

    /**
     * Remove @a size bytes from beginning of binary queue
     *
     * @return none
     * @param[in] size Number of bytes to remove
     * @exception Cynara::OutOfDataException Number of bytes is larger
     *            than available bytes in binary queue
     */
    void consume(size_t size);

    /**
     * Retrieve @a bufferSize bytes from beginning of binary queue and copy them
     * to user supplied buffer
     *
     * @return none
     * @param[in] buffer Pointer to user buffer to receive bytes
     * @param[in] bufferSize Size of user buffer pointed by @a buffer
     * @exception Cynara::OutOfDataException Number of bytes to flatten
     *            is larger than available bytes in binary queue
     */
    void flatten(void *buffer, size_t bufferSize) const;

    /**
     * Retrieve @a bufferSize bytes from beginning of binary queue, copy them
     * to user supplied buffer, and remove from binary queue
     *
     * @return none
     * @param[in] buffer Pointer to user buffer to receive bytes
     * @param[in] bufferSize Size of user buffer pointed by @a buffer
     * @exception Cynara::OutOfDataException Number of bytes to flatten
     *            is larger than available bytes in binary queue
     */
    void flattenConsume(void *buffer, size_t bufferSize);
};

} // namespace Cynara

#endif /* SRC_COMMON_CONTAINERS_BINARYQUEUE_H_ */
