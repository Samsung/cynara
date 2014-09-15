/*
 *  Copyright (c) 2000 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/common/sockets/Socket.h
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of UNIX client socket class
 */

#ifndef SRC_COMMON_SOCKETS_SOCKET_H_
#define SRC_COMMON_SOCKETS_SOCKET_H_

#include <string>

#include <containers/BinaryQueue.h>

namespace Cynara {

class Socket {
private:
    int m_sock;

    std::string m_socketPath;
    int m_pollTimeout;

    void close(void) noexcept;

    //returns true      if socket is ready
    //returns false     in case of timeout
    //throws            in critical situations
    bool waitForSocket(int event);

    //returns int       errorcode read from socket
    //throws            in critical situations
    int getSocketError(void);

public:
    Socket(const std::string &socketPath, int timeoutMiliseconds = -1);
    ~Socket();

    //returns bool      connection state (true - connected, false - not connected))
    //throws            in critical situations
    bool isConnected(void);

    //returns true      if connection succeeded
    //returns false     if connection was timeout or no one is listening
    //throws            in critical situations
    bool connect(void);

    //returns true                              if data was successfully send to server
    //returns false                             if connection was lost
    //throws ServerConnectionErrorException     if cannot connect server (or timeout)
    //throws other exceptions                   in critical situations
    bool sendToServer(BinaryQueue &queue);

    //returns true                              if data was successfully read from server
    //returns false                             if connection was lost
    //throws ServerConnectionErrorException     if cannot connect server (or timeout)
    //throws other exceptions                   in critical situations
    bool waitAndReceiveFromServer(BinaryQueue &queue);

    //returns true                              if data was successfully read from server
    //returns false                             if connection was lost
    //throws other exceptions                   in critical situations
    bool receiveFromServer(BinaryQueue &queue);
};

} // namespace Cynara

#endif /* SRC_COMMON_SOCKETS_SOCKET_H_ */
