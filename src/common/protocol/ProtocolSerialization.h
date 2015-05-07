/*
 * Copyright (c) 2011-2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/protocol/ProtocolSerialization.h
 * @author	Tomasz Swierczek (t.swierczek@samsung.com)
 * @author	Adam Malinowski (a.malinowsk2@samsung.com)
 * @version	1.0
 * @brief	Interfaces and templates used for data serialization.
 */
#ifndef SRC_COMMON_PROTOCOL_PROTOCOLSERIALIZATION_H_
#define SRC_COMMON_PROTOCOL_PROTOCOLSERIALIZATION_H_

#include <endian.h>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <protocol/ProtocolOpCode.h>

namespace Cynara {
// Abstract data stream buffer
class IStream {
public:
    virtual void read(size_t num, void *bytes) = 0;
    virtual void write(size_t num, const void *bytes) = 0;
    virtual ~IStream() {};
};

struct ProtocolSerialization {
    // char
    static void serialize(IStream &stream, const char value) {
        stream.write(sizeof(value), &value);
    }

    // unsigned char
    static void serialize(IStream &stream, const unsigned char value) {
        stream.write(sizeof(value), &value);
    }

    // unsigned 16-bit int
    static void serialize(IStream &stream, const uint16_t value) {
        uint16_t _value = htole16(value);
        stream.write(sizeof(value), &_value);
    }

    // 16-bit int
    static void serialize(IStream &stream, const int16_t value) {
        int16_t _value = htole16(value);
        stream.write(sizeof(value), &_value);
    }

    // unsigned 32-bit int
    static void serialize(IStream &stream, const uint32_t value) {
        uint32_t _value = htole32(value);
        stream.write(sizeof(value), &_value);
    }

    // 32-bit int
    static void serialize(IStream &stream, const int32_t value) {
        int32_t _value = htole32(value);
        stream.write(sizeof(value), &_value);
    }

    // bool
    static void serialize(IStream &stream, const bool value) {
        uint8_t bVal = static_cast<uint8_t>(value);
        stream.write(sizeof(bVal), &bVal);
    }

    // ProtocolOpCode
    static void serialize(IStream &stream, const ProtocolOpCode value) {
        stream.write(sizeof(value), &value);
    }

    // std::string
    static void serialize(IStream &stream, const std::string &str) {
        uint32_t length = htole32(static_cast<uint32_t>(str.size()));
        stream.write(sizeof(length), &length);
        stream.write(str.size(), str.c_str());
    }
    static void serializeNoSize(IStream &stream, const std::string &str) {
        int length = str.size();
        stream.write(length, str.c_str());
    }

    // std::vector
    template<typename T>
    static void serialize(IStream &stream, const std::vector<T> &vec) {
        uint32_t length = htole32(static_cast<uint32_t>(vec.size()));
        stream.write(sizeof(length), &length);
        for (typename std::vector<T>::const_iterator vec_iter = vec.begin();
                vec_iter != vec.end(); vec_iter++) {
            serialize(stream, *vec_iter);
        }
    }
}; // struct ProtocolSerialization

struct ProtocolDeserialization {
    // char
    static void deserialize(IStream &stream, char &value) {
        stream.read(sizeof(value), &value);
    }

    // unsigned char
    static void deserialize(IStream &stream, unsigned char &value) {
        stream.read(sizeof(value), &value);
    }

    // 16-bit int
    static void deserialize(IStream &stream, int16_t &value) {
        stream.read(sizeof(value), &value);
        value = le16toh(value);
    }

    // unsigned 16-bit int
    static void deserialize(IStream &stream, uint16_t &value) {
        stream.read(sizeof(value), &value);
        value = le16toh(value);
    }

    // 32-bit int
    static void deserialize(IStream &stream, int32_t &value) {
        stream.read(sizeof(value), &value);
        value = le32toh(value);
    }

    // unsigned 32-bit int
    static void deserialize(IStream &stream, uint32_t &value) {
        stream.read(sizeof(value), &value);
        value = le32toh(value);
    }

    // bool
    static void deserialize(IStream &stream, bool &value) {
        uint8_t bVal;
        stream.read(sizeof(bVal), &bVal);
        value = static_cast<bool>(bVal);
    }

    // PrtocolOpCode
    static void deserialize(IStream &stream, ProtocolOpCode &value) {
        stream.read(sizeof(value), &value);
    }

    // std::string
    static void deserialize(IStream &stream, std::string &str) {
        uint32_t length;
        stream.read(sizeof(length), &length);
        length = le32toh(length);
        str.resize(length);
        stream.read(length, &str[0]);
    }
    static void deserialize(IStream &stream, int length, std::string &str) {
        str.resize(length);
        stream.read(length, &str[0]);
    }

    // std::vector
    template<typename T>
    static void deserialize(IStream &stream, std::vector<T> &vec) {
        uint32_t length;
        stream.read(sizeof(length), &length);
        length = le32toh(length);
        for (uint32_t i = 0; i < length; ++i) {
            T obj;
            deserialize(stream, obj);
            vec.push_back(std::move(obj));
        }
    }
}; // struct ProtocolDeserialization
} // namespace Cynara

#endif // SRC_COMMON_PROTOCOL_PROTOCOLSERIALIZATION_H_
