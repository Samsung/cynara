/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/client/api/ApiInterface.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains libcynara-client API interface definition.
 */

#ifndef SRC_CLIENT_API_APIINTERFACE_H_
#define SRC_CLIENT_API_APIINTERFACE_H_

#include <string>

#include <cynara-client.h>
#include <types/ClientSession.h>

namespace Cynara {

class ApiInterface {
public:
    ApiInterface() = default;
    virtual ~ApiInterface() {};

    virtual int check(const std::string &client, const ClientSession &session,
                      const std::string &user, const std::string &privilege) = 0;
    virtual int simpleCheck(const std::string &client, const ClientSession &session,
                            const std::string &user, const std::string &privilege) = 0;
    virtual void flushMonitor(void) = 0;
};

} // namespace Cynara

#endif /* SRC_CLIENT_API_APIINTERFACE_H_ */
