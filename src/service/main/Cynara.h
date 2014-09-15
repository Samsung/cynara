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
 * @file        Cynara.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines main class of cynara service
 */

#ifndef SRC_SERVICE_MAIN_CYNARA_H_
#define SRC_SERVICE_MAIN_CYNARA_H_

#include <main/pointers.h>

namespace Cynara {

class Cynara {
private:
    LogicPtr m_logic;
    SocketManagerPtr m_socketManager;
    StoragePtr m_storage;
    StorageBackendPtr m_storageBackend;

    static const std::string storageDir(void);

public:
    Cynara();
    ~Cynara();

    void init(void);
    void run(void);
    void finalize(void);
};

} // namespace Cynara

#endif /* SRC_SERVICE_MAIN_CYNARA_H_ */
