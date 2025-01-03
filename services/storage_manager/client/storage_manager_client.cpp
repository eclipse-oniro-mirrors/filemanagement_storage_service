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
#include "client/storage_manager_client.h"

#include "iremote_object.h"
#include "iremote_proxy.h"
#include "iservice_registry.h"
#include "storage_service_log.h"
#include "storage_service_log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace StorageManager {
sptr<IStorageManager> StorageManagerClient::GetStorageManagerProxy(void)
{
    auto samgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        LOGE("samgr empty error");
        return nullptr;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(OHOS::STORAGE_MANAGER_MANAGER_ID);
    if (object == nullptr) {
        LOGE("storage manager client samgr ablity empty error");
        return nullptr;
    }

    return iface_cast<IStorageManager>(object);
}

int32_t StorageManagerClient::PrepareAddUser(uint32_t userId, uint32_t flags)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->PrepareAddUser(userId, flags);
}

int32_t StorageManagerClient::RemoveUser(uint32_t userId, uint32_t flags)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->RemoveUser(userId, flags);
}

int32_t StorageManagerClient::GenerateUserKeys(uint32_t userId, uint32_t flags)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->GenerateUserKeys(userId, flags);
}

int32_t StorageManagerClient::DeleteUserKeys(uint32_t userId)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->DeleteUserKeys(userId);
}

int32_t StorageManagerClient::UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                                             const std::vector<uint8_t> &token,
                                             const std::vector<uint8_t> &oldSecret,
                                             const std::vector<uint8_t> &newSecret)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->UpdateUserAuth(userId, secureUid, token, oldSecret, newSecret);
}

int32_t StorageManagerClient::UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                                           const std::vector<uint8_t> &newSecret,
                                                           uint64_t secureUid,
                                                           uint32_t userId,
                                                           std::vector<std::vector<uint8_t>> &plainText)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->UpdateUseAuthWithRecoveryKey(authToken, newSecret, secureUid, userId, plainText);
}

int32_t StorageManagerClient::ActiveUserKey(uint32_t userId,
                                            const std::vector<uint8_t> &token,
                                            const std::vector<uint8_t> &secret)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->ActiveUserKey(userId, token, secret);
}

int32_t StorageManagerClient::InactiveUserKey(uint32_t userId)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->InactiveUserKey(userId);
}

int32_t StorageManagerClient::UpdateKeyContext(uint32_t userId)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->UpdateKeyContext(userId);
}

int32_t StorageManagerClient::LockUserScreen(uint32_t userId)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return E_SA_IS_NULLPTR;
    }

    return client->LockUserScreen(userId);
}

int32_t StorageManagerClient::UnlockUserScreen(uint32_t userId,
                                               const std::vector<uint8_t> &token,
                                               const std::vector<uint8_t> &secret)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->UnlockUserScreen(userId, token, secret);
}

int32_t StorageManagerClient::GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->GetFileEncryptStatus(userId, isEncrypted, needCheckDirMount);
}

int32_t StorageManagerClient::GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->GetLockScreenStatus(userId, lockScreenStatus);
}

int32_t StorageManagerClient::MountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->MountDfsDocs(userId, relativePath, networkId, deviceId);
}

int32_t StorageManagerClient::UMountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    sptr<IStorageManager> client = GetStorageManagerProxy();
    if (client == nullptr) {
        LOGE("get storage manager service failed");
        return -EFAULT;
    }

    return client->UMountDfsDocs(userId, relativePath, networkId, deviceId);
}
}
}
