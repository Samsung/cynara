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
 * @file        Policy.h
 * @author      Lukasz Wojciechowski <l.wojciechowski@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines Policy type - used to describe single policy
                rule identified by policy key
 */

#ifndef CYNARA_COMMON_TYPES_POLICY_H
#define CYNARA_COMMON_TYPES_POLICY_H

#include "PolicyKey.h"
#include "ExtendedPolicyType.h"

struct Policy
{
	PolicyKey m_key;
	ExtendedPolicyType m_extType;
};

#endif /* CYNARA_COMMON_TYPES_POLICY_H */
