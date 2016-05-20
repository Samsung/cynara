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
 * @file        src/common/types/PolicyKey.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines PolicyKey - triple, which defines a single
                policy rule
 */

#ifndef CYNARA_COMMON_TYPES_POLICYKEY_H_
#define CYNARA_COMMON_TYPES_POLICYKEY_H_

#include <string>

#include <containers/SharedString.h>

namespace Cynara {

class PolicyKey;

class PolicyKeyFeature {
friend class PolicyKey;

public:
    PolicyKeyFeature(const PolicyKeyFeature &) = default;
    PolicyKeyFeature(PolicyKeyFeature &&) = default;

    PolicyKeyFeature& operator=(const PolicyKeyFeature &) = default;
    PolicyKeyFeature& operator=(PolicyKeyFeature &&) = default;

    typedef std::string ValueType;

    static PolicyKeyFeature create(ValueType value) {
        return PolicyKeyFeature(value);
    }

    static PolicyKeyFeature createWildcard(void) {
        return PolicyKeyFeature(wildcardValue());
    }

    static PolicyKeyFeature createAny(void) {
        return PolicyKeyFeature(anyValue());
    }

    // TODO: Different features (client, user, privilege)
    //       shouldn't be comparable
    bool operator==(const PolicyKeyFeature &other) const {
        return valuesMatch(*this, other);
    }

    bool operator==(const PolicyKeyFeature::ValueType &other) const {
        return valuesMatch(*this, PolicyKeyFeature(other));
    }

    const std::string &toString(void) const;

    const ValueType &value(void) const {
        return m_value.toString();
    }

    SharedString toSharedString(void) const {
        return m_value;
    }

    bool isAny(void) const {
        return m_isAny;
    }

    bool matchFilter(const PolicyKeyFeature &filter) const {
        return filter.isAny() || valuesMatch(*this, filter);
    }

protected:
    explicit PolicyKeyFeature(const ValueType &value) : m_value(value),
        m_isAny(value == anyValue()) {}

    static bool anyAny(const PolicyKeyFeature &pkf1, const PolicyKeyFeature &pkf2) {
        return pkf1.isAny() || pkf2.isAny();
    }

    static bool valuesMatch(const PolicyKeyFeature &pkf1, const PolicyKeyFeature &pkf2) {
        return pkf1.value() == pkf2.value();
    }

private:
    SharedString m_value;
    bool m_isAny;

    const static std::string &wildcardValue(void);
    const static std::string &anyValue(void);
};

class PolicyKey
{

public:
    PolicyKey(const PolicyKeyFeature &clientId, const PolicyKeyFeature &userId,
              const PolicyKeyFeature &privilegeId)
        : m_client(clientId), m_user(userId), m_privilege(privilegeId) {};

    PolicyKey(const PolicyKeyFeature::ValueType &clientId,
              const PolicyKeyFeature::ValueType &userId,
              const PolicyKeyFeature::ValueType &privilegeId)
        : m_client(clientId), m_user(userId), m_privilege(privilegeId) {};

    PolicyKey(const PolicyKey &) = default;
    PolicyKey(PolicyKey &&) = default;

    PolicyKey& operator=(const PolicyKey &) = default;
    PolicyKey& operator=(PolicyKey &&) = default;

    bool operator==(const PolicyKey &other) const {
        return std::tie(m_client, m_user, m_privilege)
               == std::tie(other.m_client, other.m_user, other.m_privilege);
    }

    std::string toString(void) const;

    const PolicyKeyFeature &client(void) const {
        return m_client;
    }

    const PolicyKeyFeature &user(void) const {
        return m_user;
    }

    const PolicyKeyFeature &privilege(void) const {
        return m_privilege;
    }

    bool matchFilter(const PolicyKey &filter) const {
        return m_client.matchFilter(filter.m_client) && m_user.matchFilter(filter.m_user)
               && m_privilege.matchFilter(filter.m_privilege);
    }

private:
    PolicyKeyFeature m_client;
    PolicyKeyFeature m_user;
    PolicyKeyFeature m_privilege;
};

bool operator ==(const PolicyKeyFeature::ValueType &pkf1, const PolicyKeyFeature &pkf2);

} /* namespace Cynara */


#endif /* CYNARA_COMMON_TYPES_POLICYKEY_H_ */
