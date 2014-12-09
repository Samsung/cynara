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
 * @file        src/cyad/BaseDispatcherIO.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Aggregates I/O methods (base)
 */

#ifndef SRC_CYAD_BASEDISPATCHERIO_H_
#define SRC_CYAD_BASEDISPATCHERIO_H_

#include <istream>
#include <memory>
#include <ostream>
#include <string>

namespace Cynara {

class BaseDispatcherIO {
public:
    typedef std::string Filename;
    typedef std::shared_ptr<std::istream> InputStreamPtr;

    BaseDispatcherIO() = default;
    virtual ~BaseDispatcherIO() {}

    virtual InputStreamPtr openFile(const Filename &filename) = 0;
    virtual std::ostream &cout(void) = 0;
    virtual std::istream &cin(void) = 0;
    virtual std::ostream &cerr(void) = 0;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_BASEDISPATCHERIO_H_ */
