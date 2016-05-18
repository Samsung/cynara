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
 * @file        src/common/containers/SharedString.h
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @version     1.0
 * @brief       This file is the header file of SharedString
 */

#ifndef SRC_COMMON_CONTAINERS_SHAREDSTRING_H_
#define SRC_COMMON_CONTAINERS_SHAREDSTRING_H_

#include <map>
#include <memory>
#include <string>

namespace Cynara {

struct SharedStringInternal {
    int refCount;
    std::string value;
};

class StringStorage {
public:
    typedef std::unique_ptr<SharedStringInternal> SharedStringUniquePtr;
    static StringStorage& getInstance(void);
    SharedStringInternal* get(const std::string &key);
    void erase(const std::string &key);
protected:
    StringStorage(){}
    std::map<std::string, SharedStringUniquePtr> m_sharedStringMap;
};

class SharedString {
public:
    explicit SharedString(const std::string &key = std::string());
    SharedString(SharedStringInternal *internal);
    SharedString(const SharedString &second);
    SharedString(SharedString &&second);
    SharedString& operator=(const SharedString &second);
    SharedString& operator=(SharedString &&second);
    const std::string& toString(void) const;
    virtual ~SharedString();

protected:
    void addref(void);
    void unref(void);

    SharedStringInternal *m_internal;
};

} // namespace Cynara

#endif // SRC_COMMON_CONTAINERS_SHAREDSTRING_H_

