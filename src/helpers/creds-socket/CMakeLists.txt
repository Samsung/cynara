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
# @author      Aleksander Zdyb <a.zdyb@samsung.com>
# @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
# @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
#

SET(LIB_CREDS_SOCKET_VERSION_MAJOR 0)
SET(LIB_CREDS_SOCKET_VERSION ${LIB_CREDS_SOCKET_VERSION_MAJOR}.14.1)

SET(LIB_CREDS_SOCKET_PATH ${CYNARA_PATH}/helpers/creds-socket)

SET(LIB_CREDS_SOCKET_SOURCES
    ${LIB_CREDS_SOCKET_PATH}/creds-socket.cpp
    ${LIB_CREDS_SOCKET_PATH}/creds-socket-inner.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}/include
    ${LIB_CREDS_SOCKET_PATH}
    )

ADD_LIBRARY(${TARGET_LIB_CREDS_SOCKET} SHARED ${LIB_CREDS_SOCKET_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CREDS_SOCKET}
    PROPERTIES
        SOVERSION ${LIB_CREDS_SOCKET_VERSION_MAJOR}
        VERSION ${LIB_CREDS_SOCKET_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CREDS_SOCKET}
    ${TARGET_LIB_CREDS_COMMONS}
    )

INSTALL(TARGETS ${TARGET_LIB_CREDS_SOCKET} DESTINATION ${LIB_DIR})
