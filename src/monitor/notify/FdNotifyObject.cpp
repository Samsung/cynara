/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <unistd.h>

#include <log/log.h>

#include "FdNotifyObject.h"

namespace Cynara {

FdNotifyObject::FdNotifyObject() : m_pipeFd{-1, -1} {
}

bool FdNotifyObject::init(void) {
    int ret = pipe(m_pipeFd);
    if (ret != 0) {
        int err = errno;
        LOGE("Couldn't initialize pipes: " << strerror(err));
        return false;
    }
    return true;
}

int FdNotifyObject::getNotifyFd(void) {
    return m_pipeFd[0];
}

bool FdNotifyObject::notify(void) {
    const char wakeup[] = "w";
    int ret = TEMP_FAILURE_RETRY(write(m_pipeFd[1], wakeup, sizeof(wakeup)));
    if (ret == -1) {
        return false;
    }
    return true;
}

FdNotifyObject::~FdNotifyObject() {
    if (m_pipeFd[0] != -1)
        close(m_pipeFd[0]);
    if (m_pipeFd[1] != -1)
        close(m_pipeFd[1]);
}

} /* namespace Cynara */
