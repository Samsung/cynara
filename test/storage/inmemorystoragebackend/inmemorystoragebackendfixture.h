/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/storage/inmemorystoragebackend/inmemorystoragebackendfixture.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Fixture for InMemoryStorageBackend tests
 */

#ifndef INMEMORYSTORAGEBACKENDFIXTURE_H_
#define INMEMORYSTORAGEBACKENDFIXTURE_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>
#include <string>

#include <types/PolicyBucket.h>
#include <types/PolicyCollection.h>
#include <types/PolicyKey.h>
#include <storage/Buckets.h>
#include <storage/InMemoryStorageBackend.h>

#include <TestDataCollection.h>

class InMemoryStorageBackendFixture : public ::testing::Test {
public:
    class Filter {
    public:
        Filter(const std::string &name, const Cynara::PolicyKey &key) :
            m_name(name), m_key(key) {}

        const std::string &name(void) const {
            return m_name;
        }

        const Cynara::PolicyKey &key(void) const {
            return m_key;
        }

    private:
        std::string m_name;
        Cynara::PolicyKey m_key;
    };

    typedef std::vector<Filter> Filters;

protected:
    Cynara::Buckets::mapped_type &
    createBucket(const Cynara::PolicyBucketId &bucketId) {
        auto bucket = Cynara::PolicyBucket(bucketId);
        return m_buckets.insert({ bucketId, bucket }).first->second;
    }

    Cynara::Buckets::mapped_type &
    createBucket(const Cynara::PolicyBucketId &bucketId, const Cynara::PolicyCollection &policies) {
        auto bucket = Cynara::PolicyBucket(bucketId, policies);
        return m_buckets.insert({ bucketId, bucket }).first->second;
    }

    void addToBucket(Cynara::PolicyBucketId bucketId, const Cynara::PolicyCollection &policies) {
        // TODO: Consider altering PolicyMap directly
        for (const auto &policy : policies) {
            m_buckets.at(bucketId).insertPolicy(policy);
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

    static void ASSERT_DB_EMPTY(Cynara::Buckets &buckets) {
        using ::testing::IsEmpty;
        ASSERT_EQ(0, buckets.size());
        ASSERT_THAT(buckets, IsEmpty());
    }

    virtual ~InMemoryStorageBackendFixture() {}

    const Cynara::PolicyCollection &fullPoliciesCollection(void) {
        using Cynara::PredefinedPolicyType::ALLOW;
        if (m_fullPolicyCollection.empty()) {
            m_fullPolicyCollection = {
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_cup, ALLOW),
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_wup, ALLOW),
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_cwp, ALLOW),
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_cuw, ALLOW),
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_cww, ALLOW),
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_wuw, ALLOW),
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_wwp, ALLOW),
                Cynara::Policy::simpleWithKey(TestDataCollection::Keys::k_www, ALLOW),
            };
        }
        return m_fullPolicyCollection;
    }

    const Filters &filters(void) {
        if (m_filters.empty()) {
            m_filters.push_back(Filter("cup", TestDataCollection::Keys::k_cup));
            m_filters.push_back(Filter("www", TestDataCollection::Keys::k_www));
            m_filters.push_back(Filter("wuw", TestDataCollection::Keys::k_wuw));
            m_filters.push_back(Filter("aaa", TestDataCollection::Keys::k_aaa));
            m_filters.push_back(Filter("wua", TestDataCollection::Keys::k_wua));
            m_filters.push_back(Filter("wwa", TestDataCollection::Keys::k_wwa));
            m_filters.push_back(Filter("aup", TestDataCollection::Keys::k_aup));
        }
        return m_filters;
    }

    const Cynara::PolicyCollection &expectedEraseResult(const std::string &filterName) {
        if (m_expectedResults.empty()) {
            const Cynara::PolicyCollection &pc = fullPoliciesCollection();
        //                              "cup", "wup", "cwp", "cuw", "cww", "wuw", "wwp", "www"
            m_expectedResults["cup"] = {       pc[1], pc[2], pc[3], pc[4], pc[5], pc[6], pc[7]};
            m_expectedResults["www"] = {pc[0], pc[1], pc[2], pc[3], pc[4], pc[5], pc[6],      };
            m_expectedResults["wuw"] = {pc[0], pc[1], pc[2], pc[3], pc[4],        pc[6], pc[7]};
            m_expectedResults["aaa"] = {                                                      };
            m_expectedResults["wua"] = {pc[0],        pc[2], pc[3], pc[4],        pc[6], pc[7]};
            m_expectedResults["wwa"] = {pc[0], pc[1], pc[2], pc[3], pc[4], pc[5],             };
            m_expectedResults["aup"] = {              pc[2], pc[3], pc[4], pc[5], pc[6], pc[7]};
        }

        return m_expectedResults[filterName];
    }

    bool isLinkPolicyErased(const std::string &filterName) {
        if (filterName == "aaa")
            return true;
        return false;
    }

    // TODO: consider defaulting accessor with ON_CALL
    Cynara::Buckets m_buckets;
    static const std::string m_indexFileName;
    static const std::string m_backupFileNameSuffix;
    static const std::string m_fakeDbPath;

    //erase helper structures
    Cynara::PolicyCollection m_fullPolicyCollection;
    Filters m_filters;
    std::map<std::string, Cynara::PolicyCollection> m_expectedResults;
};


#endif /* INMEMORYSTORAGEBACKENDFIXTURE_H_ */
