/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/common/types/string_validation.h
 * @author      Oskar Świtalski <o.switalski@samsung.com>
 * @version     1.0
 */

#ifndef SRC_COMMON_TYPES_STRINGVALIDATION_H
#define SRC_COMMON_TYPES_STRINGVALIDATION_H

#include <cstring>

#include <cynara-limits.h>

inline bool isStringValid(const char *string) {
    return !!string && strlen(string) <= CYNARA_MAX_ID_LENGTH;
}

inline bool isExtraStringValid(const char *string) {
    return !string || strlen(string) <= CYNARA_MAX_ID_LENGTH;
}

#endif // SRC_COMMON_TYPES_STRINGVALIDATION_H
