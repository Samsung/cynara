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
 * @file        Storage.h
 * @author      Lukasz Wojciechowski <l.wojciechowski@partner.samsung.com>
 * @version     1.0
 * @brief       This file is the implementation file of log system
 */

#ifndef CYNARA_SERVICE_STORAGE_STORAGE_H
#define CYNARA_SERVICE_STORAGE_STORAGE_H

#include <common.h>

class Storage
{
public:
//CHECKING policies
//checks policies matching key and returns minimal policy type as result
	Result checkPolicy(const PolicyKey& key, ExtendedPolicyType& result);

//ADMINISTRATION of polices
//insert (or update) new policies defined in policyVector into bucket
	Result insertOrUpdatePolicy(const PolicyVector& policyVector, const PolicyBucket& bucket = defaultPolicyBucket);
//insert (or update) new bucket (newBucket) in bucket for key with defaultPolicyKey
	Result insertOrUpdateBucket(const PolicyBucket& newBucket, const PolicyKey& key, const ExtendedPolicyType& defaultBucketPolicy, const PolicyBucket& bucket = defaultPolicyBucket);
//remove single policy rule
	Result deletePolicy(const PolicyKey& policyKey, const PolicyBucket& bucket = defaultPolicyBucket);
//remove bucket and all policy rules related (inside or pointing to) bucket
	Result deleteBucket(const PolicyBucket& bucket);

//listing will be defined later
//	Result listPolicy(const PolicyKey& policyKey, PolicyVector& result);
};

#endif /* CYNARA_SERVICE_STORAGE_STORAGE_H */
