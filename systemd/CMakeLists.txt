# Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
# @file        CMakeLists.txt
# @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
#

SET(CYNARA_ADMIN_SOCKET_GROUP
    "security_fw"
    CACHE STRING
    "Group to apply on administrative sockets")


CONFIGURE_FILE(cynara.socket.in             cynara.socket @ONLY)
CONFIGURE_FILE(cynara-admin.socket.in       cynara-admin.socket @ONLY)
CONFIGURE_FILE(cynara-agent.socket.in       cynara-agent.socket @ONLY)
CONFIGURE_FILE(cynara-monitor-get.socket.in cynara-monitor-get.socket @ONLY)

INSTALL(FILES
    ${CMAKE_SOURCE_DIR}/systemd/cynara.service
    ${CMAKE_SOURCE_DIR}/systemd/cynara.target
    ${CMAKE_BINARY_DIR}/systemd/cynara.socket
    ${CMAKE_BINARY_DIR}/systemd/cynara-admin.socket
    ${CMAKE_BINARY_DIR}/systemd/cynara-agent.socket
    ${CMAKE_BINARY_DIR}/systemd/cynara-monitor-get.socket
    DESTINATION
    ${SYSTEMD_UNIT_DIR}
)

INSTALL(DIRECTORY
    ${CMAKE_SOURCE_DIR}/systemd/sockets.target.wants
    DESTINATION
    ${SYSTEMD_UNIT_DIR}
)


