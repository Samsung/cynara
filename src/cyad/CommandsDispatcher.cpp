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
 * @file        src/cyad/CommandsDispatcher.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       CommandsDispatcher class (implementation)
 */

#include <cynara-error.h>
#include <cynara-policy-types.h>

#include <cyad/AdminLibraryInitializationFailedException.h>

#include "CommandsDispatcher.h"

namespace Cynara {

CommandsDispatcher::CommandsDispatcher(BaseDispatcherIO &io, BaseAdminApiWrapper &adminApiWrapper)
    : m_io(io), m_adminApiWrapper(adminApiWrapper), m_cynaraAdmin(nullptr)
{
    auto ret = m_adminApiWrapper.cynara_admin_initialize(&m_cynaraAdmin);
    if (ret != CYNARA_API_SUCCESS)
        throw AdminLibraryInitializationFailedException(ret);
}

CommandsDispatcher::~CommandsDispatcher() {
    m_adminApiWrapper.cynara_admin_finish(m_cynaraAdmin);
}

int CommandsDispatcher::execute(CyadCommand &) {
    m_io.cout() << "Whatever you wanted, it's not implemented" << std::endl;
    return CYNARA_API_UNKNOWN_ERROR;
}

int CommandsDispatcher::execute(HelpCyadCommand &) {
    m_io.cout() << helpMessage << std::endl;
    return CYNARA_API_SUCCESS;
}

int CommandsDispatcher::execute(ErrorCyadCommand &result) {
    m_io.cout() << "There was an error in command-line options:" << std::endl;
    m_io.cout() << result.message() << std::endl;

    m_io.cout() << std::endl << helpMessage << std::endl;
    return CYNARA_API_INVALID_COMMANDLINE_PARAM;
}

int CommandsDispatcher::execute(DeleteBucketCyadCommand &result) {
    return m_adminApiWrapper.cynara_admin_set_bucket(m_cynaraAdmin, result.bucketId().c_str(),
                                                     CYNARA_ADMIN_DELETE, nullptr);
}

int CommandsDispatcher::execute(SetBucketCyadCommand &result) {
    const auto &policyResult = result.policyResult();
    const char *metadata = policyResult.metadata().empty() ? nullptr
                                                           : policyResult.metadata().c_str();
    return m_adminApiWrapper.cynara_admin_set_bucket(m_cynaraAdmin,
                                                     result.bucketId().c_str(),
                                                     policyResult.policyType(), metadata);
}

} /* namespace Cynara */
