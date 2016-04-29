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
 * @file        src/service/monitor/EntriesQueue.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements class for storing monitor entries
 */

#include <algorithm>

#include "EntriesQueue.h"

#include <log/log.h>
namespace Cynara {

EntriesQueue::EntriesQueue(int maxQueueSize, int maxBucketSize)
    : m_firstBucketId(0), m_lastBucketId(0), m_size(0), m_maxQueueSize(maxQueueSize),
      m_maxBucketSize(maxBucketSize)
{
    clear();
}

bool EntriesQueue::push(const MonitorEntry &entry) {
    auto &lastBucket = m_entries[m_lastBucketId];
    lastBucket.entries.push_back(entry);
    if (static_cast<int>(lastBucket.entries.size()) == m_maxBucketSize) {
        m_lastBucketId = nextBucketId(m_lastBucketId);
        createBucket(m_lastBucketId);
    }
    m_size++;
    return checkSize();
}

bool EntriesQueue::checkSize(void) {
    if (m_size <= m_maxQueueSize) {
        return false;
    }
    LOGW("Maximum capacity reached. Removing least recently pushed entry.");
    auto &firstBucket = m_entries[m_firstBucketId];
    firstBucket.offset++;
    m_size--;
    if (firstBucket.offset >= m_maxBucketSize) {
        eraseBucket(m_firstBucketId);
        m_firstBucketId = nextBucketId(m_firstBucketId);
    }
    return true;
}
EntriesQueue::EntryId EntriesQueue::fetch(EntriesQueue::EntryId fromEntryId, int amount,
                                          std::vector<MonitorEntry> &entries) const {

    if (sizeFrom(fromEntryId) < amount) {
        LOGE("Not enough entries stored to fetch [" << amount << "]");
        return -1;
    }
    int startBucketId = countBucketId(fromEntryId);
    int startBucketOffset = countBucketOffset(fromEntryId);
    copyEntries(startBucketId, startBucketOffset, amount, entries);
    return (fromEntryId + amount) % (m_entries.size() * m_maxBucketSize);
}

void EntriesQueue::popUntil(EntryId entryId) {
    int endBucketId = countBucketId(entryId);
    int endBucketOffset = countBucketOffset(entryId);
    removeEntries(endBucketId, endBucketOffset);
}

void EntriesQueue::clear(void) {
    m_lastBucketId = m_firstBucketId = 0;
    m_entries.clear();
    m_entries.resize(m_maxQueueSize/m_maxBucketSize + 1);
    m_size = 0;
}
int EntriesQueue::sizeFrom(EntryId fromEntryId) const {
    int frontId = getFrontEntryId();
    int backId = getBackEntryId();

    if (fromEntryId < frontId) {
        return backId - fromEntryId;
    } else {
        return m_size - (fromEntryId - frontId);
    }
}

void EntriesQueue::createBucket(int bucketId) {
    m_entries[bucketId].offset = 0;
    m_entries[bucketId].entries.reserve(m_maxBucketSize);
}
void EntriesQueue::eraseBucket(int bucketId) {
    // Trick for forcing vector to free its memory
    m_size -= m_entries[bucketId].entries.size() - m_entries[bucketId].offset;
    std::vector<MonitorEntry>().swap(m_entries[bucketId].entries);
    m_entries[bucketId].offset = 0;
}

int EntriesQueue::countBucketId(EntryId entryId) const {
    return entryId / m_maxBucketSize;
}

int EntriesQueue::countBucketOffset(EntryId entryId) const {
    return entryId % m_maxBucketSize;
}

int EntriesQueue::nextBucketId(int bucketId) const {
    return (bucketId + 1) % m_entries.size();
}

void EntriesQueue::copyEntries(int bucketId, int offset, int amount,
                               std::vector<MonitorEntry> &entries) const {
    int toFetch = amount;
    while (entries.size() < static_cast<unsigned int>(amount)) {
        auto &bucket = m_entries[bucketId];
        int bucketAmount = std::min(offset + toFetch,
                                     static_cast<int>(bucket.entries.size()));
        entries.insert(entries.end(), bucket.entries.begin() + offset,
                                      bucket.entries.begin() + bucketAmount);
        bucketId = nextBucketId(bucketId);
        toFetch -= (bucketAmount - offset);
        offset = 0;
    }
}

void EntriesQueue::removeEntries(int endBucketId, int endOffset) {
    int bucketId = m_firstBucketId;
    while (bucketId != endBucketId) {
        eraseBucket(bucketId);
        bucketId = nextBucketId(bucketId);
    }
    m_size -= endOffset - m_entries[bucketId].offset;
    m_entries[bucketId].offset = endOffset;
    m_firstBucketId = bucketId;
}

} /* namespace Cynara */
