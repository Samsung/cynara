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
 * @file        test/shared-string/SharedStringEx.h
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @version     1.0
 * @brief       StringStorageEx implementation.
 */
#ifndef TEST_SHARED_STRING_SHARED_STRING_EX_H__
#define TEST_SHARED_STRING_SHARED_STRING_EX_H__

#include <string>

#include <containers/SharedString.h>

namespace Cynara {

class StringStorageEx : public StringStorage {
public:
    static const int NOT_FOUND;

    static StringStorageEx& getInstance(void);
    int getRefCount(const std::string &key) {
        auto it = m_sharedStringMap.find(key);
        if (it == m_sharedStringMap.end())
            return NOT_FOUND;

        return it->second->refCount;
    }
};

} // namespace Cynara

#endif // TEST_SHARED_STRING_SHARED_STRING_EX_H__
