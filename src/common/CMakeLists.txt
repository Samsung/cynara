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
# @file        CMakeLists.txt
# @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
#

SET(CYNARA_COMMON_VERSION_MAJOR 0)
SET(CYNARA_COMMON_VERSION ${CYNARA_COMMON_VERSION_MAJOR}.0.1)

SET(COMMON_PATH ${CYNARA_PATH}/common)

SET(COMMON_SOURCES
    ${COMMON_PATH}/common.cpp
    )

ADD_LIBRARY(${TARGET_CYNARA_COMMON} SHARED ${COMMON_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_CYNARA_COMMON}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fPIC -fvisibility=default"
        SOVERSION ${CYNARA_COMMON_VERSION_MAJOR}
        VERSION ${CYNARA_COMMON_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_CYNARA_COMMON}
    ${CYNARA_DEP_LIBRARIES}
    )

INSTALL(TARGETS ${TARGET_CYNARA_COMMON} DESTINATION ${LIB_INSTALL_DIR})
