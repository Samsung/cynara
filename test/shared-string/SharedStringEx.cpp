/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        test/shared-string/SharedStringEx.cpp
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @version     1.0
 * @brief       StringStorageEx implementation.
 */
#include "SharedStringEx.h"

namespace Cynara {

const int StringStorageEx::NOT_FOUND = -1;

StringStorageEx& StringStorageEx::getInstance(void) {
    // this is dirty hack but without it we will not be able
    // to extract refCount number from StringStorage
    return static_cast<StringStorageEx&>(StringStorage::getInstance());
}

} // namespace Cynara

