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
#include <sys/mount.h>

namespace OHOS {
namespace StorageDaemon {
using namespace std;

UserManager* UserManager::instance_ = nullptr;

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

int32_t UserManager::StartUser(int32_t userId)
{
    LOGI("start user %{public}d", userId);

    auto iterator = users_.find(userId);
    if (iterator == users_.end()) {
        LOGI("the user %{public}d doesn't exist", userId);
        return E_ERR;
    }

    UserInfo& user = iterator->second;
    if (user.GetState() != USER_PREPARE) {
        LOGI("the user's state %{public}d is invalid", iterator->second.GetState());
        return E_ERR;
    }

    /*
     * TODO get_property: hmdfs
     * assume non-dfs here
     */
    std::string source = DATA_SERVICE_EL2 + to_string(userId) + HMDFS_FILES;
    std::string target = STORAGE_MEDIA + to_string(userId) + LOCAL;
    int err = mount(source.c_str(), target.c_str(), nullptr, MS_BIND, nullptr);
    if (err) {
        LOGI("fail to mount, err %{public}d", err);
        return err;
    }

    user.SetState(USER_START);

    return E_OK;
}

int32_t UserManager::StopUser(int32_t userId)
{
    LOGI("stop user %{public}d", userId);

    auto iterator = users_.find(userId);
    if (iterator == users_.end()) {
        LOGI("the user %{public}d doesn't exist", userId);
        return E_ERR;
    }

    UserInfo& user = iterator->second;
    if (user.GetState() != USER_START) {
        LOGI("the user's state %{public}d is invalid", iterator->second.GetState());
        return E_ERR;
    }

    // TODO get_property: hmdfs
    std::string target = STORAGE_MEDIA + to_string(userId) + LOCAL;
    int err = umount(target.c_str());
    if (err) {
        LOGI("fail to mount, err %{public}d", err);
        return err;
    }

    user.SetState(USER_PREPARE);

    return E_OK;
}

int32_t UserManager::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    LOGI("prepare user dirs for %{public}d, flags %{public}u", userId, flags);

    auto iterator = users_.find(userId);
    if (iterator == users_.end()) {
        LOGI("the user %{public}d doesn't exist", userId);
        return E_ERR;
    }

    UserInfo& user = iterator->second;
    if (user.GetState() != USER_CREAT) {
        LOGI("the user's state %{public}d is invalid", iterator->second.GetState());
        return E_ERR;
    }

    int err = E_OK;
    if (flags & IStorageDaemon::CRYPTO_FLAG_EL1) {
        if ((err = PrepareUserEl1Dirs(userId))) {
            LOGE("failed to prepare user el1 dirs for userid %{public}d", userId);
            return err;
        }
    }

    if (flags & IStorageDaemon::CRYPTO_FLAG_EL2) {
        if ((err = PrepareUserEl2Dirs(userId))) {
            LOGE("failed to prepare user el2 dirs for userid %{public}d", userId);
            return err;
        }
    }

    user.SetState(USER_PREPARE);

    return err;
}

int32_t UserManager::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    LOGI("destroy user dirs for %{public}d, flags %{public}u", userId, flags);

    auto iterator = users_.find(userId);
    if (iterator == users_.end()) {
        LOGI("the user %{public}d doesn't exist", userId);
        return E_ERR;
    }

    UserInfo& user = iterator->second;
    if (user.GetState() != USER_PREPARE) {
        LOGI("the user's state %{public}d is invalid", iterator->second.GetState());
        return E_ERR;
    }

    int err = E_OK;
    if (flags & IStorageDaemon::CRYPTO_FLAG_EL1) {
        err = DestroyUserEl1Dirs(userId);
        if (err != E_OK) {
            LOGE("failed to destroy user el1 dirs for userid %{public}d", userId);
        }
    }

    if (flags & IStorageDaemon::CRYPTO_FLAG_EL2) {
        err = DestroyUserEl2Dirs(userId);
        if (err != E_OK) {
            LOGE("failed to destroy user el2 dirs for userid %{public}d", userId);
        }
    }

    user.SetState(USER_CREAT);

    return err;
}

int32_t UserManager::PrepareUserEl1Dirs(int32_t userId)
{
    int err = E_OK;

    for (auto& entry : g_el1DirMap) {
        const string &dir = entry.first;
        struct DirInfo &info = entry.second;

        if ((err = PrepareDir(dir + to_string(userId), info.mode, info.uid, info.gid))) {
            return err;
        }
    }

    return err;
}

int32_t UserManager::PrepareUserEl2Dirs(int32_t userId)
{
    int err = E_OK;

    for (auto &entry : g_el2DirMap) {
        const string &dir = entry.first;
        struct DirInfo &info = entry.second;

        if ((err = PrepareDir(dir + to_string(userId), info.mode, info.uid, info.gid))) {
            return err;
        }
    }

    // TODO get_property: hmdfs
    if ((err = PrepareDir(DATA_SERVICE_EL2 + to_string(userId) + HMDFS_FILES, 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }

    if ((err = PrepareDir(STORAGE_MEDIA + to_string(userId) + LOCAL, 0711, OID_ROOT, OID_ROOT))) {
        return err;
    }

    return err;
}

// Destory dirs as much as possible, and return one of err;
int32_t UserManager::DestroyUserEl1Dirs(int32_t userId)
{
    int err = E_OK;

    for (auto &entry : g_el1DirMap) {
        int32_t ret = DestroyDir(entry.first + to_string(userId));
        err = ret ? ret : err;
    }

    return err;
}

// Destory dirs as much as possible, and return one of err;
int32_t UserManager::DestroyUserEl2Dirs(int32_t userId)
{
    int err = E_OK;

    for (auto &entry : g_el2DirMap) {
        int32_t ret = DestroyDir(entry.first + to_string(userId));
        err = ret ? ret : err;
    }

    return err;
}

} // StorageDaemon
} // OHOS
