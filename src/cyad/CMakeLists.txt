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
#

SET(CYAD_PATH ${CYNARA_PATH}/cyad)

SET(CYAD_SOURCES
    ${CYAD_PATH}/AdminApiWrapper.cpp
    ${CYAD_PATH}/CynaraAdminPolicies.cpp
    ${CYAD_PATH}/DispatcherIO.cpp
    ${CYAD_PATH}/main.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}
    ${CYNARA_PATH}/include
    )

ADD_EXECUTABLE(${TARGET_CYAD} ${CYAD_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_CYAD}
    ${TARGET_LIB_CYNARA_ADMIN}
    ${TARGET_LIB_CYNARA_STORAGE}
    )

INSTALL(TARGETS ${TARGET_CYAD} DESTINATION ${SBIN_INSTALL_DIR})
