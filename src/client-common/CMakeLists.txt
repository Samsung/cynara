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
# @author      Zofia Abramowska <z.abramowska@samsung.com>
#

SET(LIB_CYNARA_CLIENT_COMMON_VERSION_MAJOR 0)
SET(LIB_CYNARA_CLIENT_COMMON_VERSION ${LIB_CYNARA_CLIENT_COMMON_VERSION_MAJOR}.14.2)

SET(LIB_CYNARA_COMMON_PATH ${CYNARA_PATH}/client-common)

INCLUDE_DIRECTORIES(
    ${LIB_CYNARA_COMMON_PATH}
    ${CYNARA_PATH}/include
    )

SET(LIB_CYNARA_COMMON_SOURCES
    ${LIB_CYNARA_COMMON_PATH}/cache/CapacityCache.cpp
    ${LIB_CYNARA_COMMON_PATH}/cache/MonitorCache.cpp
    )

ADD_DEFINITIONS("-fvisibility=default")

ADD_LIBRARY(${TARGET_LIB_CYNARA_COMMON} SHARED ${LIB_CYNARA_COMMON_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CYNARA_COMMON}
    PROPERTIES
        SOVERSION ${LIB_CYNARA_CLIENT_COMMON_VERSION_MAJOR}
        VERSION ${LIB_CYNARA_CLIENT_COMMON_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CYNARA_COMMON}
    ${TARGET_CYNARA_COMMON}
    )

INSTALL(TARGETS ${TARGET_LIB_CYNARA_COMMON} DESTINATION ${LIB_DIR})
