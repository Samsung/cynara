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
 * @file        StorageSerializer.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Headers for Cynara::StorageSerializer
 */

#ifndef SRC_SERVICE_STORAGE_STORAGESERIALIZER_H_
#define SRC_SERVICE_STORAGE_STORAGESERIALIZER_H_

#include "InMemoryStorageBackend.h"
#include "types/PolicyBucketId.h"
#include "types/PolicyCollection.h"
#include "types/PolicyResult.h"

#include <ostream>

namespace Cynara {

class PolicyBucket;
class PolicyKey;

class StorageSerializer {

public:
    typedef std::function<std::shared_ptr<StorageSerializer>(const PolicyBucketId &)>
            BucketStreamOpener;

    StorageSerializer(std::ostream &os);
    virtual ~StorageSerializer() = default;

    virtual void dump(const InMemoryStorageBackend::Buckets &buckets,
                      BucketStreamOpener streamOpener);
    virtual void dump(const PolicyBucket &bucket);

protected:
    template<typename Arg1, typename... Args>
    inline void dumpFields(const Arg1 &arg1, const Args&... args) {
        dump(arg1);
        if (sizeof...(Args) > 0) {
            outStream() << fieldSeparator();
        }
        dumpFields(args...);
    }

    inline void dumpFields(void) {
        outStream() << recordSeparator();
    }

    void dump(const PolicyKey &key);
    void dump(const PolicyType &policyType);
    void dump(const PolicyResult::PolicyMetadata &metadata);
    void dump(const PolicyCollection::value_type &policy);

protected:
    std::ostream &outStream(void) {
        return m_outStream;
    }

private:
    std::ostream &m_outStream;

    static char m_fieldSeparator;
    static char m_recordSeparator;

public:
    static const char &fieldSeparator(void) {
        return m_fieldSeparator;
    }

    static const char &recordSeparator(void) {
        return m_recordSeparator;
    }
};

} /* namespace Cynara */

#endif /* SRC_SERVICE_STORAGE_STORAGESERIALIZER_H_ */
