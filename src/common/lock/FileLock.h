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
 * @file        src/common/lock/FileLock.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       A class for acquiring and holding file lock
 */

#ifndef SRC_COMMON_LOCK_FILELOCK_H_
#define SRC_COMMON_LOCK_FILELOCK_H_

#include <string>

namespace Cynara {

class FileLock;

class Lockable {
    friend FileLock;
public:
    explicit Lockable(const std::string &lockFilename);
    ~Lockable();
private:
    int m_fd;
};

class FileLock {
public:
    explicit FileLock(Lockable &lockable);
    ~FileLock();
    bool tryLock(void);
    void lock(void);
    void unlock(void);
private:
    Lockable &m_lockable;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_LOCK_FILELOCK_H_ */
