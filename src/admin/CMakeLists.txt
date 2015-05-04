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
# @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
#

SET(LIB_CYNARA_ADMIN_VERSION_MAJOR 0)
SET(LIB_CYNARA_ADMIN_VERSION ${LIB_CYNARA_ADMIN_VERSION_MAJOR}.8.0)

IF (DB_FILES_SMACK_LABEL)
   SET(SMACK "smack")
   SET(LIBSMACK "libsmack")
   ADD_DEFINITIONS("-DDB_FILES_SMACK_LABEL=\"${DB_FILES_SMACK_LABEL}\"")
ENDIF (DB_FILES_SMACK_LABEL)

PKG_CHECK_MODULES(CYNARA_ADMIN_API_DEP
    REQUIRED
    ${LIBSMACK}
    )

SET(CYNARA_LIB_CYNARA_ADMIN_PATH ${CYNARA_PATH}/admin)

SET(LIB_CYNARA_ADMIN_SOURCES
    ${CYNARA_LIB_CYNARA_ADMIN_PATH}/api/admin-api.cpp
    ${CYNARA_LIB_CYNARA_ADMIN_PATH}/logic/Logic.cpp
    ${CYNARA_LIB_CYNARA_ADMIN_PATH}/logic/OfflineLogic.cpp
    ${CYNARA_LIB_CYNARA_ADMIN_PATH}/logic/OnlineLogic.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}
    ${CYNARA_PATH}/include
    ${CYNARA_LIB_CYNARA_ADMIN_PATH}
    )

ADD_LIBRARY(${TARGET_LIB_CYNARA_ADMIN} SHARED ${LIB_CYNARA_ADMIN_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CYNARA_ADMIN}
    PROPERTIES
        SOVERSION ${LIB_CYNARA_ADMIN_VERSION_MAJOR}
        VERSION ${LIB_CYNARA_ADMIN_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CYNARA_ADMIN}
    ${CYNARA_DEP_LIBRARIES}
    ${TARGET_CYNARA_COMMON}
    ${TARGET_LIB_CYNARA_STORAGE}
    ${SMACK}
    )

INSTALL(TARGETS ${TARGET_LIB_CYNARA_ADMIN} DESTINATION ${LIB_DIR})
