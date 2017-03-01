/*
 *  Copyright (c) 2016-2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/monitor/notify/FdNotifyObject.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of cynara's fd notification object
 */

#include <cerrno>
#include <cstring>
#include <sys/eventfd.h>
#include <unistd.h>

#include <log/log.h>

#include "FdNotifyObject.h"

namespace Cynara {

FdNotifyObject::FdNotifyObject() : m_eventFd(-1) {
}

bool FdNotifyObject::init(void) {
    m_eventFd = eventfd(0, 0);
    if (m_eventFd == -1) {
        int err = errno;
        LOGE("Couldn't initialize event fd: " << strerror(err));
        return false;
    }
    return true;
}

int FdNotifyObject::getNotifyFd(void) {
    return m_eventFd;
}

bool FdNotifyObject::notify(void) {
    int ret = eventfd_write(m_eventFd, 1);
    if (ret == -1) {
        int err = errno;
        LOGE("Couldn't write to event fd " << strerror(err));
        return false;
    }
    return true;
}

bool FdNotifyObject::snooze(void) {
    eventfd_t value;
    int ret = eventfd_read(m_eventFd, &value);
    if (ret == -1) {
        int err = errno;
        LOGE("Couldn't read from event fd " << strerror(err));
        return false;
    }
    return true;

}

FdNotifyObject::~FdNotifyObject() {
    close(m_eventFd);
}

} /* namespace Cynara */
