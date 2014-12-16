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
 * @file        src/common/plugin/ExternalPluginInterface.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines cynara side external plugin interface
 */

#ifndef SRC_COMMON_PLUGIN_EXTERNALPLUGININTERFACE_H_
#define SRC_COMMON_PLUGIN_EXTERNALPLUGININTERFACE_H_

#include <vector>

#include <types/PolicyDescription.h>

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

class ExternalPluginInterface {
public:
    /**
     * Policy types supported by plugin.
     */
    virtual const std::vector<PolicyDescription> &getSupportedPolicyDescr(void) = 0;

    /**
     * Informs, that every data stored based on previously given input
     * should be invalidated.
     */
    virtual void invalidate(void) = 0;

    virtual ~ExternalPluginInterface() {}
};

} // namespace Cynara

#endif /* SRC_COMMON_PLUGIN_EXTERNALPLUGININTERFACE_H_ */
