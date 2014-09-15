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
/**
 * @file        src/common/sockets/SocketPath.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       Header for common socket paths
 */

#ifndef  SRC_COMMON_SOCKETS_SOCKETPATH_H_
#define  SRC_COMMON_SOCKETS_SOCKETPATH_H_

#include <string>

namespace Cynara {
namespace SocketPath {

extern const std::string client;
extern const std::string admin;

} // namespace SocketPath
} // namespace Cynara

#endif // SRC_COMMON_SOCKETS_SOCKETPATH_H_
