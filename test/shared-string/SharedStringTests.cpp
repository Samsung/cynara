/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/shared-string/SharedStringTests.cpp
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @version     1.0
 * @brief       SharedString implementation tests.
 */
#include <string>

#include <gtest/gtest.h>

#include "SharedStringEx.h"

using namespace Cynara;

namespace {
int getRefCount(const std::string &pattern) {
    return StringStorageEx::getInstance().getRefCount(pattern);
}
} // namespace anonymous

TEST(SharedString, ConstructorDestructor) {
    std::string pattern = "Constructor";
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    {
        SharedString ex(pattern);
        ASSERT_EQ(1, getRefCount(pattern));
        SharedString ex2(pattern);
        ASSERT_EQ(2, getRefCount(pattern));
    }
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
}

TEST(SharedString, CopyConstructor) {
    std::string pattern = "CopyConstr";
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    {
        SharedString ex(pattern);
        ASSERT_EQ(1, getRefCount(pattern));
        {
            SharedString copy(ex);
            ASSERT_EQ(2, getRefCount(pattern));
        }
        ASSERT_EQ(1, getRefCount(pattern));
    }
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
}

TEST(SharedString, MoveConstructor) {
    std::string pattern = "MoveConstr";
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    {
        SharedString ex(pattern);
        ASSERT_EQ(1, getRefCount(pattern));
        {
            SharedString copy(std::move(ex));
            ASSERT_EQ(1, getRefCount(pattern));
        }
        ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    }
}

TEST(SharedString, CopyOperator1) {
    std::string pattern = "CopyOp";
    std::string pattern2 = "COp";
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern2));
    {
        SharedString ex(pattern);
        ASSERT_EQ(1, getRefCount(pattern));
        {
            SharedString copy(pattern2);
            ASSERT_EQ(1, getRefCount(pattern2));
            copy = ex;
            ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern2));
            ASSERT_EQ(2, getRefCount(pattern));
        }
        ASSERT_EQ(1, getRefCount(pattern));
    }
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
}

TEST(SharedString, CopyOperator2) {
    std::string pattern = "CopyOp";
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    {
        SharedString ex(pattern);
        ASSERT_EQ(1, getRefCount(pattern));
        ex = ex;
        ASSERT_EQ(1, getRefCount(pattern));
    }
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
}

TEST(SharedString, MoveOperator1) {
    std::string pattern = "MoveOp";
    std::string pattern2 = "MoveOp2";
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern2));
    {
        SharedString ex(pattern);
        ASSERT_EQ(1, getRefCount(pattern));
        {
            SharedString copy(pattern2);
            ASSERT_EQ(1, getRefCount(pattern2));
            copy = std::move(ex);
            ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern2));
            ASSERT_EQ(1, getRefCount(pattern));
        }
        ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    }
}

TEST(SharedString, MoveOperator2) {
    std::string pattern = "MoveOp";
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
    {
        SharedString ex(pattern);
        ASSERT_EQ(1, getRefCount(pattern));
        ex = std::move(ex);
        ASSERT_EQ(1, getRefCount(pattern));
    }
    ASSERT_EQ(StringStorageEx::NOT_FOUND, getRefCount(pattern));
}

