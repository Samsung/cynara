/*
 *  Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/client-common/configuration/Configuration.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains client configuration class.
 */

#ifndef SRC_CLIENT_COMMON_CONFIGURATION_CONFIGURATION_H_
#define SRC_CLIENT_COMMON_CONFIGURATION_CONFIGURATION_H_

#include <cstddef>
#include <memory>

#include <cache/CapacityCache.h>

namespace Cynara {

class Configuration;
typedef std::unique_ptr<Configuration> ConfigurationUniquePtr;

class Configuration {
public:
    Configuration() : m_cacheSize(CapacityCache::CACHE_DEFAULT_CAPACITY) {};

    void setCacheSize(std::size_t size) {
        m_cacheSize = size;
    }
    std::size_t getCacheSize(void) const {
        return m_cacheSize;
    }

    bool monitoringEnabled(void) const {
        return m_monitoringEnabled;
    }

    ~Configuration() {}
private:
    std::size_t m_cacheSize;
#if defined(MONITORING)
    static constexpr bool m_monitoringEnabled = true;
#else
    static constexpr bool m_monitoringEnabled = false;
#endif
};

} /* namespace Cynara */

#endif /* SRC_CLIENT_COMMON_CONFIGURATION_CONFIGURATION_H_ */
