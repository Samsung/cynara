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
# @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
# @brief       Cmake for tests
#

############################# Check minimum CMake version #####################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
PROJECT("cynara-tests")

############################# cmake packages ##################################

INCLUDE(FindPkgConfig)

############################# compiler flags ##################################

SET(CMAKE_C_FLAGS_DEBUG        "-g -O0 -ggdb")
SET(CMAKE_CXX_FLAGS_DEBUG      "-g -std=c++0x -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_C_FLAGS_RELEASE      "-g -O2")
SET(CMAKE_CXX_FLAGS_RELEASE    "-g -std=c++0x -O2")

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings
ADD_DEFINITIONS("-DCYNARA_NO_LOGS")             # Disable building logs

MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

SET(TARGET_CYNARA_TESTS "cynara-tests")

PKG_CHECK_MODULES(PKGS REQUIRED gmock_main)

SET(CYNARA_SRC ${PROJECT_SOURCE_DIR}/../src)

SET(CYNARA_SOURCES_FOR_TESTS
    ${CYNARA_SRC}/service/storage/Storage.cpp
    ${CYNARA_SRC}/common/types/PolicyBucket.cpp
    ${CYNARA_SRC}/common/types/PolicyKeyHelpers.cpp
    ${CYNARA_SRC}/service/storage/InMemoryStorageBackend.cpp
    ${CYNARA_SRC}/service/storage/BucketDeserializer.cpp
    ${CYNARA_SRC}/service/storage/StorageDeserializer.cpp
    ${CYNARA_SRC}/service/storage/StorageSerializer.cpp
    ${CYNARA_SRC}/common/types/PolicyKey.cpp
    ${CYNARA_SRC}/common/types/PolicyType.cpp
)

SET(CYNARA_TESTS_SOURCES
    common/exceptions/bucketrecordcorrupted.cpp
    types/policykey.cpp
    storage/storage/policies.cpp
    storage/storage/check.cpp
    storage/storage/buckets.cpp
    storage/inmemorystoragebackend/inmemorystoragebackend.cpp
    storage/inmemorystoragebackend/search.cpp
    storage/inmemorystoragebackend/buckets.cpp
    storage/serializer/bucket_load.cpp
    storage/serializer/deserialize.cpp
    storage/serializer/dump.cpp
    storage/serializer/dump_load.cpp
    storage/serializer/serialize.cpp
    common/types/policybucket.cpp
    helpers.cpp
)

INCLUDE_DIRECTORIES(
    ${PKGS_INCLUDE_DIRS}
    ${CYNARA_SRC}/common
    ${CYNARA_SRC}/include
    ${CYNARA_SRC}/service
)

ADD_EXECUTABLE(${TARGET_CYNARA_TESTS}
    ${CYNARA_SOURCES_FOR_TESTS}
    ${CYNARA_TESTS_SOURCES}
)

TARGET_LINK_LIBRARIES(${TARGET_CYNARA_TESTS}
    ${PKGS_LDFLAGS} ${PKGS_LIBRARIES}
)
INSTALL(TARGETS ${TARGET_CYNARA_TESTS} DESTINATION bin)
