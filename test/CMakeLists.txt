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
# @brief       Temporary cmake for tests
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
PROJECT("cynara-tests")

ADD_SUBDIRECTORY(${GMOCK_DIR} gmock)

SET(CYNARA_SOURCES_FOR_TESTS
    ${CYNARA_SRC}/service/storage/Storage.cpp
    ${CYNARA_SRC}/common/types/PolicyBucket.cpp
    ${CYNARA_SRC}/service/storage/InMemoryStorageBackend.cpp
    ${CYNARA_SRC}/service/storage/StorageSerializer.cpp
    ${CYNARA_SRC}/common/types/PolicyKey.cpp
)

SET(CYNARA_TESTS_SOURCES
    types/policykey.cpp
    storage/storage/policies.cpp
    storage/storage/check.cpp
    storage/storage/buckets.cpp
    storage/inmemorystoragebackend/inmemorystoragebackend.cpp
    storage/inmemorystoragebackend/search.cpp
    storage/inmemorystoragebackend/buckets.cpp
    storage/serializer/dump.cpp
    helpers.cpp
)

INCLUDE_DIRECTORIES(
    ${GMOCK_DIR}/include
    ${GMOCK_DIR}/gtest/include
    ${CYNARA_SRC}
    ${CYNARA_SRC}/common
    ${CYNARA_SRC}/include
)

ADD_EXECUTABLE(cynara-tests
    ${CYNARA_SOURCES_FOR_TESTS}
    ${CYNARA_TESTS_SOURCES}
)

SET(CMAKE_CXX_FLAGS "-std=c++0x -O0 -g -gdwarf-2")

FIND_PACKAGE(Threads)

TARGET_LINK_LIBRARIES(cynara-tests
    ${CMAKE_THREAD_LIBS_INIT}
    gmock gtest_main
)
