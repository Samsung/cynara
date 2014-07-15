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
 * @file        log.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines logging utilities.
 */

#ifndef CYNARA_COMMON_LOG_H
#define CYNARA_COMMON_LOG_H

#ifndef CYNARA_NO_LOGS
#include <systemd/sd-journal.h>
#endif

extern int __log_level;

#ifndef CYNARA_NO_LOGS
    #define __LOG(LEVEL, ...) \
        do { \
            if(LEVEL <= __log_level) \
                sd_journal_print(LEVEL, __VA_ARGS__); \
        } while (0)
#else
    #define __LOG(LEVEL, ...)
#endif

#define LEGM(...)  __LOG(LOG_EMERG, __VA_ARGS__)   /* system is unusable */
#define LOGA(...)  __LOG(LOG_ALERT, __VA_ARGS__)   /* action must be taken immediately */
#define LOGC(...)  __LOG(LOG_CRIT, __VA_ARGS__)    /* critical conditions */
#define LOGE(...)  __LOG(LOG_ERR, __VA_ARGS__)     /* error conditions */
#define LOGW(...)  __LOG(LOG_WARNING, __VA_ARGS__) /* warning conditions */
#define LOGN(...)  __LOG(LOG_NOTICE, __VA_ARGS__)  /* normal but significant condition */
#define LOGI(...)  __LOG(LOG_INFO, __VA_ARGS__)    /* informational */
#define LOGD(...)  __LOG(LOG_DEBUG, __VA_ARGS__)   /* debug-level messages */

void init_log(void);

#endif /* CYNARA_COMMON_LOG_H */
