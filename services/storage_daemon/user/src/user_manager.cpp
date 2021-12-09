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
#include "utils/errno.h"
#include "utils/log.h"

namespace OHOS {
namespace StorageDaemon {
UserManager::UserManager()
{}

UserManager* UserManager::Instance()
{
    if (instance == nullptr) {
        instance = new UserManager();
    }
    return instance;
}

int32_t UserManager::AddUser(int32_t userId)
{
    LOGI("add user %d", userId);
    if (users.count(userId) != 0) {
        return E_EXIST;
    }

    users.insert({ userId, UserInfo(userId, USER_CREAT) });
    return E_OK;
}

int32_t UserManager::RemoveUser(int32_t userId)
{
    LOGI("remove user %d", userId);
    users.erase(userId);
    return E_OK;
}

int32_t UserManager::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    return E_OK;
}

int32_t UserManager::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    return E_OK;
}
} // StorageDaemon
} // OHOS