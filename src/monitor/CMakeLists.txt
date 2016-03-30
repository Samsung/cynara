# Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
# @file        src/monitor/CMakeLists.txt
# @author      Aleksander Zdyb <a.zdyb@samsung.com>
#

SET(LIB_CYNARA_MONITOR_VERSION_MAJOR 0)
SET(LIB_CYNARA_MONITOR_VERSION ${LIB_CYNARA_MONITOR_VERSION_MAJOR}.12.0)

SET(LIB_CYNARA_MONITOR_PATH ${CYNARA_PATH}/monitor)

INCLUDE_DIRECTORIES(
    ${LIB_CYNARA_MONITOR_PATH}
    ${CYNARA_PATH}/include
    )

SET(LIB_CYNARA_MONITOR_SOURCES
    ${LIB_CYNARA_MONITOR_PATH}/api/monitor-api.cpp
    ${LIB_CYNARA_MONITOR_PATH}/logic/Logic.cpp
    )

ADD_LIBRARY(${TARGET_LIB_CYNARA_MONITOR} SHARED ${LIB_CYNARA_MONITOR_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CYNARA_MONITOR}
    PROPERTIES
        SOVERSION ${LIB_CYNARA_MONITOR_VERSION_MAJOR}
        VERSION ${LIB_CYNARA_MONITOR_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CYNARA_MONITOR}
    ${TARGET_LIB_CYNARA_COMMON}
    )

INSTALL(TARGETS ${TARGET_LIB_CYNARA_MONITOR} DESTINATION ${LIB_DIR})

