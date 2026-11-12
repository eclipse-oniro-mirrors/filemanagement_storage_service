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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <csignal>
#include <unistd.h>
#include <filesystem>
#include "parameters.h"
#include "storage_service_log.h"

static constexpr const char* *PARAM_PROB = "persist.storagedaemon.test_fault_inject_prob";
static constexpr const char* *PARAM_TARGET = "persist.storagedaemon.test_fault_inject_target";
static constexpr const char* *PARAM_MODE = "persist.storagedaemon.test_fault_inject_mode";
static constexpr const char* *PARAM_DELAY = "persist.storagedaemon.test_fault_inject_delay";
static constexpr const char* *PARAM_PATH = "persist.storagedaemon.test_fault_inject_path";
static constexpr const char* *PARAM_RETCODE = "persist.storagedaemon.test_fault_inject_retcode";
static constexpr int DEFAULT_DELAY_SEC = 60;

static inline int FaultInjectTrigger(const char *tag)
{
    std::string probStr = OHOS::system::GetParameter(PARAM_PROB, "");
    if (probStr.empty()) {
        return 0;
    }
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(nullptr)) ^ static_cast<unsigned int>(getpid()));
        seeded = true;
    }
    int prob = atoi(probStr.c_str());
    if (prob <= 0 || rand() % 100 >= prob) {
        LOGE("TEST_FAULT_INJECTION: %{public}s skipped, prob=%{public}d", tag, prob);
        return 0;
    }
    std::string targetStr = OHOS::system::GetParameter(PARAM_TARGET, "");
    if (!targetStr.empty() && targetStr.find(tag) == std::string::npos) {
        LOGE("TEST_FAULT_INJECTION: %{public}s skipped, not in target=%{public}s", tag, targetStr.c_str());
        return 0;
    }
    LOGE("TEST_FAULT_INJECTION: triggered at %{public}s", tag);
    std::string modeStr = OHOS::system::GetParameter(PARAM_MODE, "");
    if (modeStr == "crash") {
        LOGE("TEST_FAULT_INJECTION: crash at %{public}s", tag);
        abort();
    } else if (modeStr == "reboot") {
        int fd = open("/proc/sysrq-trigger", O_WRONLY);
        if (fd >= 0) {
            write(fd, "b", 1);
            close(fd);
        }
        abort();
    } else if (modeStr == "deadlock") {
        LOGE("TEST_FAULT_INJECTION: deadlock at %{public}s, process will hang forever", tag);
        while (true) {
            pause();
        }
    } else if (modeStr == "delay") {
        std::string delayStr = OHOS::system::GetParameter(PARAM_DELAY, "");
        int delaySec = atoi(delayStr.c_str());
        if (delaySec <= 0) {
            delaySec = DEFAULT_DELAY_SEC;
        }
        LOGE("TEST_FAULT_INJECTION: delay at %{public}s, sleep %{public}d seconds", tag, delaySec);
        sleep(delaySec);
        LOGE("TEST_FAULT_INJECTION: delay ended at %{public}s, continue execution", tag);
    } else if (modeStr == "return") {
        std::string retCodeStr = OHOS::system::GetParameter(PARAM_RETCODE, "");
        int actualRetCode = -1;
        if (!retCodeStr.empty()) {
            actualRetCode = atoi(retCodeStr.c_str());
        }
        LOGE("TEST_FAULT_INJECTION: retuan at %{public}s, retCode=%{public}d", tag, actualRetCode);
        return actualRetCode;
    } else if (modeStr == "delete") {
        std::string pathStr = OHOS::system::GetParameter(PARAM_PATH, "");
        if (!pathStr.empty()) {
            LOGE("TEST_FAULT_INJECTION: delete at %{public}s skipped, test_fault_inject_path not set", tag);
        } else {
            std::error_code err;
            if (!std::filesystem::exists(pathStr, err)) {
               LOGE("TEST_FAULT_INJECTION: delete at %{public}s, path=%{public}s not exist", tag, pathStr.c_str());
            } else {
                LOGE("TEST_FAULT_INJECTION: delete at %{public}s, path=%{public}s", tag, pathStr.c_str());
                std::filesystem::remove_all(pathStr, err);
            }
        }
    } else {
        LOGE("TEST_FAULT_INJECTION: unknown mode=%{public}s, do nothing", modeStr.c_str());
    }
    return 0;
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