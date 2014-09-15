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

// Serializable interface
class ISerializable {
public:
    /*    ISerializable(){};
     *    ISerializable(IStream&){}; */
    virtual void serialize(IStream &) const = 0;
    virtual ~ISerializable() {};
};

struct ProtocolSerialization {

    // ISerializable objects
    static void serialize(IStream &stream, const ISerializable &object) {
        object.serialize(stream);
    }
    static void serialize(IStream &stream, const ISerializable * const object) {
        object->serialize(stream);
    }

    // char
    static void serialize(IStream &stream, const char value) {
        stream.write(sizeof(value), &value);
    }
    static void serialize(IStream &stream, const char * const value) {
        stream.write(sizeof(*value), value);
    }

    // unsigned char
    static void serialize(IStream &stream, const unsigned char value) {
        stream.write(sizeof(value), &value);
    }
    static void serialize(IStream &stream, const unsigned char * const value) {
        stream.write(sizeof(*value), value);
    }

    // unsigned 16-bit int
    static void serialize(IStream &stream, const uint16_t value) {
        uint16_t _value = htole16(value);
        stream.write(sizeof(value), &_value);
    }
    static void serialize(IStream &stream, const uint16_t * const value) {
        uint16_t _value = htole16(*value);
        stream.write(sizeof(*value), &_value);
    }

    // 16-bit int
    static void serialize(IStream &stream, const int16_t value) {
        int16_t _value = htole16(value);
        stream.write(sizeof(value), &_value);
    }
    static void serialize(IStream &stream, const int16_t * const value) {
        int16_t _value = htole16(*value);
        stream.write(sizeof(*value), &_value);
    }

    // unsigned 32-bit int
    static void serialize(IStream &stream, const uint32_t value) {
        uint32_t _value = htole32(value);
        stream.write(sizeof(value), &_value);
    }
    static void serialize(IStream &stream, const uint32_t * const value) {
        uint32_t _value = htole32(*value);
        stream.write(sizeof(*value), &_value);
    }

    // 32-bit int
    static void serialize(IStream &stream, const int32_t value) {
        int32_t _value = htole32(value);
        stream.write(sizeof(value), &_value);
    }
    static void serialize(IStream &stream, const int32_t * const value) {
        int32_t _value = htole32(*value);
        stream.write(sizeof(*value), &_value);
    }

    // bool
    static void serialize(IStream &stream, const bool value) {
        stream.write(sizeof(value), &value);
    }
    static void serialize(IStream &stream, const bool * const value) {
        stream.write(sizeof(*value), value);
    }

    // time_t
    static void serialize(IStream &stream, const time_t value) {
        stream.write(sizeof(value), &value);
    }
    static void serialize(IStream &stream, const time_t * const value) {
        stream.write(sizeof(*value), value);
    }

    // ProtocolOpCode
    static void serialize(IStream &stream, const ProtocolOpCode value) {
        stream.write(sizeof(value), &value);
    }
    static void serialize(IStream &stream, const ProtocolOpCode * const value) {
        stream.write(sizeof(*value), value);
    }

    // std::string
    static void serialize(IStream &stream, const std::string &str) {
        int length = str.size();
        stream.write(sizeof(length), &length);
        stream.write(length, str.c_str());
    }
    static void serialize(IStream &stream, const std::string * const str) {
        int length = str->size();
        stream.write(sizeof(length), &length);
        stream.write(length, str->c_str());
    }
    static void serializeNoSize(IStream &stream, const std::string &str) {
        int length = str.size();
        stream.write(length, str.c_str());
    }
    static void serializeNoSize(IStream &stream, const std::string * const str) {
        int length = str->size();
        stream.write(length, str->c_str());
    }

    // STL templates

    // std::list
    template<typename T>
    static void serialize(IStream &stream, const std::list<T> &list) {
        int length = list.size();
        stream.write(sizeof(length), &length);
        for (typename std::list<T>::const_iterator list_iter = list.begin();
                list_iter != list.end(); list_iter++) {
            serialize(stream, *list_iter);
        }
    }
    template<typename T>
    static void serialize(IStream &stream, const std::list<T> * const list) {
        serialize(stream, *list);
    }

    // std::vector
    template<typename T>
    static void serialize(IStream &stream, const std::vector<T> &vec) {
        int length = vec.size();
        stream.write(sizeof(length), &length);
        for (typename std::vector<T>::const_iterator vec_iter = vec.begin();
                vec_iter != vec.end(); vec_iter++) {
            serialize(stream, *vec_iter);
        }
    }
    template<typename T>
    static void serialize(IStream &stream, const std::vector<T> * const vec) {
        serialize(stream, *vec);
    }

    // std::pair
    template<typename A, typename B>
    static void serialize(IStream &stream, const std::pair<A, B> &p) {
        serialize(stream, p.first);
        serialize(stream, p.second);
    }
    template<typename A, typename B>
    static void serialize(IStream &stream, const std::pair<A, B> * const p) {
        serialize(stream, *p);
    }

    // std::map
    template<typename K, typename T>
    static void serialize(IStream &stream, const std::map<K, T> &map) {
        int length = map.size();
        stream.write(sizeof(length), &length);
        typename std::map<K, T>::const_iterator it;
        for (it = map.begin(); it != map.end(); ++it) {
            serialize(stream, (*it).first);
            serialize(stream, (*it).second);
        }
    }
    template<typename K, typename T>
    static void serialize(IStream &stream, const std::map<K, T> * const map) {
        serialize(stream, *map);
    }

    // std::unique_ptr
    template<typename T>
    static void serialize(IStream &stream, const std::unique_ptr<T> &p) {
        serialize(stream, *p);
    }
}; // struct ProtocolSerialization

