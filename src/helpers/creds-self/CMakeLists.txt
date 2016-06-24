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
# @file        src/helpers/creds-self/CMakeLists.txt
# @author      Zofia Abramowska <z.abramowska@samsung.com>
#

SET(LIB_CREDS_SELF_VERSION_MAJOR 0)
SET(LIB_CREDS_SELF_VERSION ${LIB_CREDS_SELF_VERSION_MAJOR}.14.1)

SET(LIB_CREDS_SELF_PATH ${CYNARA_PATH}/helpers/creds-self)

SET(LIB_CREDS_SELF_SOURCES
    ${LIB_CREDS_SELF_PATH}/creds-self.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}/include
    ${LIB_CREDS_SELF_PATH}
    )

ADD_LIBRARY(${TARGET_LIB_CREDS_SELF} SHARED ${LIB_CREDS_SELF_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CREDS_SELF}
    PROPERTIES
        SOVERSION ${LIB_CREDS_SELF_VERSION_MAJOR}
        VERSION ${LIB_CREDS_SELF_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CREDS_SELF}
    ${TARGET_LIB_CREDS_COMMONS}
    )

INSTALL(TARGETS ${TARGET_LIB_CREDS_SELF} DESTINATION ${LIB_DIR})
