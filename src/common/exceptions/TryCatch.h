/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/exceptions/TryCatch.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains functions for catching exceptions
 */

#ifndef SRC_COMMON_EXCEPTIONS_TRYCATCH_H_
#define SRC_COMMON_EXCEPTIONS_TRYCATCH_H_

#include <exception>
#include <functional>
#include <new>

#include <exceptions/NoMemoryException.h>
#include <log/log.h>

#include <cynara-error.h>

namespace Cynara {

int tryCatch(const std::function<int(void)> &func) {
    try {
        return func();
    } catch (const std::bad_alloc &e) {
        LOGE(e.what());
        return CYNARA_API_OUT_OF_MEMORY;
    } catch (const NoMemoryException &e) {
        LOGE(e.what());
        return CYNARA_API_OUT_OF_MEMORY;
    } catch (const std::exception &e) {
        LOGE(e.what());
        return CYNARA_API_UNKNOWN_ERROR;
    } catch (...) {
        LOGE("Unexpected exception");
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

} // namespace Cynara

#endif // SRC_COMMON_EXCEPTIONS_TRYCATCH_H_
