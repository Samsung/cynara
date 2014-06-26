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
 * @file        Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-client class
 */

#include <common.h>

#include "Logic.h"

namespace Cynara {

cynara_api_result Logic::check(const std::string &client UNUSED, const std::string &session UNUSED,
                               const std::string &user UNUSED, const std::string &privilege UNUSED)
{
    //todo - this is a stub
    return cynara_api_result::CYNARA_API_ACCESS_DENIED;
}

} // namespace Cynara
