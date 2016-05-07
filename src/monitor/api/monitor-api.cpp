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
 * @file        src/monitor/api/monitor-api.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-monitor API
 */

#include <new>

#include <api/ApiInterface.h>
#include <common.h>
#include <configuration/MonitorConfiguration.h>
#include <exceptions/TryCatch.h>
#include <types/ProtocolFields.h>
#include <cynara-error.h>
#include <cynara-limits.h>
#include <cynara-monitor.h>
#include <log/log.h>
#include <logic/Logic.h>
#include <utils/Lists.h>

struct cynara_monitor {
    Cynara::ApiInterface *impl;

    cynara_monitor(Cynara::ApiInterface *_impl) : impl(_impl) {}

    ~cynara_monitor() {
        delete impl;
    }
};

struct cynara_monitor_configuration {
    Cynara::MonitorConfiguration *impl;

    cynara_monitor_configuration(Cynara::MonitorConfiguration *_impl) : impl(_impl) {}

    ~cynara_monitor_configuration() {
        delete impl;
    }
};

struct cynara_monitor_entry {
    Cynara::MonitorEntry m_monitorEntry;
};

CYNARA_API
int cynara_monitor_configuration_create(cynara_monitor_configuration **pp_conf) {
    if (!pp_conf)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        Cynara::MonitorConfigurationUniquePtr ptr(new Cynara::MonitorConfiguration());
        *pp_conf = new cynara_monitor_configuration(ptr.get());
        ptr.release();
        LOGD("Cynara monitor configuration initialized");
        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
void cynara_monitor_configuration_destroy(cynara_monitor_configuration *p_conf) {
    delete p_conf;
}

CYNARA_API
int cynara_monitor_configuration_set_buffer_size(cynara_monitor_configuration *p_conf,
                                                 size_t buffer_size) {
    if (!p_conf || !p_conf->impl)
        return CYNARA_API_INVALID_PARAM;
    if (buffer_size > CYNARA_MAX_MONITOR_BUFFER_SIZE) {
        return CYNARA_API_INVALID_PARAM;
    }

    return Cynara::tryCatch([&]() {
        p_conf->impl->setBufferSize(buffer_size);
        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_monitor_initialize(cynara_monitor **pp_cynara_monitor,
                              const cynara_monitor_configuration *p_conf) {
    if (!pp_cynara_monitor)
        return CYNARA_API_INVALID_PARAM;

    init_log();

    return Cynara::tryCatch([&]() {
        Cynara::LogicUniquePtr ptr;
        if (p_conf && p_conf->impl) {
            ptr.reset(new Cynara::Logic(*(p_conf->impl)));
        } else {
            ptr.reset(new Cynara::Logic());
        }

        int ret = ptr->init();
        if (ret != CYNARA_API_SUCCESS) {
            LOGE("Couldn't initialize cynara monitor");
            return ret;
        }
        *pp_cynara_monitor = new cynara_monitor(ptr.get());
        ptr.release();

        LOGD("Cynara client initialized");

        return CYNARA_API_SUCCESS;
    });
}

CYNARA_API
int cynara_monitor_finish(cynara_monitor *p_cynara_monitor) {
    if (p_cynara_monitor && p_cynara_monitor->impl) {
        p_cynara_monitor->impl->notifyFinish();
    }
    delete p_cynara_monitor;
    return CYNARA_API_SUCCESS;
}

CYNARA_API
int cynara_monitor_entries_get(cynara_monitor *p_cynara_monitor,
                               cynara_monitor_entry ***monitor_entries) {

    if (!p_cynara_monitor || !p_cynara_monitor->impl)
        return CYNARA_API_INVALID_PARAM;
    if (!monitor_entries)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        std::vector<Cynara::MonitorEntry> entriesVector;

        auto ret = p_cynara_monitor->impl->entriesGet(entriesVector);

        if (ret != CYNARA_API_SUCCESS)
            return ret;

        return Cynara::createNullTerminatedArray<Cynara::MonitorEntry, cynara_monitor_entry>(
                entriesVector, monitor_entries,
                [] (const Cynara::MonitorEntry &from, cynara_monitor_entry *&to) -> int {
                    to = new cynara_monitor_entry{from};
                    return CYNARA_API_SUCCESS;
                }
        );
    });
}

CYNARA_API
int cynara_monitor_entries_flush(cynara_monitor *p_cynara_monitor) {
    if (!p_cynara_monitor || !p_cynara_monitor->impl)
        return CYNARA_API_INVALID_PARAM;

    return Cynara::tryCatch([&]() {
        return p_cynara_monitor->impl->entriesFlush();
    });
}

CYNARA_API
void cynara_monitor_entries_free(cynara_monitor_entry **monitor_entries) {
    Cynara::freeNullTerminatedList(monitor_entries);
}

CYNARA_API
const char *cynara_monitor_entry_get_client(const cynara_monitor_entry *monitor_entry) {
    if (!monitor_entry) {
        LOGW("NULL passed to %s", __FUNCTION__);
        return nullptr;
    }
    return monitor_entry->m_monitorEntry.key().client().toString().c_str();
}

CYNARA_API
const char *cynara_monitor_entry_get_user(const cynara_monitor_entry *monitor_entry) {
    if (!monitor_entry) {
        LOGW("NULL passed to %s", __FUNCTION__);
        return nullptr;
    }
    return monitor_entry->m_monitorEntry.key().user().toString().c_str();
}

CYNARA_API
const char *cynara_monitor_entry_get_privilege(const cynara_monitor_entry *monitor_entry) {
    if (!monitor_entry) {
        LOGW("NULL passed to %s", __FUNCTION__);
        return nullptr;
    }
    return monitor_entry->m_monitorEntry.key().privilege().toString().c_str();
}

CYNARA_API
int cynara_monitor_entry_get_result(const cynara_monitor_entry *monitor_entry) {
    if (!monitor_entry) {
        LOGW("NULL passed to %s", __FUNCTION__);
        return CYNARA_API_INVALID_PARAM;
    }
    return monitor_entry->m_monitorEntry.result();
}

CYNARA_API
const struct timespec *cynara_monitor_entry_get_timestamp(
        const cynara_monitor_entry *monitor_entry) {
    if (!monitor_entry) {
        LOGW("NULL passed to %s", __FUNCTION__);
        return nullptr;
    }
    return &monitor_entry->m_monitorEntry.timestamp();
}

static void freeElem(struct cynara_monitor_entry *policyPtr) {
    delete policyPtr;
}
