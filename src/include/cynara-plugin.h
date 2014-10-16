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
 * @file        src/include/cynara-plugin.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines cynara side external plugin interface
 */

#ifndef CYNARA_PLUGIN_H_
#define CYNARA_PLUGIN_H_

#include <memory>
#include <string>
#include <vector>

#include <types/PolicyResult.h>
#include <types/PolicyType.h>

namespace Cynara {

class ExternalPluginInterface;

/**
 * Type of function used for creating objects implementing ExternalPluginInterface.
 * Inside plugin library function with create_t signature should have symbol
 * named "create".
 */
typedef ExternalPluginInterface *(*create_t)(void);
/**
 * Type of function used for destroying objects created with "create".
 * Inside plugin library function with destroy_t signature should have symbol
 * named "destroy".
 */
typedef void (*destroy_t)(ExternalPluginInterface *);

// These typedefs will be defined in external headers
typedef std::string PluginData;
typedef std::string AgentType;
typedef std::vector<PolicyType> PolicyTypes;

/**
 * A class defining external plugins interface.
 * These plugins work inside of cynara and either can produce
 * response through check instantly or require communication
 * with given type of agent. Agent must be registered through
 * cynara-agent API.
 */

class ExternalPluginInterface {
public:
    /**
     * Enum indicating status of calling plugin method.
     */
    enum class PluginStatus {
        SUCCESS,                /** < update() finished successfully*/
        ANSWER_READY,           /** < check() returns answer immediately through argument*/
        ANSWER_NOTREADY,        /** < check() cannot return answer immediately,
                                    < communication with agent is required */
        ERROR                   /** < either check() or update() fails */
    };

    /**
     * Policy type supported by plugin.
     */
    virtual PolicyTypes getSupportedPolicyTypes(void) = 0;

    /**
     * Asks plugin, what kind of permission does client, user and privilege has.
     *
     * @param[in] client
     * @param[in] user
     * @param[in] privilege
     * @param[out] result        Immediate response (if available)
     * @param[out] requiredAgent When ANSWER_NOTREADY, required AgentType to communicate with
     * @param[out] pluginData    Additional data, that will be passed to agent
     * @return PluginStatus      In case of success - either ANSWER_READY or ANSWER_NOTREADY,
     *                           in case of error - ERROR
     */
    virtual PluginStatus check(const std::string &client, const std::string &user,
                               const std::string &privilege, PolicyResult &result,
                               AgentType &requiredAgent, PluginData &pluginData) noexcept = 0;

    /**
     * Updates response returned by agent
     * @param[in] client
     * @param[in] user
     * @param[in] privilege
     * @param[in] agentData   Additional data, passed from agent
     * @param[out] result     Response interpreted from agent
     * @return PluginStatus   In case of success - SUCCESS, in case of error - ERROR
     */
    virtual PluginStatus update(const std::string &client, const std::string &user,
                                const std::string &privilege, const PluginData &agentData,
                                PolicyResult &result) noexcept = 0;

    virtual ~ExternalPluginInterface() {};

};

} // namespace Cynara

#endif /* CYNARA_PLUGIN_H_ */
