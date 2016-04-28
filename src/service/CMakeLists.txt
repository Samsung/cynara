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

SET(CYNARA_SERVICE_PATH ${CYNARA_PATH}/service)

IF (BUILD_COMMONS)
    SET(CYNARA_COMMON_AND_STORAGE_LIB
        ${TARGET_CYNARA_COMMON}
        ${TARGET_LIB_CYNARA_STORAGE})
ELSE (BUILD_COMMONS)
    PKG_CHECK_MODULES(LIB_COMMON_DEP
        REQUIRED
        cynara-commons
        )
    SET(CYNARA_COMMON_AND_STORAGE_LIB ${LIB_COMMON_DEP_LIBRARIES})
ENDIF (BUILD_COMMONS)

SET(CYNARA_SOURCES
    ${CYNARA_SERVICE_PATH}/agent/AgentManager.cpp
    ${CYNARA_SERVICE_PATH}/agent/AgentTalker.cpp
    ${CYNARA_SERVICE_PATH}/logic/Logic.cpp
    ${CYNARA_SERVICE_PATH}/main/CmdlineParser.cpp
    ${CYNARA_SERVICE_PATH}/main/Cynara.cpp
    ${CYNARA_SERVICE_PATH}/main/main.cpp
    ${CYNARA_SERVICE_PATH}/monitor/EntriesQueue.cpp
    ${CYNARA_SERVICE_PATH}/monitor/EntriesManager.cpp
    ${CYNARA_SERVICE_PATH}/request/CheckRequestManager.cpp
    ${CYNARA_SERVICE_PATH}/sockets/Descriptor.cpp
    ${CYNARA_SERVICE_PATH}/sockets/SocketManager.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_SERVICE_PATH}
    ${CYNARA_PATH}
    ${CYNARA_PATH}/include
    )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")

ADD_EXECUTABLE(${TARGET_CYNARA} ${CYNARA_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_CYNARA}
    ${CYNARA_COMMON_AND_STORAGE_LIB}
    ${CYNARA_DEP_LIBRARIES}
    "-pie"
    )

INSTALL(TARGETS ${TARGET_CYNARA} DESTINATION ${BIN_DIR})

INSTALL(DIRECTORY DESTINATION ${LIB_DIR}/${PROJECT_NAME}/plugin/service)
