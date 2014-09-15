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
 * @file        test/storage/inmemorystoragebackend/fakeinmemorystoragebackend.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Mock of InMemoryStorageBackend
 */

#ifndef FAKEINMEMORYSTORAGEBACKEND_H_
#define FAKEINMEMORYSTORAGEBACKEND_H_

#include <storage/Buckets.h>
#include <storage/InMemoryStorageBackend.h>

class FakeInMemoryStorageBackend : public Cynara::InMemoryStorageBackend {
public:
    using Cynara::InMemoryStorageBackend::InMemoryStorageBackend;
    MOCK_METHOD0(buckets, Cynara::Buckets&());
};


#endif /* FAKEINMEMORYSTORAGEBACKEND_H_ */
