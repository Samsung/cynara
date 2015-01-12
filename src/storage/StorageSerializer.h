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
 * @file        src/storage/StorageSerializer.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Headers for Cynara::StorageSerializer
 */

#ifndef SRC_STORAGE_STORAGESERIALIZER_H_
#define SRC_STORAGE_STORAGESERIALIZER_H_

#include <functional>
#include <fstream>
#include <ios>
#include <memory>
#include <sstream>

#include <config/PathConfig.h>
#include <exceptions/BucketSerializationException.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyCollection.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <storage/Buckets.h>

namespace Cynara {

template<typename StreamType>
class StorageSerializer {

public:
    typedef std::function<std::shared_ptr<StorageSerializer>(const PolicyBucketId &)>
            BucketStreamOpener;

    StorageSerializer(std::shared_ptr<StreamType> os);
    virtual ~StorageSerializer() {};

    virtual void dump(const Buckets &buckets,
                      BucketStreamOpener streamOpener);
    virtual void dump(const PolicyBucket &bucket);

protected:
    template<typename Arg1, typename... Args>
    inline void dumpFields(const Arg1 &arg1, const Args&... args) {
        dump(arg1);
        if (sizeof...(Args) > 0) {
            *m_outStream << PathConfig::StoragePath::fieldSeparator;
        }
        dumpFields(args...);
    }

    inline void dumpFields(void) {
        *m_outStream << PathConfig::StoragePath::recordSeparator;
    }

    void dump(const PolicyKeyFeature &keyFeature);
    void dump(const PolicyType &policyType);
    void dump(const PolicyResult::PolicyMetadata &metadata);
    void dump(const PolicyCollection::value_type &policy);

private:
    std::shared_ptr<StreamType> m_outStream;
};

template<typename StreamType>
StorageSerializer<StreamType>::StorageSerializer(std::shared_ptr<StreamType> os) : m_outStream(os) {
}

template<typename StreamType>
void StorageSerializer<StreamType>::dump(const Buckets &buckets, BucketStreamOpener streamOpener) {
    for (const auto bucketIter : buckets) {
        const auto &bucket = bucketIter.second;

        dumpFields(bucket.id(), bucket.defaultPolicy().policyType(),
                   bucket.defaultPolicy().metadata());
    }

    for (const auto bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        const auto &bucket = bucketIter.second;
        auto bucketSerializer = streamOpener(bucketId);

        if (bucketSerializer != nullptr) {
            bucketSerializer->dump(bucket);
        } else {
            throw BucketSerializationException(bucketId);
        }
    }
}

template<typename StreamType>
void StorageSerializer<StreamType>::dump(const PolicyBucket &bucket) {
    for (auto it = std::begin(bucket); it != std::end(bucket); ++it) {
        const auto &policy = *it;
        dump(policy);
    }
}

template<typename StreamType>
void StorageSerializer<StreamType>::dump(const PolicyKeyFeature &keyFeature) {
    *m_outStream << keyFeature.toString();
}

template<typename StreamType>
void StorageSerializer<StreamType>::dump(const PolicyType &policyType) {
    auto oldFormat = m_outStream->flags();
    *m_outStream << "0x" << std::uppercase <<  std::hex << policyType;
    m_outStream->flags(oldFormat);
}

template<typename StreamType>
void StorageSerializer<StreamType>::dump(const PolicyResult::PolicyMetadata &metadata) {
    *m_outStream << metadata;
}

template<typename StreamType>
void StorageSerializer<StreamType>::dump(const PolicyCollection::value_type &policy) {
    const auto &key = policy->key();
    const auto &result = policy->result();

    dumpFields(key.client(), key.user(), key.privilege(), result.policyType(), result.metadata());
}

} /* namespace Cynara */

#endif /* SRC_STORAGE_STORAGESERIALIZER_H_ */
