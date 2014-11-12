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
 * @file        src/client-async/callback/ResponseCallback.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains declaration of ResponseCallback class
 */

#ifndef SRC_CLIENT_ASYNC_CALLBACK_RESPONSECALLBACK_H_
#define SRC_CLIENT_ASYNC_CALLBACK_RESPONSECALLBACK_H_

#include <cynara-client-async.h>

namespace Cynara {

class ResponseCallback {
public:
    ResponseCallback(cynara_response_callback callback, void *userData);
    ResponseCallback(const ResponseCallback&) = default;
    ResponseCallback(ResponseCallback &&other);
    ~ResponseCallback() {};

    void onAnswer(cynara_check_id checkId, int response) const;
    void onCancel(cynara_check_id checkId) const;
    void onFinish(cynara_check_id checkId) const;
    void onDisconnected(cynara_check_id checkId) const;

private:
    cynara_response_callback m_callback;
    void *m_userData;
};

} // namespace Cynara

#endif /* SRC_CLIENT_ASYNC_CALLBACK_RESPONSECALLBACK_H_ */
