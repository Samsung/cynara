# Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
# @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
#

SET(CHSGEN_PATH ${CYNARA_PATH}/chsgen)

SET(CHSGEN_SOURCES
    ${CHSGEN_PATH}/ChecksumGenerator.cpp
    ${CHSGEN_PATH}/main.cpp
    ${CYNARA_EXTERNAL_SRC_PATH}/md5.c
    ${CYNARA_EXTERNAL_SRC_PATH}/md5wrapper.cpp
    )

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}
    ${CYNARA_PATH}/include
    ${CYNARA_EXTERNAL_SRC_PATH}
    )

ADD_EXECUTABLE(${TARGET_CHSGEN} ${CHSGEN_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_CHSGEN}
    crypt
    )

INSTALL(TARGETS ${TARGET_CHSGEN} DESTINATION ${SBIN_DIR})
