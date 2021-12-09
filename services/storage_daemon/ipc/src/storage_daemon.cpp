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

#include "ipc/storage_daemon.h"
#include "user/user_manager.h"

namespace OHOS {
namespace StorageDaemon {

// StorageDaemon::StorageDaemon() {}
int32_t Shutdown()
{
    return E_OK;
}

int32_t Mount(string volId)
{
    return E_OK;
}

int32_t UMount(string volId)
{
    return E_OK;
}

int32_t Check(string volId)
{
    return E_OK;
}

int32_t Format(string voldId)
{
    return E_OK;
}

int32_t AddUser(int32_t userId)
{
    return UserManager::Instance()->AddUser(userId);
}

int32_t RemoveUser(int32_t userId)
{
    return UserManager::Instance()->RemoveUser(userId);
}

int32_t PrepareUserDirs(int32_t userId, uint32_t flags)
{
    return UserManager::Instance()->PrepareUserDirs(userId, flags);
}

int32_t DestroyUserDirs(int32_t userId, uint32_t flags)
{
    return UserManager::Instance()->DestroyUserDirs(userId, flags);
}

} // StorageDaemon
} // OHOS