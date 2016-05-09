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
 * @brief       This file defines class for managing monitor entries
 */

#ifndef SRC_SERVICE_MONITOR_ENTRIESMANAGER_H_
#define SRC_SERVICE_MONITOR_ENTRIESMANAGER_H_

#include <list>
#include <map>
#include <vector>
#include <queue>

#include <request/RequestContext.h>
#include <types/MonitorEntry.h>

#include "EntriesQueue.h"

namespace Cynara {

class EntriesManager {
public:
    EntriesManager() = default;
    /*
     * Add monitor entry. Entries are stored in the order of arrival.
     * If max size is reached, least recently added entry will be deleted and all clients will be
     * re-registered appropriately.
     * Returns true, if entry has been added to container. Entries won't be stored if no clients
     * are waiting.
     */
    bool addEntry(const MonitorEntry &entry);
    /*
     * Get first registered client id for which we have enough entries.
     */
    RequestContext::ClientId getFilledClientId(void);

    /*
     * Check if enough entries are stored for given client.
     */
    bool isClientFilled(const RequestContext::ClientId &id);
    /*
     * Get entries for given client, if we don't have enough entries stored, empty vector is
     * returned.
     * If client is not registered for waiting, no entries will be fetched.
     * Force if you want to fetch available entries even when we store less than client requested.
     * Client context is updated, so he won't receive same entries in next fetch.
     * After fetch client is unregistered and won't come up in getFilledClientId.
     * To re-register client use modify.
     */
    std::vector<MonitorEntry> fetchEntriesForClient(RequestContext::ClientId clientId,
                                                    bool force = false);
    /*
     * Add new client. Client will be registered only for new entries (if any entries are already
     * stored, they will not be accessible for new client).
     * Returns true when client was successfully added.
     */
    bool addClient(RequestContext::ClientId clientId, uint16_t bufferSize);
    /*
     * Modify client buffer size.
     * Returns false when not registered client passed.
     */
    bool modifyClient(RequestContext::ClientId, uint16_t bufferSize);
    /*
     * Permanently delete client context.
     * Returns false when not registered client passed.
     */
    bool removeClient(RequestContext::ClientId clientId);

private:
    struct ClientInfo {
        ClientInfo(RequestContext::ClientId _clientId, int _bufferSize)
            : clientId(std::move(_clientId)), from(0),
              bufferSize(_bufferSize)
        {}
        ClientInfo(RequestContext::ClientId _clientId, EntriesQueue::EntryId _from,
                   int _bufferSize)
            : clientId(std::move(_clientId)), from(std::move(_from)), bufferSize(_bufferSize)
        {}
        RequestContext::ClientId clientId;
        EntriesQueue::EntryId from;
        int bufferSize;
    };

    typedef std::map<RequestContext::ClientId, ClientInfo> ClientInfoMap;
    typedef std::map<RequestContext::ClientId, ClientInfoMap::iterator> ClientInfoSubMap;
    typedef std::map<EntriesQueue::EntryId, std::list<ClientInfoMap::iterator>> ClientEntryIdMap;

    void registerClient(RequestContext::ClientId clientId, ClientInfoMap::iterator clientIt);
    void updateClient(ClientInfoMap::iterator clientIt, EntriesQueue::EntryId entryId);
    void addEntryCount(EntriesQueue::EntryId entryId, ClientInfoMap::iterator clientIt);
    void unregisterClient(RequestContext::ClientId client);
    void removeEntryCount(EntriesQueue::EntryId entryId, RequestContext::ClientId client);
    void cleanup(void);
    bool isClientFilled(const ClientInfo &client);
    void squashRequests();

    EntriesQueue m_container;

    /*
     * Map of all client contexts
     */
    ClientInfoMap m_clients;
    /*
     * Submap of all clients - only clients actively waiting for entries are here.
     */
    ClientInfoSubMap m_registeredClients;
    /*
     * Mapping entry id to list of clients waiting from given id.
     */
    ClientEntryIdMap m_entryCountMap;

    template <class Pred>
    ClientEntryIdMap::iterator
    friend squashEntries(ClientEntryIdMap::iterator from,
                         ClientEntryIdMap::iterator to,
                         EntriesQueue::EntryId newId,
                         std::list<ClientInfoMap::iterator> &listToUpdate, Pred p);
};

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

} /* namespace Cynara */

#endif /* SRC_SERVICE_MONITOR_ENTRIESMANAGER_H_ */
