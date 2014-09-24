/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/client-async/callback/ResponseCallback.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of ResponseCallback class
 */

#include "ResponseCallback.h"

namespace Cynara {

ResponseCallback::ResponseCallback(cynara_response_callback callback, void *userData)
    : m_callback(callback), m_userData(userData) {
}

void ResponseCallback::onAnswer(cynara_check_id checkId, int response) const {
    if (!m_callback)
        return;
    m_callback(checkId, cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER, response, m_userData);
}

void ResponseCallback::onCancel(cynara_check_id checkId) const {
    if (!m_callback)
        return;
    m_callback(checkId, cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL, 0, m_userData);
}

void ResponseCallback::onFinish(cynara_check_id checkId) const {
    if (!m_callback)
        return;
    m_callback(checkId, cynara_async_call_cause::CYNARA_CALL_CAUSE_FINISH, 0, m_userData);
}

void ResponseCallback::onDisconnected(cynara_check_id checkId) const {
    if (!m_callback)
        return;
    m_callback(checkId, cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE, 0,
               m_userData);
}

} // namespace Cynara
