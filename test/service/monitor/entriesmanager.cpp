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
 * @file        test/service/monitor/entriesmanager.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests of EntriesManager
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ctime>

#include <service/monitor/EntriesManager.h>
#include <common/types/MonitorEntry.h>

#include "../../helpers.h"

using namespace Cynara;

namespace {

typedef std::vector<MonitorEntry> MonitorEntries;

} /* namespace anonymous */

TEST(EntriesManager, oneClient) {
    EntriesManager entriesManager;
    auto filledClient = entriesManager.getFilledClientId();
    ASSERT_EQ(-1, filledClient);
}

TEST(EntriesManager, oneClientNotFilled) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 3);
    auto filledClient = entriesManager.getFilledClientId();
    ASSERT_EQ(-1, filledClient);
}

TEST(EntriesManager, oneClientFilled) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 3);
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    auto filledClient = entriesManager.getFilledClientId();
    ASSERT_EQ(666, filledClient);
}

TEST(EntriesManager, oneClientModified) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 5);

    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    ASSERT_EQ(-1, entriesManager.getFilledClientId());

    entriesManager.modifyClient(666, 3);
    ASSERT_EQ(666, entriesManager.getFilledClientId());
}

TEST(EntriesManager, oneClientRemoved) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 3);

    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    ASSERT_EQ(666, entriesManager.getFilledClientId());

    entriesManager.removeClient(666);
    ASSERT_EQ(-1, entriesManager.getFilledClientId());
}

TEST(EntriesManager, fetchForOne) {
    EntriesManager entriesManager;
    MonitorEntries expectedEntries;

    expectedEntries.push_back({{"c1", "u1", "p1"}, 0, {0, 0}});
    expectedEntries.push_back({{"c2", "u2", "p2"}, 0, {0, 0}});
    expectedEntries.push_back({{"c3", "u3", "p3"}, 0, {0, 0}});

    entriesManager.addClient(666, 3);

    entriesManager.addEntry(expectedEntries.at(0));
    entriesManager.addEntry(expectedEntries.at(1));
    entriesManager.addEntry(expectedEntries.at(2));

    auto actualEntries = entriesManager.fetchEntriesForClient(666);
    ASSERT_EQ(expectedEntries, actualEntries);
}

TEST(EntriesManager, fetchForMore) {
    EntriesManager entriesManager;
    MonitorEntries expectedEntries;

    expectedEntries.push_back({{"c1", "u1", "p1"}, 0, {0, 0}});
    expectedEntries.push_back({{"c2", "u2", "p2"}, 0, {0, 0}});
    expectedEntries.push_back({{"c3", "u3", "p3"}, 0, {0, 0}});
    expectedEntries.push_back({{"c4", "u4", "p4"}, 0, {0, 0}});

    entriesManager.addClient(666, 3);
    entriesManager.addClient(777, 4);

    entriesManager.addEntry(expectedEntries.at(0));
    entriesManager.addEntry(expectedEntries.at(1));
    entriesManager.addEntry(expectedEntries.at(2));

    // 666 should be filled as they are waiting for only 3 entries
    ASSERT_TRUE(entriesManager.isClientFilled(666));
    ASSERT_FALSE(entriesManager.isClientFilled(777));

    // 666 should get their 3 entries, then be unregistered
    auto actualEntries666 = entriesManager.fetchEntriesForClient(666);
    ASSERT_EQ(actualEntries666, Helpers::sliced<MonitorEntries>(expectedEntries.begin(), 3));
    ASSERT_FALSE(entriesManager.isClientFilled(666));

    // Add one more entry and assert 777 is filled
    entriesManager.addEntry(expectedEntries.at(3));
    ASSERT_TRUE(entriesManager.isClientFilled(777));
    auto actualEntries777 = entriesManager.fetchEntriesForClient(777);
    ASSERT_EQ(actualEntries777, expectedEntries);
    ASSERT_FALSE(entriesManager.isClientFilled(777));
}

TEST(EntriesManager, user1) {
    EntriesManager entriesManager;
    ASSERT_FALSE(entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}}));
    ASSERT_TRUE(entriesManager.addClient(666, 1));
    ASSERT_FALSE(entriesManager.isClientFilled(666));
}

TEST(EntriesManager, user2) {
    EntriesManager entriesManager;
    ASSERT_FALSE(entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}}));
    ASSERT_TRUE(entriesManager.addClient(666, 1));
    ASSERT_FALSE(entriesManager.isClientFilled(666));
    ASSERT_EQ(-1, entriesManager.getFilledClientId());
    ASSERT_TRUE(entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}}));
    ASSERT_TRUE(entriesManager.isClientFilled(666));
}

TEST(EntriesManager, user3) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    ASSERT_FALSE(entriesManager.addEntry(entry1));
    ASSERT_TRUE(entriesManager.addClient(666, 1));
    ASSERT_FALSE(entriesManager.isClientFilled(666));
    ASSERT_EQ(-1, entriesManager.getFilledClientId());
    ASSERT_TRUE(entriesManager.addEntry(entry2));
    ASSERT_TRUE(entriesManager.isClientFilled(666));
    ASSERT_EQ(666, entriesManager.getFilledClientId());
    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry2));
}

