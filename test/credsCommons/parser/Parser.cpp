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
 * @file        test/credsCommons/parser/Parser.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Tests of internal implementation of credential commons functions
 */

#include <exception>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

#include "FakeCredsCommonsInnerBackend.h"

#include <cynara-creds-commons.h>
#include <CredsCommonsInner.h>

/* Tests */
using namespace Cynara;
using ::testing::_;
using ::testing::Assign;
using ::testing::AtMost;
using ::testing::Eq;
using ::testing::Between;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::Throw;


#define NOT_A_METHOD_CODE -13197 //number which is not a method code

/*** Test for getKeyAndValue() ***/

struct getKeyAndValueTestEntry {
    std::string testString;
    std::locale loc;
    bool expectedResult;
    std::string key;
    std::string value;
};

TEST(parser, getKeyAndValue) {

    static std::vector<struct getKeyAndValueTestEntry>  getKeyAndValueTestEntries;
    // test cases initialization key and value are checked only if expectedResult is true
    getKeyAndValueTestEntries.push_back({"", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"#", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"\n", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"aa", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=#", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=\n", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"\n=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"==", std::locale("en_US.UTF8"), false, "", "="});
    getKeyAndValueTestEntries.push_back({"#=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"aa", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"##", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=b", std::locale("en_US.UTF8"), true, "a", "b"});
    getKeyAndValueTestEntries.push_back({"=aa", std::locale("en_US.UTF8"), false, "", "aa"});
    getKeyAndValueTestEntries.push_back({"aa=", std::locale("en_US.UTF8"), true, "aa", ""});
    getKeyAndValueTestEntries.push_back({"#a=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=#", std::locale("en_US.UTF8"), true, "a", ""});
    getKeyAndValueTestEntries.push_back({"=#a", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"==a", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=a=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=#\nb", std::locale("en_US.UTF8"), true, "a", ""});
    getKeyAndValueTestEntries.push_back({"a=#b\n", std::locale("en_US.UTF8"), true, "a", ""});
    getKeyAndValueTestEntries.push_back({"a\nb", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"\na=b", std::locale("en_US.UTF8"), true, "a", "b"});
    getKeyAndValueTestEntries.push_back({"a=#\nb", std::locale("en_US.UTF8"), true, "a", ""});
    getKeyAndValueTestEntries.push_back({"=a\nb", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=\nb=c#", std::locale("en_US.UTF8"), true, "a", ""});
    getKeyAndValueTestEntries.push_back({"=a\nb=c#", std::locale("en_US.UTF8"), true, "b", "c"});
    getKeyAndValueTestEntries.push_back({"aa\nkey=value", std::locale("en_US.UTF8"), true, "key",
        "value"});
    getKeyAndValueTestEntries.push_back({" key with spaces  = value with spaces #\n",
                                         std::locale("en_US.UTF8"), true, "key with spaces",
                                         "value with spaces"});
    getKeyAndValueTestEntries.push_back({"VeryLongKey1111111111111111111111111111111111111111111"
                                         "111111111111111111111111111111111111111111111111111111"
                                         "111111111 = 1\nnoImportant", std::locale("en_US.UTF8"),
                                         true,
                                         "verylongkey1111111111111111111111111111111111111111111"
                                         "111111111111111111111111111111111111111111111111111111"
                                         "111111111", "1"});
    getKeyAndValueTestEntries.push_back({"key=value", std::locale("en_US.UTF8"), true, "key",
                                         "value"});
    getKeyAndValueTestEntries.push_back({"CAPSON=CaPSon", std::locale("en_US.UTF8"), true,
                                         "capson", "capson"});
    getKeyAndValueTestEntries.push_back({" soMe_spacEs_ = vaLue# ", std::locale("en_US.UTF8"),
    	                                   true, "some_spaces_", "value"});
    int i = 0;
    for (auto it = getKeyAndValueTestEntries.begin(); it != getKeyAndValueTestEntries.end(); ++it)
    {
        i++;
        std::string keyAtStart("predefinedKey["+it->testString+"]");
        std::string valueAtStart("prdefinedValue["+it->testString+"]");
        std::string key(keyAtStart);
        std::string value(valueAtStart);
        std::stringstream ss;
        ss << it->testString;

        EXPECT_EQ(it->expectedResult, credsBackend.getKeyAndValue(ss, it->loc, key, value))
        << "Result code in case no: "<< i << " [" + it->testString + "] was wrong." <<std::endl;
        if (it->expectedResult) {
            //key and value shall be set if getKeyAndValue returns true
            EXPECT_EQ(it->key, key) << "A in case no# " << i <<std::endl;
            EXPECT_EQ(it->value, value) << "B in case no# " << i <<std::endl;
        } else {
            //key and value should not be changed if getKeyAndValue returns false
            EXPECT_EQ(key, keyAtStart) << "C in case no# " << i <<std::endl;
            EXPECT_EQ(value, valueAtStart) << "D in case no# " << i <<std::endl;
        }
    }
}

/*** Tests for interpretValue() ***/

TEST(parser, interpretValue_OK) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = false;
    EXPECT_TRUE(credsBackend.interpretValue(clientCredsMap, method, "smack", occurred));
    EXPECT_TRUE(occurred);
    EXPECT_EQ(CLIENT_METHOD_SMACK, method);
}

TEST(parser, interpretValueKeyNotInMap1) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = false;
    EXPECT_FALSE(
              credsBackend.interpretValue(clientCredsMap, method, "NeitherSmackNorPid", occurred));
    EXPECT_TRUE(occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretValueKeyNotInMap2) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = true;
    EXPECT_FALSE(
              credsBackend.interpretValue(clientCredsMap, method, "NeitherSmackNorPid", occurred));
    EXPECT_TRUE(occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretValueKeyAppearsAgain) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = true;
    EXPECT_FALSE(
              credsBackend.interpretValue(clientCredsMap, method, "smack", occurred));
    EXPECT_TRUE(occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretValueKeyEmptyKey) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = true;
    EXPECT_FALSE(
              credsBackend.interpretValue(clientCredsMap, method, "", occurred));
    EXPECT_TRUE(occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretValueEmptyMap1) {
    static const CredentialsMap clientCredsMap{};
    int method = NOT_A_METHOD_CODE;
    bool occurred = false;
    EXPECT_FALSE(
              credsBackend.interpretValue(clientCredsMap, method, "Anything", occurred));
    EXPECT_TRUE(occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretValueEmptyMap2) {
    static const CredentialsMap clientCredsMap{};
    int method = NOT_A_METHOD_CODE;
    bool occurred = true;
    EXPECT_FALSE(
              credsBackend.interpretValue(clientCredsMap, method, "Anything", occurred));
    EXPECT_TRUE(occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

/*** Tests for getMethodFromConfigurationFileOK ***/

TEST(parser, getMethodFromConfigurationFileOK2Lines) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("smack"), Return(true)))
    .WillOnce(Return(false));
    EXPECT_CALL(fakeBackend, interpretValue(_, _,"smack", Eq(false)))
    .WillOnce(DoAll(SetArgReferee<1>(CLIENT_METHOD_SMACK), SetArgReferee<3>(true),
              Return(true)));

    EXPECT_EQ(CYNARA_API_SUCCESS,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(CLIENT_METHOD_SMACK, method);
}

TEST(parser, getMethodFromConfigurationFileOK3Lines) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("uid"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("smack"), Return(true)))
    .WillOnce(Return(false));
    EXPECT_CALL(fakeBackend, interpretValue(_, _,"smack", Eq(false)))
    .WillOnce(DoAll(SetArgReferee<1>(CLIENT_METHOD_SMACK), SetArgReferee<3>(true),
              Return(true)));

    EXPECT_EQ(CYNARA_API_SUCCESS,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(CLIENT_METHOD_SMACK, method);
}

TEST(parser, getMethodFromConfigurationFileBadNoEntry) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .Times(4)
    .WillOnce(DoAll(SetArgReferee<2>("A"), SetArgReferee<3>("smack"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("B"), SetArgReferee<3>("smack"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("someWrongValue"), Return(true)))
    .WillOnce(Return(false));
    //user default is not in the file (contrary to A,B, client_default)
    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "user_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileBadValueInEntry) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .Times(3)
    .WillOnce(DoAll(SetArgReferee<2>("someOtherKey"), SetArgReferee<3>("smack"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("smack"), Return(true)))
    //the someWrongValue does not matter this time
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("someWrongValue"), Return(true)));
    EXPECT_CALL(fakeBackend, interpretValue(clientCredsMap, _, "someWrongValue", Eq(false)))
    .WillOnce(Return(false));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileBadTwoEntries) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .Times(3)
    .WillOnce(DoAll(SetArgReferee<2>("someOtherKey"), SetArgReferee<3>("smack"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("smack"), Return(true)))
    //the someWrongValue does not matter this time
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("someWrongValue"), Return(true)));
    EXPECT_CALL(fakeBackend, interpretValue(_, _, "smack", Eq(false)))
    .WillOnce(DoAll(SetArgReferee<1>(CLIENT_METHOD_SMACK), SetArgReferee<3>(true),
                          Return(true)));
    EXPECT_CALL(fakeBackend, interpretValue(clientCredsMap, _, "someWrongValue", Eq(true)))
    .WillOnce(Return(false));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileManyStrangeKeyAndValues) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("key1"), SetArgReferee<3>("value1"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key2"), SetArgReferee<3>("value2"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key3"), SetArgReferee<3>("value3"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key4"), SetArgReferee<3>("value4"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>(""), SetArgReferee<3>(""), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("pid"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>(""), SetArgReferee<3>(""), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key5"), SetArgReferee<3>("value5"), Return(true)))
    .WillOnce(Return(false));
    EXPECT_EQ(CYNARA_API_SUCCESS,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(CLIENT_METHOD_PID, method);
}

TEST(parser, getMethodFromConfigurationFileFoundTheKeyAgain) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("smack"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("gid"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("smack"),
                    Return(true)));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileTheKeyNotFound) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("something_default"), SetArgReferee<3>("smack"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("gid"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("other_default"), SetArgReferee<3>("smack"),
                    Return(true)))
    .WillOnce(Return(false));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileFoundTheKeyWithWrongValue) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .Times(2)
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("gid"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("noSmackNoPid"),
                    Return(true)));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}
