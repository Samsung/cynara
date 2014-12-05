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
# @author      Aleksander Zdyb <a.zdyb@samsung.com>
# @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
# @brief       Cmake for tests
#
PKG_CHECK_MODULES(PKGS REQUIRED gmock)

ADD_DEFINITIONS("-DCYNARA_NO_LOGS")
SET(CYNARA_SRC ${PROJECT_SOURCE_DIR}/src)

SET(CYNARA_SOURCES_FOR_TESTS
    ${CYNARA_SRC}/common/containers/BinaryQueue.cpp
    ${CYNARA_SRC}/common/protocol/ProtocolAdmin.cpp
    ${CYNARA_SRC}/common/protocol/ProtocolFrame.cpp
    ${CYNARA_SRC}/common/protocol/ProtocolFrameHeader.cpp
    ${CYNARA_SRC}/common/protocol/ProtocolFrameSerializer.cpp
    ${CYNARA_SRC}/common/request/AdminCheckRequest.cpp
    ${CYNARA_SRC}/common/request/InsertOrUpdateBucketRequest.cpp
    ${CYNARA_SRC}/common/request/ListRequest.cpp
    ${CYNARA_SRC}/common/request/RemoveBucketRequest.cpp
    ${CYNARA_SRC}/common/request/RequestTaker.cpp
    ${CYNARA_SRC}/common/request/SetPoliciesRequest.cpp
    ${CYNARA_SRC}/common/response/AdminCheckResponse.cpp
    ${CYNARA_SRC}/common/response/CheckResponse.cpp
    ${CYNARA_SRC}/common/response/CodeResponse.cpp
    ${CYNARA_SRC}/common/response/ListResponse.cpp
    ${CYNARA_SRC}/common/response/ResponseTaker.cpp
    ${CYNARA_SRC}/common/types/PolicyBucket.cpp
    ${CYNARA_SRC}/common/types/PolicyKey.cpp
    ${CYNARA_SRC}/common/types/PolicyKeyHelpers.cpp
    ${CYNARA_SRC}/common/types/PolicyResult.cpp
    ${CYNARA_SRC}/common/types/PolicyType.cpp
    ${CYNARA_SRC}/helpers/creds-commons/CredsCommonsInner.cpp
    ${CYNARA_SRC}/helpers/creds-commons/creds-commons.cpp
    ${CYNARA_SRC}/storage/BucketDeserializer.cpp
    ${CYNARA_SRC}/storage/InMemoryStorageBackend.cpp
    ${CYNARA_SRC}/storage/Integrity.cpp
    ${CYNARA_SRC}/storage/Storage.cpp
    ${CYNARA_SRC}/storage/StorageDeserializer.cpp
    ${CYNARA_SRC}/storage/StorageSerializer.cpp
)

SET(CYNARA_TESTS_SOURCES
    TestEventListenerProxy.cpp
    common/exceptions/bucketrecordcorrupted.cpp
    common/protocols/admin/listrequest.cpp
    common/protocols/admin/listresponse.cpp
    common/types/policybucket.cpp
    credsCommons/parser/Parser.cpp
    helpers.cpp
    storage/performance/bucket.cpp
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
    tests.cpp
    types/policykey.cpp
)

INCLUDE_DIRECTORIES(
    ${PKGS_INCLUDE_DIRS}
    ${CYNARA_SRC}/common
    ${CYNARA_SRC}/include
    ${CYNARA_SRC}
    credsCommons/parser
    common/protocols
)

ADD_EXECUTABLE(${TARGET_CYNARA_TESTS}
    ${CYNARA_SOURCES_FOR_TESTS}
    ${CYNARA_TESTS_SOURCES}
)

TARGET_LINK_LIBRARIES(${TARGET_CYNARA_TESTS}
    ${PKGS_LDFLAGS} ${PKGS_LIBRARIES}
)
INSTALL(TARGETS ${TARGET_CYNARA_TESTS} DESTINATION ${BIN_INSTALL_DIR})
