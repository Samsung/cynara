/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/lock/FileLock.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       A class for acquiring and holding file lock
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <exceptions/CannotCreateFileException.h>
#include <exceptions/FileLockAcquiringException.h>
#include <log/log.h>

#include "FileLock.h"

namespace Cynara {

Lockable::Lockable(const std::string &lockFilename) {
    m_fd = TEMP_FAILURE_RETRY(::open(lockFilename.c_str(), O_RDONLY));

    if (m_fd < 0) {
        LOGE("Could not open lock file <%s>", lockFilename.c_str());
        throw FileLockAcquiringException(errno);
    }

    LOGD("File lock file opened");
}

Lockable::~Lockable() {
    ::close(m_fd);
}

FileLock::FileLock(Lockable &lockable) : m_lockable(lockable) {}

FileLock::~FileLock() {
    unlock();
}

bool FileLock::tryLock(void) {
    int lock = TEMP_FAILURE_RETRY(::flock(m_lockable.m_fd, LOCK_EX | LOCK_NB));

    if (lock == 0) {
        LOGI("File lock acquired");
        return true;
    } else if (errno == EWOULDBLOCK) {
        LOGI("File lock NOT acquired");
        return false;
    }

    throw FileLockAcquiringException(errno);
}

void FileLock::lock(void) {
    int lock = TEMP_FAILURE_RETRY(::flock(m_lockable.m_fd, LOCK_EX));

    if (lock == -1)
        throw FileLockAcquiringException(errno);

    LOGI("File lock acquired");
}

void FileLock::unlock(void) {
    LOGI("Releasing file lock");
    (void) TEMP_FAILURE_RETRY(::flock(m_lockable.m_fd, LOCK_UN));
}

} /* namespace Cynara */
