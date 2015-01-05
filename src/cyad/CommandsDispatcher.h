/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/cyad/CommandsDispatcher.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       CommandsDispatcher class
 */

#ifndef SRC_CYAD_COMMANDSDISPATCHER_H_
#define SRC_CYAD_COMMANDSDISPATCHER_H_

#include <cyad/BaseAdminApiWrapper.h>
#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/DispatcherIO.h>

struct cynara_admin;
struct cynara_admin_policy;

namespace Cynara {

class CommandsDispatcher {
public:
    CommandsDispatcher(BaseDispatcherIO &io, BaseAdminApiWrapper &adminApiWrapper);
    virtual ~CommandsDispatcher();

    virtual int execute(CyadCommand &);
    virtual int execute(HelpCyadCommand &);
    virtual int execute(ErrorCyadCommand &);
    virtual int execute(DeleteBucketCyadCommand &);
    virtual int execute(SetBucketCyadCommand &);
    virtual int execute(SetPolicyCyadCommand &);
    virtual int execute(SetPolicyBulkCyadCommand &);
    virtual int execute(EraseCyadCommand &);
    virtual int execute(CheckCyadCommand &);

private:
    // TODO: Get argv[0] instead of hardcoded name
    const std::string helpMessage = "Usage: cyad [OPTIONS]\n\n"
                                    "Bucket set options (with -b or --set-bucket)\n"
                                    "  -b, --set-bucket=<name>        name of bucket\n"
                                    "  -t, --type=<policy>            default policy type\n"
                                    "  -m, --metadata=<metadata>      metadata for default policy\n"
                                    "\n"
                                    "Bucket delete options (with -d or --delete-bucket)\n"
                                    "  -d, --delete-bucket=<name>     name of bucket to delete\n"
                                    "\n"
                                    "Policy set options (with -s or --set-policy)\n"
                                    "  -k, --bucket=<name>            name of bucket;"
                                                                     " omit for default\n"
                                    "  -c, --client=<client>          client value\n"
                                    "  -u, --user=<user>              user value\n"
                                    "  -p, --privilege=<privilege>    privilege value\n"
                                    "  -t, --type=<policy>            policy type\n"
                                    "  -m, --metadata=<metadata>      metadata for policy\n"
                                    "  -f, --bulk=<filename>          path or - for stdin\n"
                                    "\n"
                                    "Policy erase options (with -e or --erase)\n"
                                    "  -e, --erase=<name>             name of bucket"
                                                                     " to start from\n"
                                    "  -r, --recursive=<yes|no>       if linked buckets should be"
                                                                     " processed as well\n"
                                    "  -c, --client=<client>          client value\n"
                                    "  -u, --user=<user>              user value\n"
                                    "  -p, --privilege=<privilege>    privilege value\n"
                                    "\n"
                                    "Administrative policy check options (with -a or --check)\n"
                                    "  -k, --bucket=<name>            name of bucket;"
                                                                     " omit for default\n"
                                    "  -r, --recursive=<yes|no>       if linked buckets should be"
                                                                     " processed as well\n"
                                    "  -c, --client=<client>          client value\n"
                                    "  -u, --user=<user>              user value\n"
                                    "  -p, --privilege=<privilege>    privilege value\n"
                                    "\n"
                                    "Help options:\n"
                                    "  -h, --help                     print help message";
    BaseDispatcherIO &m_io;
    BaseAdminApiWrapper &m_adminApiWrapper;
    struct cynara_admin *m_cynaraAdmin;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDSDISPATCHER_H_ */
