/*
 * Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/types/PolicyCollection.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines a collection of policies
 */

#ifndef SRC_COMMON_TYPES_POLICYCOLLECTION_H_
#define SRC_COMMON_TYPES_POLICYCOLLECTION_H_

#include <functional>
#include <unordered_map>
#include <vector>

#include "types/pointers.h"
#include <containers/SharedString.h>

namespace Cynara {

typedef std::vector<PolicyPtr> PolicyCollection;

typedef std::vector<SharedString> SharedStringVector;

struct SharedStringVectorHash {
    std::size_t operator()(const SharedStringVector &input) const {
        std::size_t result = 0x9e3779b9;
        std::hash<std::string> shasher;
        std::hash<std::size_t> thasher;
        for (auto &e : input)
            result = thasher(result ^ shasher(e.toString()));
        return result;
    }
};

struct SharedStringVectorEqual {
    bool operator()(const SharedStringVector &first, const SharedStringVector &second) const {
        if (first.size() != second.size())
            return false;

        auto it1 = first.begin();
        auto it2 = second.begin();

        for (; it1 != first.end() && it2 != second.end(); ++it1, ++it2)
            if (it1->toString() != it2->toString())
                return false;

        return true;
    }
};

typedef std::unordered_map<
        SharedStringVector,
        PolicyPtr,
        SharedStringVectorHash,
        SharedStringVectorEqual> PolicyMap;

class const_policy_iterator : public PolicyMap::const_iterator
{
  public:
    const_policy_iterator() : PolicyMap::const_iterator() {};
    const_policy_iterator(const PolicyMap::const_iterator &other)
        : PolicyMap::const_iterator(other) {};

    PolicyPtr operator*(void) {
        return PolicyMap::const_iterator::operator*().second;
    }
};

} // namespace Cynara

#endif /* SRC_COMMON_TYPES_POLICYCOLLECTION_H_     */
