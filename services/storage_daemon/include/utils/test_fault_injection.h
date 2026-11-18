/*
 * Copyright (c) 2021-2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef STORAGE_DAEMON_TEST_FAULT_INJECTION__H
#define STORAGE_DAEMON_TEST_FAULT_INJECTION__H

#ifdef ENABLE_TEST_FAULT_INJECTION

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <random>
#include <string>
#include "parameters.h"
#include "storage_service_log.h"
#include "utils/file_utils.h"

static constexpr const char *PARAM_PROB = "persist.storagedaemon.test_fault_inject_prob";
static constexpr const char *PARAM_TARGET = "persist.storagedaemon.test_fault_inject_target";
static constexpr const char *PARAM_MODE = "persist.storagedaemon.test_fault_inject_mode";
static constexpr const char *PARAM_DELAY = "persist.storagedaemon.test_fault_inject_delay";
static constexpr const char *PARAM_PATH = "persist.storagedaemon.test_fault_inject_path";
static constexpr const char *PARAM_RETCODE = "persist.storagedaemon.test_fault_inject_retcode";
static constexpr int DEFAULT_DELAY_SEC = 60;

static inline std::string GetFaultParam(const char *key)
{
    return OHOS::system::GetParameter(key, "");
}

static inline int GetFaultIntParam(const char *key, int defaultVal)
{
    std::string val = GetFaultParam(key);
    return val.empty() ? defaultVal : atoi(val.c_str());
}

static inline bool IsProbSkipped(const char *tag)
{
    int prob = GetFaultIntParam(PARAM_PROB, 0);
    if (prob <= 0) {
        return true;
    }
    std::random_device rd;
    bool skipped = static_cast<int>(rd() % 100) >= prob;
    if (skipped) {
        LOGE("TEST_FAULT_INJECTION: %{public}s skipped, prob=%{public}d", tag, prob);
    }
    return skipped;
}

static inline bool IsTargetSkipped(const char *tag)
{
    std::string target = GetFaultParam(PARAM_TARGET);
    if (target.empty()) {
        return false;
    }
    bool skipped = target.find(tag) == std::string::npos;
    if (skipped) {
        LOGE("TEST_FAULT_INJECTION: %{public}s skipped, not in target=%{public}s", tag, target.c_str());
    }
    return skipped;
}

static inline void TriggerCrash(const char *tag)
{
    LOGE("TEST_FAULT_INJECTION: crash at %{public}s", tag);
    raise(SIGABRT);
    _Exit(EXIT_FAILURE);
}

static inline void TriggerReboot(const char *tag)
{
    int fd = open("/proc/sysrq-trigger", O_WRONLY);
    if (fd >= 0) {
        (void)write(fd, "b", 1);
        (void)close(fd);
    }
    TriggerCrash(tag);
}

static inline void TriggerDeadlock(const char *tag)
{
    LOGE("TEST_FAULT_INJECTION: deadlock at %{public}s, process will hang over", tag);
    while (true) {
        pause();
    }
}

static inline void TriggerDelay(const char *tag)
{
    int delaySec = GetFaultIntParam(PARAM_DELAY, 0);
    if (delaySec <= 0) {
        delaySec = DEFAULT_DELAY_SEC;
    }
    LOGE("TEST_FAULT_INJECTION: delay at %{public}s, sleep %{public}d seconds", tag, delaySec);
    (void)sleep(delaySec);
    LOGE("TEST_FAULT_INJECTION: delay ended at %{public}s, continue execution", tag);
}

static inline int TriggerReturn(const char *tag)
{
    int retCode = GetFaultIntParam(PARAM_RETCODE, -1);
    LOGE("TEST_FAULT_INJECTION: return at %{public}s, retCode=%{public}d", tag, retCode);
    return retCode;
}

static inline void DeleteFaultPath(const char *tag, const std::string &path)
{
    struct stat st = {};
    if (stat(path.c_str(), &st) != 0) {
        LOGE("TEST_FAULT_INJECTION: delete at %{public}s, path=%{public}s not exist", tag, path.c_str());
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        (void)OHOS::StorageDaemon::RmDirRecurse(path);
        return;
    }
    (void)unlink(path.c_str());
}

static inline void TriggerDelete(const char *tag)
{
    std::string path = GetFaultParam(PARAM_PATH);
    if (path.empty()) {
        LOGE("TEST_FAULT_INJECTION: delete at %{public}s skipped, test_fault_inject_path not set", tag);
        return;
    }
    DeleteFaultPath(tag, path);
}

static inline int HandleFatalMode(const char *tag, const std::string &mode)
{
    if (mode == "crash") {
        TriggerCrash(tag);
    } else if (mode == "reboot") {
        TriggerReboot(tag);
    } else if (mode == "deadlock") {
        TriggerDeadlock(tag);
    }
    return 0;
}

static inline int HandleSoftMode(const char *tag, const std::string &mode)
{
    if (mode == "delay") {
        TriggerDelay(tag);
    } else if (mode == "return") {
        return TriggerReturn(tag);
    } else if (mode == "delete") {
        TriggerDelete(tag);
    } else {
        LOGE("TEST_FAULT_INJECTION: unknown soft mode=%{public}s, do nothing", mode.c_str());
    }
    return 0;
}

static inline int FaultInjectTrigger(const char *tag)
{
    if (IsProbSkipped(tag) || IsTargetSkipped(tag)) {
        return 0;
    }
    LOGE("TEST_FAULT_INJECTION: triggered at %{public}s", tag);
    std::string mode = GetFaultParam(PARAM_MODE);
    if (mode == "crash" || mode == "reboot" || mode == "deadlock") {
        return HandleFatalMode(tag, mode);
    }
    return HandleSoftMode(tag, mode);
}

#define FAULT_POINT(tag) \
    do { \
        int _fault_ret = FaultInjectTrigger(tag); \
        if (_fault_ret != 0) { return _fault_ret; } \
    } while (0)

#else // ENABLE_TEST_FAULT_INJECTION

#define FAULT_POINT(tag) ((void)0)

#endif // ENABLE_TEST_FAULT_INJECTION

#endif // STORAGE_DAEMON_TEST_FAULT_INJECTION__H
