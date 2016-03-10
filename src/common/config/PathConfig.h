/*
 * Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/config/PathConfig.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file specifies PathConfig namespace containing default cynara paths
 */

#ifndef SRC_COMMON_CONFIG_PATHCONFIG_H_
#define SRC_COMMON_CONFIG_PATHCONFIG_H_

#include <string>

namespace Cynara {
namespace PathConfig {

extern const std::string statePath;
extern const std::string libraryPath;
extern const std::string clientPath;
extern const std::string confPath;
extern const std::string testsPath;

namespace SocketPath {
extern const std::string client;
extern const std::string admin;
extern const std::string agent;
extern const std::string monitorGet;
} // namespace SocketPath

namespace StoragePath {
extern const std::string dbDir;
extern const std::string lockFile;
extern const std::string indexFilename;
extern const std::string guardFilename;
extern const std::string checksumFilename;
extern const std::string backupFilenameSuffix;
extern const std::string bucketFilenamePrefix;
extern const char fieldSeparator;
extern const char recordSeparator;
} // namespace StoragePath

namespace PluginPath {
extern const std::string clientDir;
extern const std::string serviceDir;
} // namespace PluginPath

} // namespace PathConfig
} // namespace Cynara

#endif /* SRC_COMMON_CONFIG_PATHCONFIG_H_ */