struct ProtocolDeserialization {

    // ISerializable objects
    // T instead of ISerializable is needed to call proper constructor
    template<typename T>
    static void deserialize(IStream &stream, T &object) {
        object = T(stream);
    }
    template<typename T>
    static void deserialize(IStream &stream, T *&object) {
        object = new T(stream);
    }

    // char
    static void deserialize(IStream &stream, char &value) {
        stream.read(sizeof(value), &value);
    }
    static void deserialize(IStream &stream, char *&value) {
        value = new char;
        stream.read(sizeof(*value), value);
    }

    // unsigned char
    static void deserialize(IStream &stream, unsigned char &value) {
        stream.read(sizeof(value), &value);
    }
    static void deserialize(IStream &stream, unsigned char *&value) {
        value = new unsigned char;
        stream.read(sizeof(*value), value);
    }

    // 16-bit int
    static void deserialize(IStream &stream, int16_t &value) {
        stream.read(sizeof(value), &value);
        value = le16toh(value);
    }
    static void deserialize(IStream &stream, int16_t *&value) {
        value = new int16_t;
        stream.read(sizeof(*value), value);
        value = le16toh(value);
    }

    // unsigned 16-bit int
    static void deserialize(IStream &stream, uint16_t &value) {
        stream.read(sizeof(value), &value);
        value = le16toh(value);
    }
    static void deserialize(IStream &stream, uint16_t *&value) {
        value = new uint16_t;
        stream.read(sizeof(*value), value);
        value = le16toh(value);
    }

    // 32-bit int
    static void deserialize(IStream &stream, int32_t &value) {
        stream.read(sizeof(value), &value);
        value = le32toh(value);
    }
    static void deserialize(IStream &stream, int32_t *&value) {
        value = new int32_t;
        stream.read(sizeof(*value), value);
        value = le32toh(value);
    }

    // unsigned 32-bit int
    static void deserialize(IStream &stream, uint32_t &value) {
        stream.read(sizeof(value), &value);
        value = le32toh(value);
    }
    static void deserialize(IStream &stream, uint32_t *&value) {
        value = new uint32_t;
        stream.read(sizeof(*value), value);
        value = le32toh(value);
    }

    // bool
    static void deserialize(IStream &stream, bool &value) {
        stream.read(sizeof(value), &value);
    }
    static void deserialize(IStream &stream, bool *&value) {
        value = new bool;
        stream.read(sizeof(*value), value);
    }

    // time_t
    static void deserialize(IStream &stream, time_t &value) {
        stream.read(sizeof(value), &value);
    }
    static void deserialize(IStream &stream, time_t *&value) {
        value = new time_t;
        stream.read(sizeof(*value), value);
    }

    // PrtocolOpCode
    static void deserialize(IStream &stream, ProtocolOpCode &value) {
        stream.read(sizeof(value), &value);
    }
    static void deserialize(IStream &stream, ProtocolOpCode *&value) {
        value = new ProtocolOpCode;
        stream.read(sizeof(*value), value);
    }

    // std::string
    static void deserialize(IStream &stream, std::string &str) {
        int length;
        stream.read(sizeof(length), &length);
        str.resize(length);
        stream.read(length, &str[0]);
    }
    static void deserialize(IStream &stream, std::string *&str) {
        int length;
        stream.read(sizeof(length), &length);
        str = new std::string(length, '\0');
        stream.read(length, &str[0]);
    }
    static void deserialize(IStream &stream, int length, std::string &str) {
        str.resize(length);
        stream.read(length, &str[0]);
    }
    static void deserialize(IStream &stream, int length, std::string *&str) {
        str = new std::string(length, '\0');
        stream.read(length, &str[0]);
    }

    // STL templates

    // std::list
    template<typename T>
    static void deserialize(IStream &stream, std::list<T> &list) {
        int length;
        stream.read(sizeof(length), &length);
        for (int i = 0; i < length; ++i) {
            T obj;
            deserialize(stream, obj);
            list.push_back(std::move(obj));
        }
    }
    template<typename T>
    static void deserialize(IStream &stream, std::list<T> *&list) {
        list = new std::list<T>;
        deserialize(stream, *list);
    }

    // std::vector
    template<typename T>
    static void deserialize(IStream &stream, std::vector<T> &vec) {
        int length;
        stream.read(sizeof(length), &length);
        for (int i = 0; i < length; ++i) {
            T obj;
            deserialize(stream, obj);
            vec.push_back(std::move(obj));
        }
    }
    template<typename T>
    static void deserialize(IStream &stream, std::vector<T> *&vec) {
        vec = new std::vector<T>;
        deserialize(stream, *vec);
    }

    // std::pair
    template<typename A, typename B>
    static void deserialize(IStream &stream, std::pair<A, B> &p) {
        deserialize(stream, p.first);
        deserialize(stream, p.second);
    }
    template<typename A, typename B>
    static void deserialize(IStream &stream, std::pair<A, B> *&p) {
        p = new std::pair<A, B>;
        deserialize(stream, *p);
    }

    // std::map
    template<typename K, typename T>
    static void deserialize(IStream &stream, std::map<K, T> &map) {
        int length;
        stream.read(sizeof(length), &length);
        for (int i = 0; i < length; ++i) {
            K key;
            T obj;
            deserialize(stream, key);
            deserialize(stream, obj);
            map[key] = std::move(obj);
        }
    }
    template<typename K, typename T>
    static void deserialize(IStream &stream, std::map<K, T> *&map) {
        map = new std::map<K, T>;
        deserialize(stream, *map);
    }
};
// struct ProtocolDeserialization
}// namespace Cynara

#endif // SRC_COMMON_PROTOCOL_PROTOCOLSERIALIZATION_H_
