/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
/*
 * @file        Backtrace.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Header file for backtrace utility class.
 */

#ifndef SRC_COMMON_LOG_BACKTRACE_H_
#define SRC_COMMON_LOG_BACKTRACE_H_

#if defined(BUILD_TYPE_DEBUG) && !defined(CYNARA_NO_LOGS)
#include <elfutils/libdwfl.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif
#include <string>

namespace Cynara {

class Backtrace {
public:
#if defined(BUILD_TYPE_DEBUG) && !defined(CYNARA_NO_LOGS)
    static const std::string getBacktrace(void);
#else
    static const std::string getBacktrace(void) {
        return "";
    }
#endif

private:
    static Backtrace &getInstance(void);

    Backtrace();
    Backtrace(Backtrace const &) = delete;
    ~Backtrace();

    void operator=(Backtrace const &) = delete;

    void init(void);
    const std::string buildBacktrace(void);

#if defined(BUILD_TYPE_DEBUG) && !defined(CYNARA_NO_LOGS)
    void getSourceInfo(unw_word_t address, std::string &fileName, int &lineNumber);

    Dwfl *m_dwfl;
    static const Dwfl_Callbacks m_callbacks;
#endif
};

} /* namespace Cynara */

#endif /* SRC_COMMON_LOG_BACKTRACE_H_ */
