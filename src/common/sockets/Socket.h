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
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of UNIX client socket class
 */

#ifndef SRC_COMMON_SOCKETS_SOCKET_H_
#define SRC_COMMON_SOCKETS_SOCKET_H_

#include <string>

#include <containers/BinaryQueue.h>
#include <containers/RawBuffer.h>

namespace Cynara {

class Socket {
public:
    enum class ConnectionStatus {
        ALREADY_CONNECTED,
        CONNECTION_SUCCEEDED,
        CONNECTION_IN_PROGRESS,
        CONNECTION_FAILED
    };

    enum class SendStatus {
        PARTIAL_DATA_SENT,
        ALL_DATA_SENT,
        CONNECTION_LOST
    };

private:
    int m_sock;
    bool m_connectionInProgress;

    std::string m_socketPath;
    int m_pollTimeout;

    RawBuffer m_sendBuffer;
    size_t m_sendBufferPos;
    size_t m_sendBufferEnd;
    BinaryQueue m_sendQueue;

    void close(void);

    //returns true      if socket is ready
    //returns false     in case of timeout
    //throws            in critical situations
    bool waitForSocket(int event);

    //returns int       errorcode read from socket
    //throws            in critical situations
    int getSocketError(void);

    //throws            in critical situations
    void createSocket(void);

    //returns ConnectionStatus::CONNECTION_SUCCEEDED           if connection succeeded
    //returns ConnectionStatus::CONNECTION_IN_PROGRESS         if connection in progress
    //returns ConnectionStatus::CONNECTION_FAILED              if connection failed
    //throws                                                   in critical situations
    ConnectionStatus connectSocket(void);

    //returns SendStatus::PARTIAL_DATA_SENT         if no data to send is available
    //returns SendStatus::ALL_DATA_SENT             if no additional data to send
    //returns SendStatus::CONNECTION_LOST           if connection was lost
    //throws                                        in critical situations
    SendStatus sendBuffer(void);

public:
    Socket(const std::string &socketPath, int timeoutMiliseconds = -1);
    ~Socket();

    //returns bool      connection state (true - connected, false - not connected))
    //throws            in critical situations
    bool isConnected(void);

    //returns ConnectionStatus::CONNECTION_SUCCEEDED           if connection succeeded
    //returns ConnectionStatus::CONNECTION_IN_PROGRESS         if connection in progress
    //returns ConnectionStatus::CONNECTION_FAILED              if connection failed
    //throws                                                   in critical situations
    ConnectionStatus connect(void);

    //returns ConnectionStatus::ALREADY_CONNECTED              if was already connected
    //returns ConnectionStatus::CONNECTION_SUCCEEDED           if connection succeeded
    //returns ConnectionStatus::CONNECTION_IN_PROGRESS         if connection in progress
    //returns ConnectionStatus::CONNECTION_FAILED              if connection failed
    //throws                                                   in critical situations
    ConnectionStatus completeConnection(void);

    //returns socket descriptor
    //returns -1                if socket descriptor no present
    int getSockFd(void);

    //returns true          There is still data to send
    //returns false         No data to send
    bool isDataToSend(void);

    //returns SendStatus::PARTIAL_DATA_SENT         if no all data sent
    //returns SendStatus::ALL_DATA_SENT             if all data was sent
    //returns SendStatus::CONNECTION_LOST           if connection was lost
    //throws                                        in critical situations
    SendStatus sendToServer(BinaryQueue &queue);

    //returns true                              if data was successfully read from server
    //returns false                             if connection was lost
    //throws                                    in critical situations
    bool receiveFromServer(BinaryQueue &queue);
};

} // namespace Cynara

#endif /* SRC_COMMON_SOCKETS_SOCKET_H_ */
