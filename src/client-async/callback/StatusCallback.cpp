/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/client-async/callback/StatusCallback.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of StatusCallback class
 */

#include "StatusCallback.h"

namespace Cynara {

StatusCallback::StatusCallback(cynara_status_callback callback, void *userData)
    : m_callback(callback), m_userData(userData), m_sockFd(-1),
      m_status(cynara_async_status::CYNARA_STATUS_FOR_READ) {
}

void StatusCallback::onStatusChange(int newFd, cynara_async_status status) {
    if (!m_callback)
        return;
    if (m_status == status && m_sockFd == newFd)
        return;
    m_callback(m_sockFd, newFd, status, m_userData);
    m_sockFd = newFd;
    m_status = status;
}

void StatusCallback::onDisconnected(void) {
    onStatusChange(-1, cynara_async_status::CYNARA_STATUS_FOR_READ);
}

} // namespace Cynara
