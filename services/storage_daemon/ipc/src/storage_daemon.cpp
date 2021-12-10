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
#include "utils/errno.h"

namespace OHOS {
namespace StorageDaemon {
int32_t StorageDaemon::Shutdown()
{
    return E_OK;
}

int32_t StorageDaemon::Mount(std::string volId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageDaemon::UMount(std::string volId)
{
    return E_OK;
}

int32_t StorageDaemon::Check(std::string volId)
{
    return E_OK;
}

int32_t StorageDaemon::Format(std::string voldId)
{
    return E_OK;
}

int32_t StorageDaemon::AddUser(int32_t userId)
{
    int32_t ret;

    (void)pthread_mutex_lock(&user_lock_);
    ret = UserManager::Instance()->AddUser(userId);
    (void)pthread_mutex_unlock(&user_lock_);

    return ret;
}

int32_t StorageDaemon::RemoveUser(int32_t userId)
{
    int32_t ret;

    (void)pthread_mutex_lock(&user_lock_);
    ret = UserManager::Instance()->RemoveUser(userId);
    (void)pthread_mutex_unlock(&user_lock_);

    return ret;
}

int32_t StorageDaemon::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    int32_t ret;

    (void)pthread_mutex_lock(&user_lock_);
    ret = UserManager::Instance()->PrepareUserDirs(userId, flags);
    (void)pthread_mutex_unlock(&user_lock_);

    return ret;
}

int32_t StorageDaemon::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    int32_t ret;

    (void)pthread_mutex_lock(&user_lock_);
    ret = UserManager::Instance()->DestroyUserDirs(userId, flags);
    (void)pthread_mutex_unlock(&user_lock_);

    return ret;
}

//TODO add mutex_lock
int32_t StorageDaemon::StartUser(int32_t userId)
{
    int32_t ret;

    (void)pthread_mutex_lock(&user_lock_);
    ret = UserManager::Instance()->StartUser(userId);
    (void)pthread_mutex_unlock(&user_lock_);

    return ret;
}


int32_t StorageDaemon::StopUser(int32_t userId)
{
    int32_t ret;

    (void)pthread_mutex_lock(&user_lock_);
    ret = UserManager::Instance()->StopUser(userId);
    (void)pthread_mutex_unlock(&user_lock_);

    return ret;
}

 // StorageDaemon
} // OHOS