TEST(EntriesManager, user4) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 2));
    ASSERT_TRUE(entriesManager.addClient(777, 2));

    ASSERT_TRUE(entriesManager.addEntry(entry1));
    ASSERT_TRUE(entriesManager.addEntry(entry2));

    ASSERT_TRUE(entriesManager.isClientFilled(666));
    ASSERT_TRUE(entriesManager.isClientFilled(777));

    ASSERT_TRUE(entriesManager.removeClient(666));

    ASSERT_TRUE(entriesManager.isClientFilled(777));
    ASSERT_THAT(entriesManager.fetchEntriesForClient(777), ElementsAre(entry1, entry2));
}

TEST(EntriesManager, user5) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry3{{"c3", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 1));

    ASSERT_TRUE(entriesManager.addEntry(entry1));

    ASSERT_TRUE(entriesManager.addClient(777, 2))
                        ;
    ASSERT_TRUE(entriesManager.addEntry(entry2));
    ASSERT_TRUE(entriesManager.addEntry(entry3));

    ASSERT_TRUE(entriesManager.removeClient(666));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(777), ElementsAre(entry2, entry3));
}

TEST(EntriesManager, user6) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry3{{"c3", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 2));

    ASSERT_TRUE(entriesManager.addEntry(entry1));

    ASSERT_TRUE(entriesManager.addClient(777, 2))
                        ;
    ASSERT_TRUE(entriesManager.addEntry(entry2));
    ASSERT_TRUE(entriesManager.addEntry(entry3));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry1, entry2));
    ASSERT_THAT(entriesManager.fetchEntriesForClient(777), ElementsAre(entry2, entry3));
}

TEST(EntriesManager, user7) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry3{{"c3", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry4{{"c4", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 2));

    ASSERT_TRUE(entriesManager.addEntry(entry1));
    ASSERT_TRUE(entriesManager.addEntry(entry2));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry1, entry2));

    ASSERT_TRUE(entriesManager.modifyClient(666, 2));

    ASSERT_TRUE(entriesManager.addEntry(entry3));
    ASSERT_TRUE(entriesManager.addEntry(entry4));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry3, entry4));
}

TEST(EntriesManager, user8) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry3{{"c3", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry4{{"c4", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 2));
    ASSERT_TRUE(entriesManager.addClient(777, 2));

    ASSERT_TRUE(entriesManager.addEntry(entry1));
    ASSERT_TRUE(entriesManager.addEntry(entry2));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry1, entry2));
    ASSERT_TRUE(entriesManager.modifyClient(666, 2));

    ASSERT_TRUE(entriesManager.addEntry(entry3));
    ASSERT_TRUE(entriesManager.addEntry(entry4));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry3, entry4));
    ASSERT_THAT(entriesManager.fetchEntriesForClient(777), ElementsAre(entry1, entry2));
}

TEST(EntriesManager, user9) {
    using ::testing::ElementsAre;
    using ::testing::IsEmpty;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry3{{"c3", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry4{{"c4", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 2));

    ASSERT_TRUE(entriesManager.addEntry(entry1));
    ASSERT_TRUE(entriesManager.addEntry(entry2));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry1, entry2));
    ASSERT_TRUE(entriesManager.removeClient(666));

    ASSERT_FALSE(entriesManager.addEntry(entry3));
    ASSERT_FALSE(entriesManager.addEntry(entry4));

    ASSERT_TRUE(entriesManager.addClient(666, 2));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), IsEmpty());
}

TEST(EntriesManager, userFlush) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry3{{"c3", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 4));

    ASSERT_TRUE(entriesManager.addEntry(entry1));
    ASSERT_TRUE(entriesManager.addEntry(entry2));
    ASSERT_TRUE(entriesManager.addEntry(entry3));

    ASSERT_THAT(entriesManager.fetchEntriesForClient(666, true), ElementsAre(entry1, entry2, entry3));
}

TEST(EntriesManager, userFlushNone) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    ASSERT_TRUE(entriesManager.addClient(666, 4));
    ASSERT_THAT(entriesManager.fetchEntriesForClient(666, true), ElementsAre());
}

TEST(EntriesManager, userFetchFlush) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    MonitorEntry entry1{{"c1", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry2{{"c2", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry3{{"c3", "u", "p"}, 0, {0, 0}};
    MonitorEntry entry4{{"c4", "u", "p"}, 0, {0, 0}};

    ASSERT_TRUE(entriesManager.addClient(666, 3));

    ASSERT_TRUE(entriesManager.addEntry(entry1));
    ASSERT_TRUE(entriesManager.addEntry(entry2));
    ASSERT_TRUE(entriesManager.addEntry(entry3));
    ASSERT_THAT(entriesManager.fetchEntriesForClient(666), ElementsAre(entry1, entry2, entry3));

    ASSERT_TRUE(entriesManager.addEntry(entry4));
    ASSERT_TRUE(entriesManager.modifyClient(666, 3));
    ASSERT_THAT(entriesManager.fetchEntriesForClient(666, true), ElementsAre(entry4));
}
