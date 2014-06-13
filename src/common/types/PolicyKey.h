/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        PolicyKey.h
 * @author      Lukasz Wojciechowski <l.wojciechowski@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines PolicyKey - triple, which defines a single
                policy rule
 */

#ifndef CYNARA_COMMON_TYPES_POLICYKEY_H
#define CYNARA_COMMON_TYPES_POLICYKEY_H

#include "ClientId.h"
#include "UserId.h"
#include "PrivilegeId.h"

#include <tuple>

namespace Cynara {

struct PolicyKey
{
public:
    PolicyKey() {};
    PolicyKey(const ClientId &clientId, const UserId &userId, const PrivilegeId &privilegeId)
        : m_client(clientId), m_user(userId), m_privilege(privilegeId) {};

    bool operator==(const PolicyKey &other) const {
        return std::tie(m_client, m_user, m_privilege)
            == std::tie(other.m_client, other.m_user, other.m_privilege);
    }

private:
    ClientId m_client;
    UserId m_user;
    PrivilegeId m_privilege;
};

}
#endif /* CYNARA_COMMON_TYPES_POLICYKEY_H */
