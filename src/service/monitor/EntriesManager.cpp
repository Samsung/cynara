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
 * @file        src/service/monitor/EntriesManager.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements class for managing monitor entries
 */

#include <algorithm>
#include <list>
#include <ctime>

#include <log/log.h>

#include "EntriesManager.h"

bool operator<(const struct timespec &lhs, const struct timespec &rhs) {
    return lhs.tv_sec < rhs.tv_sec || (lhs.tv_sec == rhs.tv_sec && lhs.tv_nsec < rhs.tv_nsec);
}

namespace Cynara {

bool EntriesManager::isClientFilled(const ClientInfo &client) {
    return m_container.sizeFrom(client.from) >= client.bufferSize;
}

RequestContext::ClientId EntriesManager::getFilledClientId(void) {
    for (auto registeredClient : m_registeredClients) {
        auto &registeredClientInfo = registeredClient.second->second;
        if (m_container.sizeFrom(registeredClientInfo.from) >= registeredClientInfo.bufferSize) {
            return registeredClientInfo.clientId;
        }
    }
    return -1;
}

bool EntriesManager::isClientFilled(const RequestContext::ClientId &id) {
    auto clientIt = m_registeredClients.find(id);
    if (clientIt == m_registeredClients.end()) {
        LOGW("Client [" << id << "] is not registered");
        return false;
    }
    return isClientFilled(clientIt->second->second);
}

std::vector<MonitorEntry> EntriesManager::fetchEntriesForClient(RequestContext::ClientId clientId,
                                                                bool force) {
    auto regClientIt = m_registeredClients.find(clientId);
    if (regClientIt == m_registeredClients.end()) {
        LOGW("Client <" << clientId << "> not registered");
        return {};
    }
    auto clientIt = regClientIt->second;
    auto &clientInfo = clientIt->second;
    if (!force && !isClientFilled(clientInfo)) {
        LOGW("Not enough entries for client " << clientId);
        return {};
    }

    int entriesToFetch = std::min(m_container.sizeFrom(clientInfo.from), clientInfo.bufferSize);
    std::vector<MonitorEntry> entries;
    auto nextNotFetchedEntry = m_container.fetch(clientInfo.from, entriesToFetch, entries);

    updateClient(clientIt, nextNotFetchedEntry);
    //When entries are fetched for client, client is removed from waiting queue
    unregisterClient(clientId);
    cleanup();
    return entries;
}
bool EntriesManager::addClient(RequestContext::ClientId clientId, uint16_t bufferSize) {
    if (clientId == RequestContext::InvalidClientId) {
        LOGE("Invalid client id");
        return false;
    }
    if (m_clients.find(clientId) != m_clients.end()) {
        LOGE("Client [" << clientId << "] already exists, use modify instead");
        return false;
    }
    /*
     * For the first time client is registered for future entry, after this its state is kept,
     * so client can fetch entries continuously
     */
    EntriesQueue::EntryId startPoint = m_container.getBackEntryId();
    auto itInfo = m_clients.emplace(clientId, ClientInfo(clientId, startPoint, bufferSize));
    addEntryCount(startPoint, itInfo.first);
    registerClient(clientId, itInfo.first);
    return true;
}

bool EntriesManager::removeClient(RequestContext::ClientId clientId) {
    auto clientIt = m_clients.find(clientId);
    if (clientIt == m_clients.end()) {
        LOGE("Requested client [" << clientId << "] not found");
        return false;
    }

    unregisterClient(clientId);
    removeEntryCount(clientIt->second.from, clientIt->second.clientId);
    m_clients.erase(clientId);
    cleanup();
    return true;
}

bool EntriesManager::modifyClient(RequestContext::ClientId clientId, uint16_t bufferSize) {
    auto clientIt = m_clients.find(clientId);
    if (clientIt == m_clients.end()) {
        LOGE("Unknow client [" << clientId << "]");
        return false;
    }
    clientIt->second.bufferSize = bufferSize;
    registerClient(clientId, clientIt);
    return true;
}

bool EntriesManager::addEntry(const MonitorEntry &entry) {
    if (m_clients.empty()) {
        LOGD("Ignoring new entry, no client waiting");
        return false;
    }
    if (m_container.push(entry))
        squashRequests();
    return true;
}

void EntriesManager::updateClient(ClientInfoMap::iterator clientIt, EntriesQueue::EntryId entryId) {
    auto &clientInfo = clientIt->second;
    auto oldId = clientInfo.from;

    removeEntryCount(oldId, clientInfo.clientId);
    clientInfo.from = entryId;
    addEntryCount(entryId, clientIt);
}

void EntriesManager::addEntryCount(EntriesQueue::EntryId entryId,
                                   ClientInfoMap::iterator clientIt) {
    m_entryCountMap[entryId].push_back(clientIt);
}

void EntriesManager::registerClient(RequestContext::ClientId clientId,
                                    ClientInfoMap::iterator clientIt) {
    m_registeredClients[clientId] = clientIt;
}

void EntriesManager::removeEntryCount(EntriesQueue::EntryId entryId,
                                      RequestContext::ClientId client) {
    auto it = m_entryCountMap.find(entryId);
    auto &clientList = it->second;
    if (clientList.size() == 1) {
        m_entryCountMap.erase(it);
        return;
    }
    auto clientIt = std::find_if(clientList.begin(), clientList.end(),
            [client] (const ClientInfoMap::iterator &it) {
                        return it->second.clientId == client;
            });
    clientList.erase(clientIt);
}
void EntriesManager::unregisterClient(RequestContext::ClientId clientId) {
    m_registeredClients.erase(clientId);
}

void EntriesManager::cleanup() {
    if (m_clients.empty()) {
        m_container.clear();
        return;
    }

    auto lowestWaitedEntryIdIt = m_entryCountMap.begin();
    if (lowestWaitedEntryIdIt == m_entryCountMap.end()) {
        return;
    }
    m_container.popUntil(lowestWaitedEntryIdIt->first);
}

template <class Pred>
EntriesManager::ClientEntryIdMap::iterator
squashEntries(EntriesManager::ClientEntryIdMap::iterator from,
              EntriesManager::ClientEntryIdMap::iterator to,
              EntriesQueue::EntryId newId,
              std::list<EntriesManager::ClientInfoMap::iterator> &listToUpdate, Pred p) {
    auto it = from;
    for (; it != to && p(it); it++) {
        auto &clientItList = it->second;
        for (auto &clientIt : clientItList) {
            clientIt->second.from = newId;
            listToUpdate.push_back(clientIt);
        }
    }
    return it;
}

void EntriesManager::squashRequests() {
    /*
     * This is ugly. If container got overflowed, it's front Id value change, meaning that all
     * clients waiting for entries with earlier id than queue front id get invalidated.
     * I would be easy if container weren't circular, but it is. We have to handle both cases:
     * when front id is lower than back id and when it's the other way around.
     */
    auto frontId = m_container.getFrontEntryId();
    auto backId = m_container.getBackEntryId();

    // we are moving clients waiting for invalidated entries to this list
    auto &listToUpdate = m_entryCountMap[frontId];

    if (frontId > backId) {
        /*
         * We have situation like this:
         *
         * Queue: | X | X | backId | | | | frontId | X | X |
         * All id's which are between backId and frontId get invalidated, so all clients waiting
         * for these id's need to be moved to frontId
         */
        // Get first iterator with value bigger than backId
        auto startIt = m_entryCountMap.lower_bound(backId);
        auto stopIt = squashEntries(startIt, m_entryCountMap.end(), frontId, listToUpdate,
                                    [frontId] (const ClientEntryIdMap::iterator &it)
                                        {return it->first < frontId;});
        m_entryCountMap.erase(startIt, stopIt);
    } else {
        /*
         * Normal situation, move all clients waiting for entryId < firstId or
         * entryId > backId (remember that when queue is circular, frontId might be incremented to
         * lesser value and we have to remember about clients waiting for entryId bigger even than
         * backId) to firstId.
         */

        auto startIt = m_entryCountMap.begin();
        auto stopIt = squashEntries(startIt, m_entryCountMap.end(), frontId, listToUpdate,
                                    [frontId] (const ClientEntryIdMap::iterator &it)
                                        {return it->first < frontId;});
        m_entryCountMap.erase(startIt, stopIt);

        startIt = m_entryCountMap.lower_bound(backId);
        stopIt = squashEntries(startIt, m_entryCountMap.end(), frontId, listToUpdate,
                               [frontId] (const ClientEntryIdMap::iterator &) {return true;});
        m_entryCountMap.erase(startIt, stopIt);
    }
}

} /* namespace Cynara */
