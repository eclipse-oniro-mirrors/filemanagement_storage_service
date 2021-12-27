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

#include <string>
#include <vector>
#include <unordered_map> 
#include <sys/types.h>
#include <nocopyable.h>
#include "user/user_info.h"

namespace OHOS {
namespace StorageDaemon {
struct DirInfo {
    const std::string path;
    mode_t mode;
    uid_t uid;
    gid_t gid;
};

constexpr uid_t OID_ROOT = 0;
constexpr uid_t OID_SYSTEM = 1000;

class UserManager final {
public:
    virtual ~UserManager() = default;
    static UserManager* Instance();
    int32_t AddUser(int32_t userId);
    int32_t RemoveUser(int32_t userId);
    int32_t PrepareUserDirs(int32_t userId, uint32_t flags);
    int32_t DestroyUserDirs(int32_t userId, uint32_t flags);
    int32_t StartUser(int32_t userId);
    int32_t StopUser(int32_t userId);

private:
    UserManager() = default;
    bool PrepareUserEl1Dirs(int32_t userId);
    bool PrepareUserEl2Dirs(int32_t userId);
    bool PrepareUserHmdfsDirs(int32_t userId);
    bool DestroyUserEl1Dirs(int32_t userId);
    bool DestroyUserEl2Dirs(int32_t userId);
    bool DestroyUserHmdfsDirs(int32_t userId);
    int32_t CheckUserState(int32_t userId, UserState state);
    void SetUserState(int32_t userId, UserState state);
    DISALLOW_COPY_AND_MOVE(UserManager);

    static UserManager* instance_;
    std::unordered_map<int32_t, UserInfo> users_;

    const std::vector<DirInfo> el1DirVec_ = {
        {"/data/app/el1/%d", 0711, OID_ROOT, OID_ROOT},
        {"/data/service/el1/%d", 0711, OID_ROOT, OID_ROOT},
        {"/data/vendor/el1/%d", 0711, OID_ROOT, OID_ROOT}
    };
    const std::vector<DirInfo> el2DirVec_ = {
        {"/data/app/el2/%d", 0711, OID_ROOT, OID_ROOT},
        {"/data/service/el2/%d", 0711, OID_ROOT, OID_ROOT},
        {"/data/vendor/el2/%d", 0711, OID_ROOT, OID_ROOT}
    };
    const std::vector<DirInfo> hmdfsDirVec_ = {
        {"/data/service/el2/%d/hmdfs", 0711, OID_SYSTEM, OID_SYSTEM},
        {"/data/service/el2/%d/hmdfs/files", 0711, OID_SYSTEM, OID_SYSTEM},
        {"/storage/media/%d", 0711, OID_ROOT, OID_ROOT},
        {"/storage/media/%d/local", 0711, OID_ROOT, OID_ROOT}
    };
    const std::string hmdfsSource_ = "/data/service/el2/%d/hmdfs/files";
    const std::string hmdfsTarget_ = "/storage/media/%d/local";
};
} // STORAGE_DAEMON
} // OHOS

#endif // OHOS_STORAGE_DAEMON_USER_MANAGER_H