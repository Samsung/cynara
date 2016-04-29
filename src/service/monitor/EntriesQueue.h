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
 * @file        src/service/monitor/EntriesQueue.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines class for storing monitor entries
 */

#ifndef SRC_SERVICE_MONITOR_ENTRIESQUEUE_H_
#define SRC_SERVICE_MONITOR_ENTRIESQUEUE_H_

#include <vector>

#include <cynara-limits.h>
#include <types/MonitorEntry.h>

namespace {

const int MAX_QUEUE_SIZE = 2 * CYNARA_MAX_MONITOR_BUFFER_SIZE;
const int MAX_BUCKET_SIZE = 2 * 255;

}

namespace Cynara {

class EntriesQueue {
public:
    typedef int EntryId;

    EntriesQueue(int maxQueueSize = MAX_QUEUE_SIZE, int maxBucketSize = MAX_BUCKET_SIZE);

    /*
     * Add entry to queue, entries are stored in order they arrive. If capacity is reached,
     * least recently pushed entry is removed. Returns true, when overflow occured.
     */
    bool push(const MonitorEntry &entry);

    /*
     * Fetch given amount including given entryId, if not enough entries stored, returns empty
     * vector.
     */
    EntryId fetch(EntryId fromEntryId, int amount, std::vector<MonitorEntry> &entries) const;

    /*
     * Remove entries from queue pushed before given entryId excluding given entryId.
     */
    void popUntil(EntryId entryId);

    int size(void) const {
        return m_size;
    }

    int capacity(void) const {
        return m_maxQueueSize;
    }

    void clear(void);

    /*
     * Return number of elements stored counted from given fromEntryId inclusively
     */
    int sizeFrom(EntryId fromEntryId) const;

    bool empty(void) const {
        return m_size == 0;
    }

    /*
     * Returns first valid stored entryId. Mind that first entryId can be bigger than last entryId.
     */
    int getFrontEntryId(void) const {
        return m_firstBucketId * m_maxBucketSize + m_entries[m_firstBucketId].offset;
    }

    /*
     * Returns entryId after last valid entry.
     * Mind that last entryId can be smaller than first entryId.
     */
    int getBackEntryId(void) const {
        return (m_lastBucketId * m_maxBucketSize + m_entries[m_lastBucketId].entries.size());
    }

private:

    /*
     * Structure for keeping buffer on monitor entries. Buffer never gets smaller, so offset
     * indicates if any entries were fetched from the beginning.
     */
    struct EntriesBucket {
        EntriesBucket() : offset(0) {}
        std::vector<MonitorEntry> entries;
        int offset;
    };

    void createBucket(int bucketId);
    void eraseBucket(int bucketId);
    int countBucketId(EntryId entryId) const;
    int countBucketOffset(EntryId entryId) const;
    int nextBucketId(int bucketId) const;
    void copyEntries(int bucket, int offset, int amount, std::vector<MonitorEntry> &entries) const;
    void removeEntries(int endBucket, int endOffset);
    bool checkSize(void);

    std::vector<EntriesBucket> m_entries;
    int m_firstBucketId;
    int m_lastBucketId;
    int m_size;

    int m_maxQueueSize;
    int m_maxBucketSize;
};

} /* namespace Cynara */

#endif /* SRC_SERVICE_MONITOR_ENTRIESQUEUE_H_ */
