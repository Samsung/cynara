# Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
# @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
#

SET(LIB_CYNARA_ASYNC_VERSION_MAJOR 0)
SET(LIB_CYNARA_ASYNC_VERSION ${LIB_CYNARA_ASYNC_VERSION_MAJOR}.6.1)

SET(CYNARA_LIB_CYNARA_ASYNC_PATH ${CYNARA_PATH}/client-async)

INCLUDE_DIRECTORIES(
    ${CYNARA_LIB_CYNARA_ASYNC_PATH}
    ${CYNARA_PATH}/client-common
    ${CYNARA_PATH}/common
    ${CYNARA_PATH}/include
    )

SET(LIB_CYNARA_ASYNC_SOURCES
    ${CYNARA_LIB_CYNARA_ASYNC_PATH}/api/client-async-api.cpp
    ${CYNARA_LIB_CYNARA_ASYNC_PATH}/callback/ResponseCallback.cpp
    ${CYNARA_LIB_CYNARA_ASYNC_PATH}/callback/StatusCallback.cpp
    ${CYNARA_LIB_CYNARA_ASYNC_PATH}/logic/Logic.cpp
    ${CYNARA_LIB_CYNARA_ASYNC_PATH}/sequence/SequenceContainer.cpp
    ${CYNARA_LIB_CYNARA_ASYNC_PATH}/sockets/SocketClientAsync.cpp
    )

ADD_LIBRARY(${TARGET_LIB_CYNARA_ASYNC} SHARED ${LIB_CYNARA_ASYNC_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CYNARA_ASYNC}
    PROPERTIES
        SOVERSION ${LIB_CYNARA_ASYNC_VERSION_MAJOR}
        VERSION ${LIB_CYNARA_ASYNC_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CYNARA_ASYNC}
    ${TARGET_LIB_CYNARA_COMMON}
    )

INSTALL(TARGETS ${TARGET_LIB_CYNARA_ASYNC} DESTINATION ${LIB_INSTALL_DIR})
