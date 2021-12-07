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

#include <unordered_map> 

#include "user/user_info.h"
#include "utils/nocopyable.h"

namespace OHOS {
namespace StorageDaemon {
class UserManager final {
public:
    virtual ~UserManager() = default;
    static UserManager* Instance();

    int32_t AddUser(int32_t userId);
    int32_t RemoveUser(int32_t userId);
    int32_t PrepareUserDirs(int32_t userId, uint32_t flags);
    int32_t DestroyUserDirs(int32_t userId, uint32_t flags);
private:
    int32_t PrepareUserEl1Dirs(int32_t userId);
    int32_t PrepareUserEl2Dirs(int32_t userId);
    int32_t DestroyUserEl1Dirs(int32_t userId);
    int32_t DestroyUserEl2Dirs(int32_t userId);
    UserManager() = default;
    DISABLE_COPY_ASSIGN_MOVE(UserManager);

private:
    static UserManager* instance;
    std::unordered_map<int32_t, UserInfo> users;
};
} // STORAGE_DAEMON
} // OHOS

#endif // OHOS_STORAGE_DAEMON_USER_MANAGER_H