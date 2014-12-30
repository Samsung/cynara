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
 * @file        src/common/plugin/PluginManager.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Definition of PluginManager class
 */

#define _BSD_SOURCE_

#include <cinttypes>
#include <cstdlib>
#include <dirent.h>
#include <dlfcn.h>
#include <functional>

#include <log/log.h>

#include "PluginManager.h"


namespace {
    int pluginFilter(const struct dirent *ent) {
#ifdef _DIRENT_HAVE_D_TYPE
        if (ent->d_type != DT_REG) {
            return 0;
        }
#endif
        if (ent->d_name[0] == '.') {
            return 0;
        }
        return 1;
    }
}

namespace Cynara {

PluginManager::PluginManager(const std::string &pluginDir) : m_dir(pluginDir) {
}

PluginManager::~PluginManager(void) {
    // We have to be sure, that external objects will be destroyed
    // before handles to libraries are closed.
    for (auto &plugin : m_plugins) {
        plugin.second.reset();
    }
}

ExternalPluginPtr PluginManager::getPlugin(PolicyType pType) {
    return m_plugins[pType];
}

std::vector<PolicyDescription> PluginManager::getPolicyDescriptions(void) const {
    std::vector<PolicyDescription> descriptions;
    descriptions.reserve(m_plugins.size());
    for (auto &plugin : m_plugins) {
        descriptions.push_back(plugin.first);
    }
    return descriptions;
}

void PluginManager::invalidateAll(void) {
    for (auto &plugin : m_plugins) {
        plugin.second->invalidate();
    }
}

void PluginManager::loadPlugins(void) {
    struct dirent **nameList = NULL;
    int fileAmount = scandir(m_dir.c_str(), &nameList, pluginFilter, alphasort);

    if (fileAmount < 0) {
        auto error = strerror(errno);
        LOGE("Couldn't scan for plugins in <%s> : <%s>", m_dir.c_str(), error);
        return;
    }

    std::unique_ptr<dirent*, std::function<void(dirent**)>> direntPtr(nameList,
            [fileAmount](dirent** dirs) {
                for (int i = 0; i < fileAmount; i++) {
                    free(dirs[i]);
                }
                free(dirs);
            });
    for (int i = 0; i < fileAmount; i++) {
        openPlugin(m_dir + nameList[i]->d_name);
    }
}

void PluginManager::openPlugin(const std::string &path) {
    LOGD("Loading plugin: <%s>", path.c_str());

    void *handle = dlopen(path.c_str(), RTLD_LAZY);

    if (!handle) {
        LOGW("File could not be dlopened <%s> : <%s>", path.c_str(), dlerror());
        return;
    }
    PluginLibPtr handlePtr(handle, std::ptr_fun(dlclose));

    //Flush any previous errors
    dlerror();
    create_t creator = reinterpret_cast<create_t>(dlsym(handle, "create"));

    char *error;
    if ((error = dlerror()) != NULL) {
        LOGE("Couldn't resolve symbol <create> from lib <%s> : <%s>", path.c_str(), error);
        return;
    }

    destroy_t destroyer = reinterpret_cast<destroy_t>(dlsym(handle, "destroy"));
    if ((error = dlerror()) != NULL) {
        LOGE("Couldn't resolve symbol <destroy> from lib <%s> : <%s>", path.c_str(), error);
        return;
    }

    ExternalPluginPtr pluginPtr(creator(), destroyer);

    if (!pluginPtr) {
        LOGE("Couldn't create plugin for <%s>", path.c_str());
        return;
    }

    auto policies = pluginPtr->getSupportedPolicyDescr();
    if (policies.empty()) {
        LOGE("Plugin <%s> does not support any type!", path.c_str());
        return;
    }
    for (auto &desc : policies) {
        if (!m_plugins.insert(std::make_pair(desc, pluginPtr)).second) {
            LOGW("Policy: type [%" PRIu16 "] name <%s> was already supported.",
                 desc.type, desc.name.c_str());
        } else {
            LOGD("Supported policy: type [%" PRIu16 "] name <%s>", desc.type, desc.name.c_str());
        }
    }

    m_pluginLibs.push_back(std::move(handlePtr));
}

} // namespace Cynara

