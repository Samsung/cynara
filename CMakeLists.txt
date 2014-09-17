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
# @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
#

############################# Check minimum CMake version #####################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
PROJECT("cynara")
set(CYNARA_VERSION 0.2.2)

############################# cmake packages ##################################

INCLUDE(FindPkgConfig)
INCLUDE(CheckCXXCompilerFlag)

#############################  install dirs  ##################################

SET(LIB_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/lib"
    CACHE PATH
    "Library installation directory")

SET(BIN_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/bin"
    CACHE PATH
    "Binary installation directory")

SET(INCLUDE_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/include"
    CACHE PATH
    "Include installation directory")

############################# compiler flags ##################################

SET(CMAKE_CXX_FLAGS_PROFILING  "-O0 -g -pg")
SET(CMAKE_CXX_FLAGS_DEBUG      "-O0 -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE    "-O2 -g")
SET(CMAKE_CXX_FLAGS_CCOV       "-O2 -g --coverage")

# Check for C++11 support and enable proper compilation flags
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
IF(COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSEIF(COMPILER_SUPPORTS_CXX0X)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ELSE()
    MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF()

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings

# Don't export symbols by default
ADD_DEFINITIONS("-fvisibility=hidden")


IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

SET(TARGET_CYNARA "cynara")
SET(TARGET_LIB_CYNARA "cynara-client")
SET(TARGET_LIB_CYNARA_COMMON "cynara-client-commons")
SET(TARGET_LIB_CYNARA_ADMIN "cynara-admin")
SET(TARGET_CYNARA_COMMON "cynara-commons")
SET(TARGET_CYNARA_TESTS "cynara-tests")
SET(TARGET_LIB_CREDS_COMMONS "cynara-creds-commons")
SET(TARGET_LIB_CREDS_DBUS "cynara-creds-dbus")
SET(TARGET_LIB_CREDS_SOCKET "cynara-creds-socket")
SET(TARGET_LIB_SESSION "cynara-session")
SET(TARGET_LIB_CYNARA_STORAGE "cynara-storage")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(pkgconfig)
ADD_SUBDIRECTORY(systemd)

IF (BUILD_TESTS)
ADD_SUBDIRECTORY(test)
ENDIF()
