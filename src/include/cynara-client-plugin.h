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
 * @file        src/include/cynara-client-plugin.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Oskar Switalski <o.switalski@samsung.com>
 * @version     1.0
 * @brief       This file defines cynara client side of external plugin interface -
 *              ClientPluginInterface.
 */

#ifndef CYNARACLIENTPLUGIN_H_
#define CYNARACLIENTPLUGIN_H_

#include <memory>

#include <plugin/ExternalPluginInterface.h>
#include <types/ClientSession.h>
#include <types/PolicyResult.h>

namespace Cynara {

class ClientPluginInterface;
typedef std::shared_ptr<ClientPluginInterface> ClientPluginInterfacePtr;

/**
 * A class defining external plugins interface.
 *
 * These plugins work inside of cynara client library. They interpret
 * PolicyResult returned by cynara in terms of:
 * * cacheability - tells, whether value should be cached (for e.g. policyType like
 *                  ALLOW_ONCE should not be cached)
 * * usability - whether cache entry can still be used (for e.g. policy allowing access for
 *               given type)
 * * value - translates PolicyResult to CYNARA_API_ACCESS_ALLOWED or CYNARA_API_ACCESS_DENIED
 *
 * Plugin implementing ClientPluginInterface must implement ExternalPluginInterface.
 * Creation/destruction functions with signatures compatible to Cynara::create_t and
 * Cynara::destroy_t must be provided as factories of ClientPluginInterface.
 */
class ClientPluginInterface : public ExternalPluginInterface {
public:
    /**
     * Return entry cacheability
     */
    virtual bool isCacheable(const ClientSession &session, const PolicyResult &result) = 0;
    /**
     * Return entry usability
     */
    virtual bool isUsable(const ClientSession &session, const ClientSession &prevSession,
                          bool &updateSession, PolicyResult &result) = 0;
    /**
     * Translate PolicyResult to CYNARA_API_ACCESS_ALLOWED or CYNARA_API_ACCESS_DENIED
     */
    virtual int toResult(const ClientSession &session, PolicyResult &result) = 0;

    virtual ~ClientPluginInterface() {};
};

}

#endif // CYNARACLIENTPLUGIN_H_
