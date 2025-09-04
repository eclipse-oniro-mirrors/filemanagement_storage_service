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

#ifndef OHOS_STORAGE_DAEMON_USER_MANAGER_H
#define OHOS_STORAGE_DAEMON_USER_MANAGER_H

#include "user/mount_manager.h"

namespace OHOS {
namespace StorageDaemon {
class UserManager final {
public:
    UserManager();
    virtual ~UserManager() = default;
    static UserManager &GetInstance();
    int32_t CheckCrypto(int32_t userId, uint32_t flags);
    int32_t PrepareUserDirs(int32_t userId, uint32_t flags);
    int32_t DestroyUserDirs(int32_t userId, uint32_t flags);
    int32_t StartUser(int32_t userId);
    int32_t StopUser(int32_t userId);
    void CreateElxBundleDataDir(uint32_t userId, uint8_t elx);
    void CheckDirsFromVec(int32_t userId);
    int32_t CreateUserDir(const std::string &path, mode_t mode, uid_t uid, gid_t gid);
    int32_t DeleteUserDir(const std::string &path);

private:
    int32_t PrepareDirsFromIdAndLevel(int32_t userId, const std::string &level);
    int32_t DestroyDirsFromIdAndLevel(int32_t userId, const std::string &level);
    int32_t PrepareEl1BundleDir(int32_t userId);
    int32_t DestroyEl1BundleDir(int32_t userId);
    int32_t PrepareEl2BackupDir(int32_t userId);
    int32_t DestroyEl2BackupDir(int32_t userId);
    int32_t PrepareEl1Dir(int32_t userId);
    int32_t DestroyEl1Dir(int32_t userId);
    int32_t SetElDirFscryptPolicy(int32_t userId, const std::string &level,
        const std::vector<FileList> &list);
    int32_t CheckUserIdRange(int32_t userId);

    DISALLOW_COPY_AND_MOVE(UserManager);

    const std::vector<DirInfo> rootDirVec_;
    const std::vector<DirInfo> el3DirEl4DirEl5DirVec_;
    const std::vector<DirInfo> subDirVec_;
    const std::vector<DirInfo> el2DirVec_;
    const std::vector<DirInfo> el1DirVec_;
    const char *bundle_ = "/data/app/el1/bundle/%d";
    std::mutex mutex_;
};
} // STORAGE_DAEMON
} // OHOS

#endif // OHOS_STORAGE_DAEMON_USER_MANAGER_H