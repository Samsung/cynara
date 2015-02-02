/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/cyad/Cyad.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       A command-line tool to manage Cynara's database
 */

#include <new>
#include <ostream>

#include <cynara-error.h>

#include <cyad/AdminLibraryInitializationFailedException.h>

#include "Cyad.h"

namespace Cynara {

Cyad::Cyad(int argc, char **argv) : m_parser(argc, argv), m_cynaraInitError(CYNARA_API_SUCCESS) {
    try {
        m_dispatcher.reset(new CommandsDispatcher(m_io, m_adminApiWrapper, m_errorApiWrapper));
    } catch (const Cynara::AdminLibraryInitializationFailedException &ex) {
        m_cynaraInitError = ex.errorCode();
    }
}

Cyad::~Cyad() {}

int Cyad::run(void) {
    if (cynaraOperational() == false) {
        m_io.cerr() << "Failed to initialize libcynara-admin: " << m_cynaraInitError << std::endl;
        return m_cynaraInitError;
    }

    try {
        auto command = m_parser.parseMain();
        return command->run(*m_dispatcher);
    } catch (const std::bad_alloc &) {
        m_io.cerr() << "Cyad could not allocate memory" << std::endl;
        return CYNARA_API_OUT_OF_MEMORY;
    }
}

bool Cyad::cynaraOperational(void) const {
    return m_cynaraInitError == CYNARA_API_SUCCESS;
}

} /* namespace Cynara */
