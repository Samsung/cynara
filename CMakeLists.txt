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
# @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
#

############################# Check minimum CMake version #####################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
PROJECT("cynara")

############################# cmake packages ##################################

INCLUDE(FindPkgConfig)

############################# compiler flags ##################################

SET(CMAKE_C_FLAGS_PROFILING    "-g -O0 -pg")
SET(CMAKE_CXX_FLAGS_PROFILING  "-g -std=c++0x -O0 -pg")
SET(CMAKE_C_FLAGS_DEBUG        "-g -O0 -ggdb")
SET(CMAKE_CXX_FLAGS_DEBUG      "-g -std=c++0x -O0 -ggdb")
SET(CMAKE_C_FLAGS_RELEASE      "-g -O2")
SET(CMAKE_CXX_FLAGS_RELEASE    "-g -std=c++0x -O2")
SET(CMAKE_C_FLAGS_CCOV         "-g -O2 --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       "-g -std=c++0x -O2 --coverage")

# If supported for the target machine, emit position-independent code,suitable
# for dynamic linking and avoiding any limit on the size of the global offset
# table. This option makes a difference on the m68k, PowerPC and SPARC.
ADD_DEFINITIONS("-fPIC")

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings

STRING(REGEX MATCH "([^.]*)" API_VERSION "${VERSION}")
ADD_DEFINITIONS("-DAPI_VERSION=\"$(API_VERSION)\"")

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

SET(TARGET_CYNARA "cynara")
SET(TARGET_LIB_CYNARA "cynara-client")
SET(TARGET_LIB_CYNARA_ADMIN "cynara-admin")
SET(TARGET_CYNARA_COMMON "cynara-commons")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(build)
ADD_SUBDIRECTORY(systemd)
