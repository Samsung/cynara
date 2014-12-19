/*
 *  Copyright (c) 2014 Samsung Electronics Co.
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
 * @file        src/admin/logic/Utility.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of utility function for managing null pointer
 *              terminated lists.
 */

#ifndef SRC_ADMIN_LOGIC_UTILITY_H_
#define SRC_ADMIN_LOGIC_UTILITY_H_

#include <cstdlib>
#include <functional>
#include <memory>
#include <vector>

#include <cynara-error.h>

namespace Cynara {

//This function requires proper freeElem function for given type T.
template<typename T>
static void freeNullTerminatedList(T **list) {
    for (int i = 0; list[i]; i++) {
        freeElem(list[i]);
    }
    free(list);
}

template<typename V, typename T>
static int createNullTerminatedArray(const std::vector<V> &vectorized, T ***array,
                                     std::function<int(const V&, T*&)> copyFun) {
    size_t elems = vectorized.size();

    T **tab = reinterpret_cast<T**>(calloc(elems + 1U, sizeof(T*)));
    if (!tab)
        return CYNARA_API_OUT_OF_MEMORY;

    std::unique_ptr<T*, std::function<void(T**)>> plumber(tab, freeNullTerminatedList<T>);

    for (size_t i = 0; i < elems; i++) {
        int ret = copyFun(vectorized[i], tab[i]);
        if (ret != CYNARA_API_SUCCESS)
            return ret;
    }

    *array = tab;
    plumber.release();
    return CYNARA_API_SUCCESS;
}

} // namespace Cynara

#endif /* SRC_ADMIN_LOGIC_UTILITY_H_ */
