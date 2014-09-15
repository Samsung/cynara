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
 * @file        bucket.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Performance tests for Cynara::PolicyBucket
 */

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyKey.h>
#include <types/PolicyType.h>

#include "../../Benchmark.h"

using namespace Cynara;

class Benchmark {
public:

};

class PolicyKeyGenerator {
public:
    typedef std::vector<std::string> Features;
    typedef std::reference_wrapper<Features> FeaturesRef;

    PolicyKeyGenerator(size_t featuresCount, size_t featureLength) {
        const std::vector<FeaturesRef> toGenerate = { m_clients, m_users, m_privileges };
        for (auto featuresRef : toGenerate) {
            auto &features = featuresRef.get();
            features.resize(featuresCount);
            std::generate(std::begin(features), std::end(features),
                std::bind(&PolicyKeyGenerator::randomKeyFeature, this, featureLength));
        }
    }

    PolicyKey randomKey(void) const {
        return { m_clients.at(rand() % m_clients.size()),
                 m_users.at(rand() % m_users.size()),
                 m_privileges.at(rand() % m_privileges.size())
        };
    }

    char randomChar(void) const {
        return (std::rand() % ('z' - 'a')) + 'a';
    }

    std::string randomKeyFeature(size_t length) const {
        std::string str(length, 0);
        std::generate_n( str.begin(), length, std::bind(&PolicyKeyGenerator::randomChar, this));
        return str;
    }

private:
    Features m_clients;
    Features m_users;
    Features m_privileges;
};

TEST(Performance, bucket_filtered_100000) {
    using std::chrono::microseconds;

    PolicyBucket bucket;

    PolicyKeyGenerator generator(100, 10);

    const std::size_t policyNumber = 100000;
    for (std::size_t i = 0; i < policyNumber; ++i) {
        bucket.insertPolicy(std::make_shared<Policy>(generator.randomKey(),
                            PredefinedPolicyType::ALLOW));
    }

    const unsigned int measureRepeats = 1000;
    auto result = Benchmark::measure<microseconds>([&bucket, &generator, measureRepeats] () {
        for (auto i = 0u; i < measureRepeats; ++i) {
            bucket.filtered(generator.randomKey());
        }
    });

    auto key = std::string("performance_" + std::to_string(policyNumber));
    auto value = std::to_string(result.count() / measureRepeats) + " [us]";
    RecordProperty(key, value);
}
