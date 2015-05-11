/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/common/protocols/ProtocolSerialization.cpp
 * @author      Oskar Świtalski <o.switalski@samsung.com>
 * @version     1.0
 * @brief       Tests for limiting length of string/vector identifier
 */

#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <common/protocol/ProtocolSerialization.h>
#include <cynara-limits.h>

class FakeIStream : public Cynara::IStream {
public:
    MOCK_METHOD2(read, void(size_t, void*));
    MOCK_METHOD2(write, void(size_t, const void*));
};

ACTION_P(AssignInt, param) { *static_cast<int*>(arg1) = param; }
ACTION_P(AssignString, param) { strcpy(static_cast<char*>(arg1), param); }

TEST(Serialization, maxString) {
    using ::testing::_;

    FakeIStream stream;
    std::string string;

    string.resize(CYNARA_MAX_ID_LENGTH, ' ');

    EXPECT_CALL(stream, write(_, _)).Times(2);
    EXPECT_NO_THROW(Cynara::ProtocolSerialization::serialize(stream, string));
}

TEST(Serialization, maxVector) {
    using ::testing::_;
    FakeIStream stream;
    std::vector<char> vector;

    vector.resize(CYNARA_MAX_VECTOR_SIZE);

    EXPECT_CALL(stream, write(_, _)).Times(CYNARA_MAX_VECTOR_SIZE + 1);
    EXPECT_NO_THROW(Cynara::ProtocolSerialization::serialize(stream, vector));
}

TEST(Serialization, vectorWithOverMaxString) {
    using ::testing::_;
    FakeIStream stream;
    std::string string;
    string.resize(CYNARA_MAX_ID_LENGTH + 1, ' ');
    std::vector<std::string> vector = { string };

    EXPECT_CALL(stream, write(_, _)).Times(1);
    EXPECT_THROW(Cynara::ProtocolSerialization::serialize(stream, vector),
                 Cynara::InvalidProtocolException);
}

TEST(Serialization, overMaxVector) {
    FakeIStream stream;
    std::vector<char> vector;

    vector.resize(CYNARA_MAX_VECTOR_SIZE + 1);

    EXPECT_THROW(Cynara::ProtocolSerialization::serialize(stream, vector),
                 Cynara::InvalidProtocolException);
}

TEST(Serialization, overMaxString) {
    FakeIStream stream;
    std::string string;

    string.resize(CYNARA_MAX_ID_LENGTH + 1, ' ');

    EXPECT_THROW(Cynara::ProtocolSerialization::serialize(stream, string),
                 Cynara::InvalidProtocolException);
}

TEST(Deserialization, overMaxString) {
    using ::testing::_;

    FakeIStream stream;
    std::string string;

    EXPECT_CALL(stream, read(_, _)).WillOnce(AssignInt(CYNARA_MAX_ID_LENGTH + 1));
    EXPECT_THROW(Cynara::ProtocolDeserialization::deserialize(stream, string),
                 Cynara::InvalidProtocolException);
}

TEST(Deserialization, overMaxStringWithLength) {
    using ::testing::_;

    FakeIStream stream;
    std::string string;

    EXPECT_THROW(Cynara::ProtocolDeserialization::deserialize(stream, CYNARA_MAX_ID_LENGTH + 1,
                                                              string),
                 Cynara::InvalidProtocolException);
}

TEST(Deserialization, maxString) {
    using ::testing::_;

    FakeIStream stream;
    std::string string;
    std::string stringInput;

    stringInput.resize(CYNARA_MAX_ID_LENGTH, ' ');

    EXPECT_CALL(stream, read(_, _)).WillOnce(AssignInt(CYNARA_MAX_ID_LENGTH))
            .WillOnce(AssignString(stringInput.c_str()));
    EXPECT_NO_THROW(Cynara::ProtocolDeserialization::deserialize(stream, string));
}

TEST(Deserialization, maxStringWithLength) {
    using ::testing::_;

    FakeIStream stream;
    std::string string;
    std::string stringInput;

    stringInput.resize(CYNARA_MAX_ID_LENGTH, ' ');

    EXPECT_CALL(stream, read(_, _)).WillOnce(AssignString(stringInput.c_str()));
    EXPECT_NO_THROW(Cynara::ProtocolDeserialization::deserialize(stream, CYNARA_MAX_ID_LENGTH,
                                                                 string));
}

TEST(Deserialization, overMaxVector) {
    using ::testing::_;

    FakeIStream stream;
    std::vector<std::string> vector;

    EXPECT_CALL(stream, read(_, _)).WillOnce(AssignInt(CYNARA_MAX_ID_LENGTH + 1));
    EXPECT_THROW(Cynara::ProtocolDeserialization::deserialize(stream, vector),
                 Cynara::InvalidProtocolException);
}

TEST(Deserialization, maxVector) {
    using ::testing::_;
    using ::testing::Sequence;
    Sequence sequence;

    FakeIStream stream;
    std::string stringInput;
    std::vector<std::string> vector;
    stringInput.resize(CYNARA_MAX_ID_LENGTH, ' ');
    vector.resize(CYNARA_MAX_VECTOR_SIZE);

    EXPECT_CALL(stream, read(_, _)).InSequence(sequence)
            .WillOnce(AssignInt(CYNARA_MAX_VECTOR_SIZE));

    for (int i = 0; i < CYNARA_MAX_VECTOR_SIZE; i++) {
        EXPECT_CALL(stream, read(_, _)).InSequence(sequence)
                .WillOnce(AssignInt(CYNARA_MAX_ID_LENGTH))
                .WillOnce(AssignString(stringInput.c_str()));
    }

    EXPECT_NO_THROW(Cynara::ProtocolDeserialization::deserialize(stream, vector));
}

TEST(Deserialization, vectorWithOverMaxString) {
    using ::testing::_;

    FakeIStream stream;
    std::vector<std::string> vector;

    EXPECT_CALL(stream, read(_, _)).WillOnce(AssignInt(1))
            .WillOnce(AssignInt(CYNARA_MAX_ID_LENGTH + 1));
    EXPECT_THROW(Cynara::ProtocolDeserialization::deserialize(stream, vector),
                 Cynara::InvalidProtocolException);
}
