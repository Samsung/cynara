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

IF (BUILD_COMMONS)
    INSTALL(FILES
        ${CYNARA_PATH}/include/cynara-admin.h
        ${CYNARA_PATH}/include/cynara-admin-types.h
        ${CYNARA_PATH}/include/cynara-agent.h
        ${CYNARA_PATH}/include/cynara-client.h
        ${CYNARA_PATH}/include/cynara-client-async.h
        ${CYNARA_PATH}/include/cynara-client-plugin.h
        ${CYNARA_PATH}/include/cynara-creds-commons.h
        ${CYNARA_PATH}/include/cynara-creds-self.h
        ${CYNARA_PATH}/include/cynara-creds-socket.h
        ${CYNARA_PATH}/include/cynara-error.h
        ${CYNARA_PATH}/include/cynara-limits.h
        ${CYNARA_PATH}/include/cynara-plugin.h
        ${CYNARA_PATH}/include/cynara-policy-types.h
        ${CYNARA_PATH}/include/cynara-session.h
        ${CYNARA_PATH}/include/cynara-monitor.h
        DESTINATION ${INCLUDE_DIR}/cynara
        )
ENDIF (BUILD_COMMONS)

IF (BUILD_DBUS)
    INSTALL(FILES
        ${CYNARA_PATH}/include/cynara-creds-dbus.h
        ${CYNARA_PATH}/include/cynara-creds-gdbus.h
        DESTINATION ${INCLUDE_DIR}/cynara
        )
ENDIF (BUILD_DBUS)
