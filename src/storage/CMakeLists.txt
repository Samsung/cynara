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
# @author      Aleksander Zdyb <a.zdyb@samsung.com>
#

SET(LIB_CYNARA_STORAGE_VERSION_MAJOR 0)
SET(LIB_CYNARA_STORAGE_VERSION ${LIB_CYNARA_STORAGE_VERSION_MAJOR}.9.0)

SET(CYNARA_LIB_CYNARA_STORAGE_PATH ${CYNARA_PATH}/storage)

SET(LIB_CYNARA_STORAGE_SOURCES
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}/BucketDeserializer.cpp
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}/ChecksumStream.cpp
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}/ChecksumValidator.cpp
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}/InMemoryStorageBackend.cpp
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}/Integrity.cpp
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}/Storage.cpp
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}/StorageDeserializer.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_LIB_CYNARA_STORAGE_PATH}
    ${CYNARA_PATH}/include
    ${CYNARA_PATH}
    )

ADD_DEFINITIONS("-fvisibility=default")

ADD_LIBRARY(${TARGET_LIB_CYNARA_STORAGE} SHARED ${LIB_CYNARA_STORAGE_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_CYNARA_STORAGE}
    PROPERTIES
        SOVERSION ${LIB_CYNARA_STORAGE_VERSION_MAJOR}
        VERSION ${LIB_CYNARA_STORAGE_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_CYNARA_STORAGE}
    ${CYNARA_DEP_LIBRARIES}
    ${TARGET_CYNARA_COMMON}
    crypt
    )

INSTALL(TARGETS ${TARGET_LIB_CYNARA_STORAGE} DESTINATION ${LIB_DIR})
