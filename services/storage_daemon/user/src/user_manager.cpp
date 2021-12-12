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
#include "ipc/istorage_daemon.h"
#include "utils/errno.h"
#include "utils/file_utils.h"
#include "utils/log.h"
#include "utils/user_path.h"

namespace OHOS {
namespace StorageDaemon {
using namespace std;

UserManager* UserManager::Instance()
{
    if (instance_ == nullptr) {
        instance_ = new UserManager();
    }
    return instance_;
}

int32_t UserManager::AddUser(int32_t userId)
{
    LOGI("add user %{public}d", userId);
    if (users_.count(userId) != 0) {
        return E_EXIST;
    }

    users_.insert({ userId, UserInfo(userId, USER_CREAT) });
    return E_OK;
}

int32_t UserManager::RemoveUser(int32_t userId)
{
    LOGI("remove user %{public}d", userId);
    users_.erase(userId);
    return E_OK;
}

int32_t UserManager::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    LOGI("prepare user dirs for %{public}d, flags %{public}u", userId, flags);

    int err = E_OK;
    if (flags & IStorageDaemon::CRYPTO_FLAG_EL1) {
        if ((err = PrepareUserEl1Dirs(userId))) {
            LOGI("failed to prepare user el1 dirs for userid %{public}d", userId);
            return err;
        }
        // TODO need set policy on those path
    }
    if (flags & IStorageDaemon::CRYPTO_FLAG_EL2) {
        if ((err = PrepareUserEl2Dirs(userId))) {
            LOGI("failed to prepare user el2 dirs for userid %{public}d", userId);
            return err;
        }
        // TODO need set policy on those path
    }
    return err;
}

int32_t UserManager::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    LOGI("destroy user dirs for %{public}d, flags %{public}u", userId, flags);

    int err = E_OK;
    if (flags & IStorageDaemon::CRYPTO_FLAG_EL1) {
        int res = DestroyUserEl1Dirs(userId);
        if (res != E_OK) {
            err = res;
            LOGI("failed to destroy user el1 dirs for userid %{public}d", userId);
        }
    }
    if (flags & IStorageDaemon::CRYPTO_FLAG_EL2) {
        int res = DestroyUserEl2Dirs(userId);
        if (res != E_OK) {
            err = res;
            LOGI("failed to destroy user el2 dirs for userid %{public}d", userId);
        }
    }
    return err;
}

// TODO we don't have OID define
constexpr uid_t OID_ROOT = 0;
int32_t UserManager::PrepareUserEl1Dirs(int32_t userId)
{
    int err;
    // TODO we don't have OID define
    if ((err = PrepareDir(DATA_APP_EL1 + to_string(userId), 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }
    if ((err = PrepareDir(DATA_SERVICE_EL1 + to_string(userId), 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }
    if ((err = PrepareDir(DATA_VENDOR_EL1 + to_string(userId), 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }
    return err;
}

int32_t UserManager::PrepareUserEl2Dirs(int32_t userId)
{
    int err;
    // TODO we don't have OID define
    if ((err = PrepareDir(DATA_APP_EL2 + to_string(userId), 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }
    if ((err = PrepareDir(DATA_SERVICE_EL2 + to_string(userId), 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }
    if ((err = PrepareDir(DATA_VENDOR_EL2 + to_string(userId), 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }
    return err;
}

// Destory dirs as much as possible, and return one of err;
int32_t UserManager::DestroyUserEl1Dirs(int32_t userId)
{
    int err = E_OK;
    int res = DestroyDir(DATA_APP_EL1 + to_string(userId));
    err = res ? res : err;
    res = DestroyDir(DATA_SERVICE_EL1 + to_string(userId));
    err = res ? res : err;
    res = DestroyDir(DATA_VENDOR_EL1 + to_string(userId));
    err = res ? res : err;
    return err;
}

// Destory dirs as much as possible, and return one of err;
int32_t UserManager::DestroyUserEl2Dirs(int32_t userId)
{
    int err = E_OK;
    int res = DestroyDir(DATA_APP_EL2 + to_string(userId));
    err = res ? res : err;
    res = DestroyDir(DATA_SERVICE_EL2 + to_string(userId));
    err = res ? res : err;
    res = DestroyDir(DATA_VENDOR_EL2 + to_string(userId));
    err = res ? res : err;
    return err;
}

} // StorageDaemon
} // OHOS
