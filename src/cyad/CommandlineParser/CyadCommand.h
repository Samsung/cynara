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
 * @file        src/cyad/CommandlineParser/CyadCommand.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       A representation of Cyad command
 */

#ifndef SRC_CYAD_COMMANDLINEPARSER_CYADCOMMAND_H_
#define SRC_CYAD_COMMANDLINEPARSER_CYADCOMMAND_H_

#include <string>

#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

namespace Cynara {

class CommandsDispatcher;

class CyadCommand {
public:
    CyadCommand() = default;
    virtual ~CyadCommand() {}

    virtual int run(CommandsDispatcher &dispatcher);

    virtual bool isError(void) const {
        return false;
    }
};

class ErrorCyadCommand : public CyadCommand {
public:
    ErrorCyadCommand(const std::string &message) : m_message(message) {}
    virtual ~ErrorCyadCommand() {}

    virtual int run(CommandsDispatcher &dispatcher);

    virtual bool isError(void) const {
        return true;
    }

    virtual const std::string &message(void) const {
        return m_message;
    }

private:
    std::string m_message;
};

class HelpCyadCommand : public CyadCommand {
public:
    using CyadCommand::CyadCommand;

    virtual int run(CommandsDispatcher &dispatcher);
};

class SetBucketCyadCommand : public CyadCommand {
public:
    SetBucketCyadCommand(const PolicyBucketId &bucketId, const PolicyResult &policyResult)
        : m_bucketId(bucketId), m_policyResult(policyResult) {}

    virtual ~SetBucketCyadCommand() {}

    virtual int run(CommandsDispatcher &dispatcher);

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

    const PolicyResult &policyResult(void) const {
        return m_policyResult;
    }

private:
    PolicyBucketId m_bucketId;
    PolicyResult m_policyResult;
};

class DeleteBucketCyadCommand : public CyadCommand {
public:
    explicit DeleteBucketCyadCommand(const PolicyBucketId &bucketId) : m_bucketId(bucketId) {}

    virtual ~DeleteBucketCyadCommand() {}

    virtual int run(CommandsDispatcher &dispatcher);

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

private:
    PolicyBucketId m_bucketId;
};

class SetPolicyCyadCommand : public CyadCommand {
public:
    SetPolicyCyadCommand(const PolicyBucketId &bucketId, const PolicyResult &policyResult,
                         const PolicyKey &policyKey)
        : m_bucketId(bucketId), m_policyResult(policyResult), m_policyKey(policyKey) {}

    virtual ~SetPolicyCyadCommand() {}

    virtual int run(CommandsDispatcher &dispatcher);

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

    const PolicyResult &policyResult(void) const {
        return m_policyResult;
    }

    const PolicyKey &policyKey(void) const {
        return m_policyKey;
    }

private:
    PolicyBucketId m_bucketId;
    PolicyResult m_policyResult;
    PolicyKey m_policyKey;
};

class SetPolicyBulkCyadCommand : public CyadCommand {
public:
    SetPolicyBulkCyadCommand(const std::string &filename) : m_filename(filename) {}

    virtual ~SetPolicyBulkCyadCommand() {}

    virtual int run(CommandsDispatcher &dispatcher);

    const std::string &filename(void) const {
        return m_filename;
    }

private:
    std::string m_filename;
};

class EraseCyadCommand : public CyadCommand {
public:
    EraseCyadCommand(const PolicyBucketId &bucketId, bool recursive, const PolicyKey &policyKey)
        : m_bucketId(bucketId), m_recursive(recursive), m_policyKey(policyKey) {}

    virtual ~EraseCyadCommand() {}

    virtual int run(CommandsDispatcher &dispatcher);

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

    bool recursive(void) const {
        return m_recursive;
    }

    const PolicyKey &policyKey(void) const {
        return m_policyKey;
    }

private:
    PolicyBucketId m_bucketId;
    bool m_recursive;
    PolicyKey m_policyKey;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_CYADCOMMAND_H_ */
