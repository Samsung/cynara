/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/helpers/creds-self/creds-self.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-creds-self API
 */

#include <cstring>
#include <string>
#include <sys/smack.h>
#include <sys/types.h>
#include <unistd.h>

#include <attributes/attributes.h>
#include <exceptions/TryCatch.h>
#include <log/log.h>

#include <cynara-creds-self.h>

namespace {
    int getSelfSmackLabel(char **client) {
        return Cynara::tryCatch([&client](){
            ssize_t ret = smack_new_label_from_self(client);
            if (ret < 0) {
                LOGE("smack_new_label_from_self failed with %zd", ret);
                return CYNARA_API_UNKNOWN_ERROR;
            }
            return CYNARA_API_SUCCESS;
        });
    }

    int copyStr(char **client, const std::string &str) {
        char *clientTmp = strdup(str.c_str());
        if (!clientTmp) {
            LOGE("strdup failed");
            return CYNARA_API_OUT_OF_MEMORY;
        }
        *client = clientTmp;
        return CYNARA_API_SUCCESS;
    }

    int getSelfPid(char **client) {
        return Cynara::tryCatch([&client](){
            return copyStr(client, std::to_string(getpid()));
        });
    }

    int getSelfUid(char **client) {
        return Cynara::tryCatch([&client](){
            return copyStr(client, std::to_string(getuid()));
        });
    }

    int getSelfGid(char **client) {
        return Cynara::tryCatch([&client](){
            return copyStr(client, std::to_string(getgid()));
        });
    }
} // namespace anonymous

CYNARA_API
int cynara_creds_self_get_client(enum cynara_client_creds method, char **client) {
    if (client == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (method == cynara_client_creds::CLIENT_METHOD_DEFAULT) {
        int ret = cynara_creds_get_default_client_method(&method);
        if (ret != CYNARA_API_SUCCESS)
            return getSelfSmackLabel(client);
    }

    switch (method) {
        case cynara_client_creds::CLIENT_METHOD_SMACK:
            return getSelfSmackLabel(client);
        case cynara_client_creds::CLIENT_METHOD_PID:
            return getSelfPid(client);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

CYNARA_API
int cynara_creds_self_get_user(enum cynara_user_creds method, char **user) {
    if (user == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (method == cynara_user_creds::USER_METHOD_DEFAULT) {
        int ret = cynara_creds_get_default_user_method(&method);
        if (ret != CYNARA_API_SUCCESS)
            return ret;
    }

    switch (method) {
        case cynara_user_creds::USER_METHOD_UID:
            return getSelfUid(user);
        case cynara_user_creds::USER_METHOD_GID:
            return getSelfGid(user);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

