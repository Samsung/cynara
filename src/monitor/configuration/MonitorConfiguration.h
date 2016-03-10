/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/monitor/configuration/Configuration.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains monitor configuration class.
 */

#ifndef SRC_MONITOR_CONFIGURATION_MONITORCONFIGURATION_H_
#define SRC_MONITOR_CONFIGURATION_MONITORCONFIGURATION_H_

#include <cstddef>
#include <memory>

namespace Cynara {

class MonitorConfiguration;
typedef std::unique_ptr<MonitorConfiguration> MonitorConfigurationUniquePtr;

class MonitorConfiguration {
public:
    // TODO: Where to define the default value?
    MonitorConfiguration() : m_bufferSize(100) {};
    ~MonitorConfiguration() = default;

    void setBufferSize(std::size_t size) {
        m_bufferSize = size;
    }
    std::size_t getBufferSize(void) const {
        return m_bufferSize;
    }
private:
    std::size_t m_bufferSize;
};

} /* namespace Cynara */

#endif /* SRC_MONITOR_CONFIGURATION_MONITORCONFIGURATION_H_ */
