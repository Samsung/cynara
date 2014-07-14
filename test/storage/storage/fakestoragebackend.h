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
 * @file        fakestoragebackend.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Mock of StorageBackend
 */

#ifndef FAKESTORAGEBACKEND_H_
#define FAKESTORAGEBACKEND_H_

using namespace Cynara;

class FakeStorageBackend : public StorageBackend {
public:
    MOCK_METHOD1(searchDefaultBucket, PolicyBucket(const PolicyKey &key));
    MOCK_METHOD2(searchBucket, PolicyBucket(const PolicyBucketId &bucket, const PolicyKey &key));
    MOCK_METHOD2(createBucket, void(const PolicyBucketId &bucketId,
                                    const PolicyResult &defaultPolicy));
    MOCK_METHOD2(updateBucket, void(const PolicyBucketId &bucketId,
                                    const PolicyResult &defaultPolicy));
    MOCK_METHOD1(deleteBucket, void(const PolicyBucketId &bucketId));
    MOCK_METHOD1(hasBucket, bool(const PolicyBucketId &bucketId));
    MOCK_METHOD2(deletePolicy, void(const PolicyBucketId &bucketId, const PolicyKey &key));
    MOCK_METHOD1(deleteLinking, void(const PolicyBucketId &bucket));
    MOCK_METHOD2(insertPolicy, void(const PolicyBucketId &bucketId, PolicyPtr policy));
};



#endif /* FAKESTORAGEBACKEND_H_ */
