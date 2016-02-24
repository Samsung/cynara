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
 * @file        src/external/md5wrapper.cpp
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @version     1.0
 * @brief       Wrapper for md5.h
 */
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <md5wrapper.h>
#include <md5.h>

namespace Cynara {

const std::string generateMD5(const std::string &data) {
    MD5Context context;
    std::vector<u_int8_t> result(MD5_DIGEST_LENGTH);
    MD5Init(&context);
    MD5Update(&context, reinterpret_cast<const u_int8_t *>(data.data()), data.size());
    MD5Final(result.data(), &context);

    std::stringstream output;
    output << std::setfill('0') << std::hex;

    for (auto e : result)
        output << std::setw(2) << static_cast<int>(e);

    return output.str();
}

} // namespace Cynara

