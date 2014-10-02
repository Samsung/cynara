# Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        pkgconfig/cynara-agent/CMakeLists.txt
# @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
#

CONFIGURE_FILE(cynara-agent.pc.in cynara-agent.pc @ONLY)

INSTALL(FILES
    ${CMAKE_BINARY_DIR}/pkgconfig/cynara-agent/cynara-agent.pc
    DESTINATION
    ${LIB_INSTALL_DIR}/pkgconfig
    )
