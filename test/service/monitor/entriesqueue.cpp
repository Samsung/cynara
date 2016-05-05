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
 * @file        test/service/monitor/entriesqueue.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests of EntriesQueue
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <ctime>

#include <service/monitor/EntriesQueue.h>
#include <common/types/MonitorEntry.h>

#include "../../helpers.h"

using namespace Cynara;

namespace {

    typedef std::vector<MonitorEntry> MonitorEntries;

} /* namespace anonymous */

TEST(EntriesQueue, empty) {
    EntriesQueue queue(10, 2);
    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(10, queue.capacity());
    ASSERT_EQ(0, queue.size());
    ASSERT_EQ(0, queue.sizeFrom(queue.getFrontEntryId()));
    ASSERT_EQ(0, queue.getFrontEntryId());
    ASSERT_EQ(0, queue.getBackEntryId());
}

TEST(EntriesQueue, pushOne) {
    EntriesQueue queue;
    queue.push({{"c", "u", "p"}, 0, {0, 0}});
    ASSERT_EQ(1, queue.size());
    ASSERT_EQ(1, queue.sizeFrom(queue.getFrontEntryId()));
    ASSERT_EQ(queue.getFrontEntryId(), queue.getBackEntryId() - 1);
}

TEST(EntriesQueue, clear) {
    EntriesQueue queue;
    auto originalCapacity = queue.capacity();
    queue.push({{"c", "u", "p"}, 0, {0, 0}});
    ASSERT_EQ(1, queue.size());
    queue.clear();
    ASSERT_EQ(0, queue.size());
    ASSERT_EQ(originalCapacity, queue.capacity());

}

TEST(EntriesQueue, fetchFromEmpty) {
    EntriesQueue queue;
    MonitorEntries entries;
    ASSERT_EQ(-1, queue.fetch(queue.getFrontEntryId(), 1, entries));
}

TEST(EntriesQueue, fetchOne) {
    using ::testing::ElementsAre;

    EntriesQueue queue;
    MonitorEntry monitorEntry{{"c", "u", "p"}, 0, {0, 0}};
    queue.push(monitorEntry);
    std::vector<MonitorEntry> entries;

    auto nextId = queue.fetch(queue.getFrontEntryId(), 1, entries);
    ASSERT_EQ(1, nextId);
    ASSERT_THAT(entries, ElementsAre(monitorEntry));
    ASSERT_EQ(1, queue.size());

    queue.popUntil(nextId);
    ASSERT_EQ(0, queue.size());
    ASSERT_EQ(1, queue.getFrontEntryId());
    ASSERT_EQ(1, queue.getBackEntryId());
}

TEST(EntriesQueue, overflow) {
    EntriesQueue queue(10, 2);
    MonitorEntry monitorEntry1{{"c1", "u1", "p1"}, 0, {0, 0}};
    MonitorEntry monitorEntry2{{"c2", "u2", "p2"}, 0, {0, 0}};
    MonitorEntry monitorEntry11{{"c11", "u11", "p11"}, 0, {0, 0}};

    queue.push(monitorEntry1);
    queue.push(monitorEntry2);
    for (auto i = 0; i < 8; ++i)
        queue.push({{"cn", "un", "pn"}, 0, {0, 0}});

    // Now 1st entry should be overwritten
    auto overflow = queue.push(monitorEntry11);
    ASSERT_TRUE(overflow);

    std::vector<MonitorEntry> entries;
    auto sizeFromFront = queue.sizeFrom(queue.getFrontEntryId());
    ASSERT_EQ(10, sizeFromFront);
    queue.fetch(queue.getFrontEntryId(), sizeFromFront, entries);
    ASSERT_EQ(monitorEntry2, entries.at(0));
    ASSERT_EQ(monitorEntry11, entries.at(9));
}

TEST(EntriesQueue, pushFetch) {
    EntriesQueue queue(10, 2);
    MonitorEntries expectedEntries;

    for (auto i = 0; i < 20; ++i)
        expectedEntries.push_back({{"c", "u", "p"}, 0, {i, 0}});

    auto push = [&queue, &expectedEntries] (std::size_t from, std::size_t count) {
        for (auto i = from; i < from + count; ++i)
            queue.push(expectedEntries.at(i));
    };

    auto pop = [&queue] (std::size_t count) -> MonitorEntries {
        MonitorEntries entries;
        auto nextId = queue.fetch(queue.getFrontEntryId(), count, entries);
        queue.popUntil(nextId);
        return entries;
    };

    // push 5 entries [0-5)
    push(0, 5);
    ASSERT_EQ(5, queue.size());

    // pop 3 entries, compare [0-3)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin(), 3), pop(3));
    ASSERT_EQ(2, queue.size());

    // push 4 entries [5-9)
    push(5, 4);
    ASSERT_EQ(6, queue.size());

    // pop 5 entries, compare [3-8)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin() + 3, 5), pop(5));
    ASSERT_EQ(1, queue.size());

    // push 3 entries [9-12)
    push(9, 3);
    ASSERT_EQ(4, queue.size());

    // pop 4 entries, compare [8-12)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin() + 8, 4), pop(4));
    ASSERT_EQ(0, queue.size());

    // push 10 entries [10-20)
    push(10, 10);
    ASSERT_EQ(10, queue.size());

    // pop 10 entries, compare [10-20)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin() + 10, 10), pop(10));
    ASSERT_EQ(0, queue.size());
}

TEST(EntriesQueue, pushFetch2) {
    EntriesQueue queue(10, 2);
    MonitorEntries expectedEntries;

    for (auto i = 0; i < 20; ++i)
        expectedEntries.push_back({{"c", "u", "p"}, 0, {i, 0}});

    auto push = [&queue, &expectedEntries] (std::size_t from, std::size_t count) {
        for (auto i = from; i < from + count; ++i)
            queue.push(expectedEntries.at(i));
    };

    auto pop = [&queue] (std::size_t count) -> MonitorEntries {
        MonitorEntries entries;
        auto nextId = queue.fetch(queue.getFrontEntryId(), count, entries);
        queue.popUntil(nextId);
        return entries;
    };

    // push 5 entries [0-5)
    push(0, 5);
    ASSERT_EQ(5, queue.size());

    // pop 3 entries, compare [0-3)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin(), 3), pop(3));
    ASSERT_EQ(2, queue.size());

    // push 4 entries [5-9)
    push(5, 4);
    ASSERT_EQ(6, queue.size());

    // pop 5 entries, compare [3-8)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin() + 3, 5), pop(5));
    ASSERT_EQ(1, queue.size());

    // push 6 entries [9-15)
    push(9, 6);
    ASSERT_EQ(7, queue.size());

    // pop 5 entries, compare [8-13)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin() + 8, 5), pop(5));
    ASSERT_EQ(2, queue.size());

    // push 8 entries [10-18)
    push(15, 5);
    ASSERT_EQ(7, queue.size());

    // pop 7 entries, compare [13-20)
    ASSERT_EQ(Helpers::sliced<MonitorEntries>(expectedEntries.begin() + 13, 7), pop(7));
    ASSERT_EQ(0, queue.size());
}