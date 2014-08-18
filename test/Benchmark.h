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
/*
 * @file        Benchmark.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       A generic benchmark
 */

#ifndef TEST_BENCHMARK_H_
#define TEST_BENCHMARK_H_

#include <chrono>

namespace Cynara {

namespace Benchmark {

typedef std::function<void(void)> Function;

template <typename Precision>
Precision measure(Function fn) {
    using std::chrono::high_resolution_clock;

    high_resolution_clock::time_point t0 = high_resolution_clock::now();
    fn();
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    return std::chrono::duration_cast<Precision>(t1 - t0);
}

}  // namespace Benchmark

}  // namespace Cynara


#endif /* TEST_BENCHMARK_H_ */
