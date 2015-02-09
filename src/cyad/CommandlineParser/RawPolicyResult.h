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
 * @file        src/cyad/CommandlineParser/RawPolicyResult.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       An abstraction of raw PolicyType
 */

#ifndef SRC_CYAD_COMMANDLINEPARSER_RAWPOLICYRESULT_H_
#define SRC_CYAD_COMMANDLINEPARSER_RAWPOLICYRESULT_H_

namespace Cynara {

class RawPolicyResult {
public:
    RawPolicyResult(const std::string &type, const std::string &metadata)
        : m_type(type), m_metadata(metadata) {}
    ~RawPolicyResult() {}

    const std::string &policyType(void) const {
        return m_type;
    }
    const std::string &metadata(void) const {
        return m_metadata;
    }

private:
    std::string m_type;
    std::string m_metadata;

};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_RAWPOLICYRESULT_H_ */
