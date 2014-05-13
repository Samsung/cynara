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
 * @file        Storage.cpp
 * @author      Lukasz Wojciechowski <l.wojciechowski@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements policy rules storage procedures
 */

#include "Storage.h"

Result Storage::checkPolicy(const PolicyKey& key, ExtendedPolicyType& result)
{
	//todo
	TODO_USE_ME(key);
	TODO_USE_ME(result);
	return RESULT_OK;
}

Result Storage::insertOrUpdatePolicy(const PolicyVector& policyVector, const PolicyBucket& bucket)
{
	//todo
	TODO_USE_ME(policyVector);
	TODO_USE_ME(bucket);
	return RESULT_OK;
}

Result Storage::insertOrUpdateBucket(const PolicyBucket& newBucket, const PolicyKey& key, const ExtendedPolicyType& defaultBucketPolicy, const PolicyBucket& bucket)
{
	//todo
	TODO_USE_ME(newBucket);
	TODO_USE_ME(key);
	TODO_USE_ME(defaultBucketPolicy);
	TODO_USE_ME(bucket);
	return RESULT_OK;
}

Result Storage::deletePolicy(const PolicyKey& policyKey, const PolicyBucket& bucket)
{
	//todo
	TODO_USE_ME(policyKey);
	TODO_USE_ME(bucket);
	return RESULT_OK;
}

Result Storage::deleteBucket(const PolicyBucket& bucket)
{
	//todo
	TODO_USE_ME(bucket);
	return RESULT_OK;
}

/*
Result Storage::listPolicy(const PolicyKey& policyKey, PolicyVector& result)
{
	//todo
	TODO_USE_ME(policyKey);
	TODO_USE_ME(result);;
	return RESULT_OK;
}
*/
