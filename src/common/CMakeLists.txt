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

SET(CYNARA_COMMON_VERSION_MAJOR 0)
SET(CYNARA_COMMON_VERSION ${CYNARA_COMMON_VERSION_MAJOR}.6.1)

SET(COMMON_PATH ${CYNARA_PATH}/common)

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}/include
    )

SET(COMMON_SOURCES
    ${COMMON_PATH}/config/PathConfig.cpp
    ${COMMON_PATH}/containers/BinaryQueue.cpp
    ${COMMON_PATH}/error/api.cpp
    ${COMMON_PATH}/lock/FileLock.cpp
    ${COMMON_PATH}/log/AuditLog.cpp
    ${COMMON_PATH}/log/log.cpp
    ${COMMON_PATH}/plugin/PluginManager.cpp
    ${COMMON_PATH}/protocol/ProtocolAdmin.cpp
    ${COMMON_PATH}/protocol/ProtocolAgent.cpp
    ${COMMON_PATH}/protocol/ProtocolClient.cpp
    ${COMMON_PATH}/protocol/ProtocolFrame.cpp
    ${COMMON_PATH}/protocol/ProtocolFrameHeader.cpp
    ${COMMON_PATH}/protocol/ProtocolFrameSerializer.cpp
    ${COMMON_PATH}/protocol/ProtocolSerialization.cpp
    ${COMMON_PATH}/protocol/ProtocolSignal.cpp
    ${COMMON_PATH}/request/AdminCheckRequest.cpp
    ${COMMON_PATH}/request/AgentActionRequest.cpp
    ${COMMON_PATH}/request/AgentRegisterRequest.cpp
    ${COMMON_PATH}/request/CancelRequest.cpp
    ${COMMON_PATH}/request/CheckRequest.cpp
    ${COMMON_PATH}/request/DescriptionListRequest.cpp
    ${COMMON_PATH}/request/EraseRequest.cpp
    ${COMMON_PATH}/request/InsertOrUpdateBucketRequest.cpp
    ${COMMON_PATH}/request/ListRequest.cpp
    ${COMMON_PATH}/request/RemoveBucketRequest.cpp
    ${COMMON_PATH}/request/RequestTaker.cpp
    ${COMMON_PATH}/request/SetPoliciesRequest.cpp
    ${COMMON_PATH}/request/SignalRequest.cpp
    ${COMMON_PATH}/request/SimpleCheckRequest.cpp
    ${COMMON_PATH}/response/AdminCheckResponse.cpp
    ${COMMON_PATH}/response/AgentActionResponse.cpp
    ${COMMON_PATH}/response/AgentRegisterResponse.cpp
    ${COMMON_PATH}/response/CancelResponse.cpp
    ${COMMON_PATH}/response/CheckResponse.cpp
    ${COMMON_PATH}/response/CodeResponse.cpp
    ${COMMON_PATH}/response/DescriptionListResponse.cpp
    ${COMMON_PATH}/response/ListResponse.cpp
    ${COMMON_PATH}/response/ResponseTaker.cpp
    ${COMMON_PATH}/response/SimpleCheckResponse.cpp
    ${COMMON_PATH}/sockets/Socket.cpp
    ${COMMON_PATH}/sockets/SocketClient.cpp
    ${COMMON_PATH}/types/PolicyBucket.cpp
    ${COMMON_PATH}/types/PolicyDescription.cpp
    ${COMMON_PATH}/types/PolicyKey.cpp
    ${COMMON_PATH}/types/PolicyKeyHelpers.cpp
    ${COMMON_PATH}/types/PolicyResult.cpp
    ${COMMON_PATH}/types/PolicyType.cpp
    )

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
SET(COMMON_SOURCES ${COMMON_SOURCES}
    ${COMMON_PATH}/log/Backtrace.cpp
    )
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

ADD_DEFINITIONS("-fvisibility=default")

ADD_LIBRARY(${TARGET_CYNARA_COMMON} SHARED ${COMMON_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_CYNARA_COMMON}
    PROPERTIES
        SOVERSION ${CYNARA_COMMON_VERSION_MAJOR}
        VERSION ${CYNARA_COMMON_VERSION}
    )

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
SET(CYNARA_DBG_LIBRARIES
    ${CYNARA_DEP_LIBRARIES}
    dw
    )
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

TARGET_LINK_LIBRARIES(${TARGET_CYNARA_COMMON}
    ${CYNARA_DEP_LIBRARIES}
    ${CYNARA_DBG_LIBRARIES}
    dl
    )

INSTALL(TARGETS ${TARGET_CYNARA_COMMON} DESTINATION ${LIB_INSTALL_DIR})
INSTALL(FILES
    ${COMMON_PATH}/log/log.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/cynara/log
    )
INSTALL(FILES
    ${COMMON_PATH}/types/ClientSession.h
    ${COMMON_PATH}/types/PolicyDescription.h
    ${COMMON_PATH}/types/PolicyResult.h
    ${COMMON_PATH}/types/PolicyType.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/cynara/types
    )
INSTALL(FILES
    ${COMMON_PATH}/plugin/ExternalPluginInterface.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/cynara/plugin
    )
