# Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
set(CYNARA_VERSION 0.10.0)

############################# cmake packages ##################################

INCLUDE(FindPkgConfig)
INCLUDE(CheckCXXCompilerFlag)
INCLUDE(GNUInstallDirs)

########################## search for packages ################################

PKG_CHECK_MODULES(SYSTEMD_DEP
    QUIET
    libsystemd-daemon
    libsystemd-journal
    )

IF (SYSTEMD_DEP_FOUND)
    SET(BUILD_WITH_SYSTEMD ON)
ENDIF (SYSTEMD_DEP_FOUND)

########################  directory configuration  ############################

SET(LIB_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
    CACHE PATH
    "Object code libraries directory")

SET(BIN_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
    CACHE PATH
    "User executables directory")

SET(SBIN_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SBINDIR}"
    CACHE PATH
    "System admin executables directory")

SET(SYS_CONFIG_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}"
    CACHE PATH
    "Read-only single-machine data directory")

SET(INCLUDE_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}"
    CACHE PATH
    "Header files directory")

SET(LOCAL_STATE_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALSTATEDIR}"
    CACHE PATH
    "Modifiable single-machine data directory")

SET(DATA_ROOT_DIR
    "${CMAKE_INSTALL_PREFIX}/share"
    CACHE PATH
    "Read-only architecture-independent data root directory")

SET(SYSTEMD_UNIT_DIR
    "${CMAKE_INSTALL_PREFIX}/lib/systemd/system"
    CACHE PATH
    "Systemd system unit file directory")

SET(SOCKET_DIR
    "${CMAKE_INSTALL_PREFIX}/tmp"
    CACHE PATH
    "Running system information directory")

ADD_DEFINITIONS("-DLIB_DIR=\"${LIB_DIR}\"")
ADD_DEFINITIONS("-DBIN_DIR=\"${BIN_DIR}\"")
ADD_DEFINITIONS("-DSBIN_DIR=\"${SBIN_DIR}\"")
ADD_DEFINITIONS("-DSYS_CONFIG_DIR=\"${SYS_CONFIG_DIR}\"")
ADD_DEFINITIONS("-DINCLUDE_DIR=\"${INCLUDE_DIR}\"")
ADD_DEFINITIONS("-DLOCAL_STATE_DIR=\"${LOCAL_STATE_DIR}\"")
ADD_DEFINITIONS("-DDATA_ROOT_DIR=\"${DATA_ROOT_DIR}\"")
ADD_DEFINITIONS("-DSYSTEMD_UNIT_DIR=\"${SYSTEMD_UNIT_DIR}\"")
ADD_DEFINITIONS("-DSOCKET_DIR=\"${SOCKET_DIR}\"")

############################# compiler flags ##################################

SET(CMAKE_CXX_FLAGS_PROFILING  "-O0 -g -pg")
SET(CMAKE_CXX_FLAGS_DEBUG      "-O0 -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE    "-O2 -g")
SET(CMAKE_CXX_FLAGS_CCOV       "-O2 -g --coverage")

# Check for C++11 support and enable proper compilation flags
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
IF(COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    IF(COMPILER_SUPPORTS_CXX0X)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    ELSE()
        MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    ENDIF()
ENDIF()

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings

# Don't export symbols by default
ADD_DEFINITIONS("-fvisibility=hidden")

# Export version of cynara
ADD_DEFINITIONS("-DCYNARA_VERSION=\"${CYNARA_VERSION}\"")

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

IF (BUILD_WITH_SYSTEMD)
    ADD_DEFINITIONS("-DBUILD_WITH_SYSTEMD")
ENDIF (BUILD_WITH_SYSTEMD)

IF (CYNARA_NO_LOGS)
    ADD_DEFINITIONS("-DCYNARA_NO_LOGS")
ENDIF (CYNARA_NO_LOGS)

SET(TARGET_CYNARA "cynara")
SET(TARGET_LIB_CYNARA "cynara-client")
SET(TARGET_LIB_CYNARA_ASYNC "cynara-client-async")
SET(TARGET_LIB_CYNARA_COMMON "cynara-client-commons")
SET(TARGET_LIB_CYNARA_ADMIN "cynara-admin")
SET(TARGET_LIB_CYNARA_AGENT "cynara-agent")
SET(TARGET_CYNARA_COMMON "cynara-commons")
SET(TARGET_CYNARA_TESTS "cynara-tests")
SET(TARGET_LIB_CREDS_COMMONS "cynara-creds-commons")
SET(TARGET_LIB_CREDS_DBUS "cynara-creds-dbus")
SET(TARGET_LIB_CREDS_GDBUS "cynara-creds-gdbus")
SET(TARGET_LIB_CREDS_SOCKET "cynara-creds-socket")
SET(TARGET_LIB_SESSION "cynara-session")
SET(TARGET_LIB_CYNARA_STORAGE "cynara-storage")
SET(TARGET_CYAD "cyad")
SET(TARGET_CHSGEN "cynara-db-chsgen")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(pkgconfig)
ADD_SUBDIRECTORY(conf)

IF (BUILD_WITH_SYSTEMD)
    ADD_SUBDIRECTORY(systemd)
ENDIF (BUILD_WITH_SYSTEMD)

ADD_SUBDIRECTORY(migration)

IF (BUILD_TESTS)
    ADD_SUBDIRECTORY(test)
ENDIF (BUILD_TESTS)
