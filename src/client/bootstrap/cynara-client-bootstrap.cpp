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
/*
 * @file        cynara-client-bootstrap.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains bootstrap version of libcynara-client API implementation.
 */

#include <security-server.h>
#include "cynara-client-bootstrap.h"

CynaraClientBootstrap :: CynaraClientBootstrap() {
}

CynaraClientBootstrap :: ~CynaraClientBootstrap() {
}

cynara_api_result CynaraClientBootstrap :: check(const std::string& client,
    const std::string& session UNUSED, const std::string& user UNUSED, const std::string& privilege)
{
    int is_enabled = 0;

    int ret = security_server_app_has_privilege(client.c_str(), APP_TYPE_WGT, privilege.c_str(),
                                                &is_enabled);

    if(ret == PC_OPERATION_SUCCESS && is_enabled)
        return cynara_api_result::CYNARA_API_SUCCESS;
    return cynara_api_result::CYNARA_API_ACCESS_DENIED;
}
