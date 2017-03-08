/*
 *  Copyright (c) 2016-2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/monitor/notify/FdNotifyObject.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of cynara's fd notification object
 */

#ifndef SRC_MONITOR_NOTIFY_FDNOTIFYOBJECT_H_
#define SRC_MONITOR_NOTIFY_FDNOTIFYOBJECT_H_

namespace Cynara {

class FdNotifyObject {
public:
    FdNotifyObject();

    FdNotifyObject(const FdNotifyObject &) = delete;
    FdNotifyObject& operator=(const FdNotifyObject &) = delete;

    FdNotifyObject(FdNotifyObject &&) = delete;
    FdNotifyObject& operator=(FdNotifyObject &&) = delete;

    ~FdNotifyObject();
    int getNotifyFd(void);
    bool init(void);
    bool notify(void);
private:
    int m_pipeFd[2];
};

} /* namespace Cynara */

#endif /* SRC_MONITOR_NOTIFY_FDNOTIFYOBJECT_H_ */
