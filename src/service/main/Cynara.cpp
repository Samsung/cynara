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
/*
 * @file        Cynara.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements main class of cynara service
 */

#include <stddef.h>

#include <storage/InMemoryStorageBackend.h>

#include "Cynara.h"

namespace Cynara {

Cynara::Cynara()
    : m_logic(nullptr), m_socketManager(nullptr), m_storage(nullptr), m_storageBackend(nullptr) {
}

Cynara* Cynara::getInstance(void) {
    static Cynara instance;
    return &instance;
}

Cynara::~Cynara() {
}

void Cynara::init(void) {
    getInstance()->m_logic = new Logic();
    getInstance()->m_socketManager = new SocketManager();
    getInstance()->m_storageBackend = new InMemoryStorageBackend();
    getInstance()->m_storage = new Storage(*getInstance()->m_storageBackend);
}

void Cynara::run(void) {
    getInstance()->m_socketManager->run();
}

void Cynara::finalize(void) {
    delete getInstance()->m_logic;
    delete getInstance()->m_socketManager;
    delete getInstance()->m_storageBackend;
    delete getInstance()->m_storage;
}

Logic *Cynara::getLogic(void) {
    return getInstance()->m_logic;
}

SocketManager *Cynara::getSocketManager(void) {
    return getInstance()->m_socketManager;
}

Storage *Cynara::getStorage(void) {
    return getInstance()->m_storage;
}

} // namespace Cynara
