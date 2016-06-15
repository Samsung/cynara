/*
 * Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/service/sockets/SocketManager.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines socket layer manager for cynara
 */

#ifndef SRC_SERVICE_SOCKETS_SOCKETMANAGER_H_
#define SRC_SERVICE_SOCKETS_SOCKETMANAGER_H_

#include <vector>
#include <memory>
#include <stdio.h>

#include <common.h>

#include <main/pointers.h>
#include <protocol/Protocol.h>
#include <request/RequestTaker.h>
#include "Descriptor.h"

namespace Cynara {

const size_t DEFAULT_BUFFER_SIZE = BUFSIZ;

class SocketManager {
public:
    SocketManager();
    ~SocketManager();

    void run(void);
    void mainLoopStop(void);

    void bindLogic(LogicPtr logic) {
        m_logic = logic;
    }

    void unbindAll(void) {
        m_logic.reset();
    }

    void disconnectAllClients(void);

private:
    LogicPtr m_logic;

    typedef std::vector<Descriptor> FDVector;
    FDVector m_fds;

    bool m_working;

    fd_set m_readSet;
    fd_set m_writeSet;
    int m_maxDesc;

    void init(void);
    void mainLoop(void);

    void readyForRead(int fd);
    void readyForWrite(int fd);
    void readyForAccept(int fd);
    void closeSocket(int fd);
    bool handleRead(int fd, const RawBuffer &readbuffer);

    void createDomainSocket(ProtocolPtr protocol, const std::string &path, mode_t mask,
                            bool client);
    static int createDomainSocketHelp(const std::string &path, mode_t mask);
#ifdef BUILD_WITH_SYSTEMD_DAEMON
    static int getSocketFromSystemD(const std::string &path);
#endif
    void createSignalSocket(ProtocolPtr protocol);

    Descriptor &createDescriptor(int fd, bool client);

    void addReadSocket(int fd);
    void removeReadSocket(int fd);
    void addWriteSocket(int fd);
    void removeWriteSocket(int fd);

    RequestTakerPtr requestTaker(void);
};

} // namespace Cynara

#endif /* SRC_SERVICE_SOCKETS_SOCKETMANAGER_H_ */
