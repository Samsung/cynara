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
 * @file        src/common/plugin/PluginManager.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Declaration of PluginManager class
 */

#ifndef SRC_COMMON_PLUGIN_PLUGINMANAGER_H_
#define SRC_COMMON_PLUGIN_PLUGINMANAGER_H_

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <plugin/ExternalPluginInterface.h>
#include <types/PolicyDescription.h>

namespace Cynara {
typedef std::shared_ptr<ExternalPluginInterface> ExternalPluginPtr;

class PluginManager {
public:
    PluginManager(const std::string &pluginDir);
    ~PluginManager();

    void loadPlugins(void);

    ExternalPluginPtr getPlugin(PolicyType pType);
    std::vector<PolicyDescription> getPolicyDescriptions(void) const;
    void invalidateAll(void);

private:
    typedef std::unique_ptr<void, std::function<void (void*)>> PluginLibPtr;
    typedef std::list<PluginLibPtr> PluginLibs;

    std::string m_dir;
    std::map<PolicyDescription, ExternalPluginPtr> m_plugins;
    PluginLibs m_pluginLibs;

    void openPlugin(const std::string &path);
};

} // namespace Cynara

#endif /* SRC_COMMON_PLUGIN_PLUGINMANAGER_H_ */
