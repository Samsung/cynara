# Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
# @author      Jacek Bukarewicz <j.bukarewicz@samsung.com>
#

CONFIGURE_FILE(cynara-creds-gdbus.pc.in cynara-creds-gdbus.pc @ONLY)

INSTALL(FILES
    ${CMAKE_BINARY_DIR}/pkgconfig/cynara-creds-gdbus/cynara-creds-gdbus.pc
    DESTINATION
    ${LIB_DIR}/pkgconfig
    )
