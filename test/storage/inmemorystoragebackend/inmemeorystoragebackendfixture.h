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
/**
 * @file        inmemeorystoragebackendfixture.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Fixture for InMemeoryStorageBackend tests
 */

#ifndef INMEMEORYSTORAGEBACKENDFIXTURE_H_
#define INMEMEORYSTORAGEBACKENDFIXTURE_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <types/PolicyBucket.h>
#include <types/PolicyCollection.h>
#include <storage/Buckets.h>
#include <storage/InMemoryStorageBackend.h>

class InMemeoryStorageBackendFixture : public ::testing::Test {

protected:
    Cynara::Buckets::mapped_type &
    createBucket(const Cynara::PolicyBucketId &bucketId) {
        auto bucket = Cynara::PolicyBucket();
        return m_buckets.insert({ bucketId, bucket }).first->second;
    }

    Cynara::Buckets::mapped_type &
    createBucket(const Cynara::PolicyBucketId &bucketId, const Cynara::PolicyCollection &policies) {
        auto bucket = Cynara::PolicyBucket(policies);
        return m_buckets.insert({ bucketId, bucket }).first->second;
    }

    void addToBucket(Cynara::PolicyBucketId bucketId, const Cynara::PolicyCollection &policies) {
        // TODO: Consider altering PolicyMap directly
        for (const auto &policy : policies) {
            m_buckets[bucketId].insertPolicy(policy);
        }
    }

    static void ASSERT_DB_VIRGIN(Cynara::Buckets &buckets)  {
        using ::testing::IsEmpty;
        ASSERT_EQ(1, buckets.size());
        auto defaultBucketIter = buckets.find(Cynara::defaultPolicyBucketId);
        ASSERT_NE(buckets.end(), defaultBucketIter);
        auto &defaultBucket = defaultBucketIter->second;
        ASSERT_THAT(defaultBucket, IsEmpty());
        ASSERT_EQ(Cynara::PredefinedPolicyType::DENY, defaultBucket.defaultPolicy());
    }

    virtual ~InMemeoryStorageBackendFixture() {}

    // TODO: consider defaulting accessor with ON_CALL
    Cynara::Buckets m_buckets;
};


#endif /* INMEMEORYSTORAGEBACKENDFIXTURE_H_ */
