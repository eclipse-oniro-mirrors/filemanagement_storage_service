/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "user/user_manager.h"
#include <stdlib.h>
#include "utils/errno.h"
#include "utils/file_utils.h"
#include "utils/string_utils.h"
#include "utils/log.h"
#include "ipc/istorage_daemon.h"

using namespace std;

namespace OHOS {
namespace StorageDaemon {
constexpr int32_t UMOUNT_RETRY_TIMES = 3;
UserManager* UserManager::instance_ = nullptr;

UserManager* UserManager::Instance()
{
    if (instance_ == nullptr) {
        instance_ = new UserManager();
    }

    return instance_;
}

int32_t UserManager::StartUser(int32_t userId)
{
    LOGI("start user %{public}d", userId);

    if ((Mount(StringPrintf(hmdfsSource_.c_str(), userId),
               StringPrintf(hmdfsTarget_.c_str(), userId),
               nullptr, MS_BIND, nullptr))) {
        LOGE("failed to mount, err %{public}d", errno);
        return E_MOUNT;
    }

    return E_OK;
}

int32_t UserManager::StopUser(int32_t userId)
{
    LOGI("stop user %{public}d", userId);

    int32_t count = 0;
    while (count < UMOUNT_RETRY_TIMES) {
        int32_t err = UMount(StringPrintf(hmdfsTarget_.c_str(), userId));
        if (err == E_OK) {
            break;
        } else if (errno == EBUSY) {
            count++;
            continue;
        } else {
            LOGE("failed to umount, errno %{public}d", errno);
            return E_UMOUNT;
        }
    }

    return E_OK;
}

int32_t UserManager::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    LOGI("prepare user dirs for %{public}d, flags %{public}u", userId, flags);

    if (flags & IStorageDaemon::CRYPTO_FLAG_EL1) {
        if (!PrepareUserEl1Dirs(userId)) {
            LOGE("failed to prepare user el1 dirs for userid %{public}d", userId);
            return E_PREPARE_DIR;
        }
    }

    if (flags & IStorageDaemon::CRYPTO_FLAG_EL2) {
        if (!PrepareUserEl2Dirs(userId)) {
            LOGE("failed to prepare user el2 dirs for userid %{public}d", userId);
            return E_PREPARE_DIR;
        }
    }

    return E_OK;
}

int32_t UserManager::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    LOGI("destroy user dirs for %{public}d, flags %{public}u", userId, flags);

    int32_t err = E_OK;

    if (flags & IStorageDaemon::CRYPTO_FLAG_EL1) {
        if (!DestroyUserEl1Dirs(userId)) {
            LOGE("failed to destroy user el1 dirs for userid %{public}d", userId);
            err = E_DESTROY_DIR;
        }
    }

    if (flags & IStorageDaemon::CRYPTO_FLAG_EL2) {
        if (!DestroyUserEl2Dirs(userId)) {
            LOGE("failed to destroy user el2 dirs for userid %{public}d", userId);
            err = E_DESTROY_DIR;
        }
    }

    return err;
}

inline bool PrepareUserDirsFromVec(int32_t userId, std::vector<DirInfo> dirVec)
{
    for (DirInfo &dir : dirVec) {
        if (!PrepareDir(StringPrintf(dir.path.c_str(), userId), dir.mode, dir.uid, dir.gid)) {
                return false;
        }
    }

    return true;
}

inline bool DestroyUserDirsFromVec(int32_t userId, std::vector<DirInfo> dirVec)
{
    bool ret = true;

    for (DirInfo &dir : dirVec) {
        if (IsEndWith(dir.path.c_str(), "%d")) {
            ret &= RmDirRecurse(StringPrintf(dir.path.c_str(), userId));
        }
    }

    return ret;
}

bool UserManager::PrepareUserEl1Dirs(int32_t userId)
{
    return PrepareUserDirsFromVec(userId, el1DirVec_);
}

bool UserManager::PrepareUserEl2Dirs(int32_t userId)
{
    return PrepareUserDirsFromVec(userId, el2DirVec_) && PrepareUserHmdfsDirs(userId);
}

bool UserManager::PrepareUserHmdfsDirs(int32_t userId)
{
    return PrepareUserDirsFromVec(userId, hmdfsDirVec_);
}

bool UserManager::DestroyUserEl1Dirs(int32_t userId)
{
    return DestroyUserDirsFromVec(userId, el1DirVec_);
}

bool UserManager::DestroyUserEl2Dirs(int32_t userId)
{
    return DestroyUserDirsFromVec(userId, hmdfsDirVec_) && DestroyUserDirsFromVec(userId, el2DirVec_);
}

bool UserManager::DestroyUserHmdfsDirs(int32_t userId)
{
    return DestroyUserDirsFromVec(userId, hmdfsDirVec_);
}

} // StorageDaemon
} // OHOS