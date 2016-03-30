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
# @file        src/agent/CMakeLists.txt
# @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
#

SET(LIB_CYNARA_AGENT_VERSION_MAJOR 0)
SET(LIB_CYNARA_AGENT_VERSION ${LIB_CYNARA_AGENT_VERSION_MAJOR}.12.0)

SET(CYNARA_LIB_CYNARA_AGENT_PATH ${CYNARA_PATH}/agent)

SET(LIB_CYNARA_AGENT_SOURCES
    ${CYNARA_LIB_CYNARA_AGENT_PATH}/api/agent-api.cpp
    ${CYNARA_LIB_CYNARA_AGENT_PATH}/logic/Logic.cpp
    ${CYNARA_LIB_CYNARA_AGENT_PATH}/socket/AgentSocketClient.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}/include
    ${CYNARA_LIB_CYNARA_AGENT_PATH}
    )

ADD_LIBRARY(${TARGET_LIB_CYNARA_AGENT} SHARED ${LIB_CYNARA_AGENT_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CYNARA_AGENT}
    PROPERTIES
        SOVERSION ${LIB_CYNARA_AGENT_VERSION_MAJOR}
        VERSION ${LIB_CYNARA_AGENT_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CYNARA_AGENT}
    ${TARGET_CYNARA_COMMON}
    )

INSTALL(TARGETS ${TARGET_LIB_CYNARA_AGENT} DESTINATION ${LIB_DIR})
