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
 * @file        test/cyad/FakeErrorApiWrapper.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Wrapper around cynara-error API (mock)
 */

#ifndef TEST_CYAD_FAKEERRORAPIWRAPPER_H_
#define TEST_CYAD_FAKEERRORAPIWRAPPER_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cyad/BaseErrorApiWrapper.h>

class FakeErrorApiWrapper : public Cynara::BaseErrorApiWrapper {
public:
    using BaseErrorApiWrapper::BaseErrorApiWrapper;

    MOCK_METHOD3(cynara_strerror,
                 int((int errnum, char *buf, size_t buflen)));
};

#endif /* TEST_CYAD_FAKEERRORAPIWRAPPER_H_ */
