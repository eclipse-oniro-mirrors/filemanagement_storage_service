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

#include "storage_daemon_communication/storage_daemon_communication.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "ipc/istorage_daemon.h"
#include "ipc/storage_daemon.h"
#include "ipc/storage_daemon_proxy.h"
#include "os_account_manager.h"
#ifdef ENABLE_SCREENLOCK_MANAGER
#include "screenlock_manager.h"
#endif
#include "storage_service_errno.h"
#include "storage_service_log.h"

namespace OHOS {
namespace StorageManager {
StorageDaemonCommunication::StorageDaemonCommunication()
{
    LOGI("DEBUG StorageDaemonCommunication constructer");
    storageDaemon_ = nullptr;
}

StorageDaemonCommunication::~StorageDaemonCommunication()
{
    LOGI("DEBUG ~StorageDaemonCommunication destructer ~");
}

int32_t StorageDaemonCommunication::Connect()
{
    LOGD("StorageDaemonCommunication::Connect start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (storageDaemon_ == nullptr) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            LOGE("StorageDaemonCommunication::Connect samgr nullptr");
            return E_SA_IS_NULLPTR;
        }
        auto object = sam->GetSystemAbility(STORAGE_MANAGER_DAEMON_ID);
        if (object == nullptr) {
            LOGE("StorageDaemonCommunication::Connect object nullptr");
            return E_REMOTE_IS_NULLPTR;
        }
        storageDaemon_ = iface_cast<OHOS::StorageDaemon::IStorageDaemon>(object);
        if (storageDaemon_ == nullptr) {
            LOGE("StorageDaemonCommunication::Connect service nullptr");
            return E_SERVICE_IS_NULLPTR;
        }
        deathRecipient_ = new (std::nothrow) SdDeathRecipient();
        if (!deathRecipient_) {
            LOGE("StorageDaemonCommunication::Connect failed to create death recipient");
            return E_DEATH_RECIPIENT_IS_NULLPTR;
        }

        storageDaemon_->AsObject()->AddDeathRecipient(deathRecipient_);
    }
    LOGD("StorageDaemonCommunication::Connect end");
    return E_OK;
}

int32_t StorageDaemonCommunication::PrepareAddUser(int32_t userId, uint32_t flags)
{
    LOGI("StorageDaemonCommunication::PrepareAddUser start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::PrepareAddUser connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->PrepareUserDirs(userId, flags);
}

int32_t StorageDaemonCommunication::RemoveUser(int32_t userId, uint32_t flags)
{
    LOGI("StorageDaemonCommunication::RemoveUser start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::RemoveUser connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->DestroyUserDirs(userId, flags);
}

int32_t StorageDaemonCommunication::PrepareStartUser(int32_t userId)
{
    LOGD("StorageDaemonCommunication::PrepareStartUser start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::PrepareStartUser connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->StartUser(userId);
}

int32_t StorageDaemonCommunication::StopUser(int32_t userId)
{
    LOGI("StorageDaemonCommunication::StopUser start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::StopUser connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->StopUser(userId);
}

int32_t StorageDaemonCommunication::CompleteAddUser(int32_t userId)
{
    LOGI("StorageDaemonCommunication::CompleteAddUser start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::CompleteAddUser connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::CompleteAddUser service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->CompleteAddUser(userId);
}

int32_t StorageDaemonCommunication::Mount(std::string volumeId, int32_t flag)
{
    LOGI("StorageDaemonCommunication::mount start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::mount connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->Mount(volumeId, flag);
}

int32_t StorageDaemonCommunication::Unmount(std::string volumeId)
{
    LOGI("StorageDaemonCommunication::unmount start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::unmount connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->UMount(volumeId);
}

int32_t StorageDaemonCommunication::Check(std::string volumeId)
{
    LOGI("StorageDaemonCommunication::check start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::check connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->Check(volumeId);
}

int32_t StorageDaemonCommunication::Partition(std::string diskId, int32_t type)
{
    LOGI("StorageDaemonCommunication::Partition start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::Partition connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->Partition(diskId, type);
}

int32_t StorageDaemonCommunication::Format(std::string volumeId, std::string type)
{
    LOGI("StorageDaemonCommunication::Format start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::Format connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->Format(volumeId, type);
}

int32_t StorageDaemonCommunication::SetVolumeDescription(std::string volumeId, std::string description)
{
    LOGI("StorageDaemonCommunication::SetVolumeDescription start");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("StorageDaemonCommunication::SetVolumeDescription connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->SetVolumeDescription(volumeId, description);
}

int32_t StorageDaemonCommunication::GenerateUserKeys(uint32_t userId, uint32_t flags)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->GenerateUserKeys(userId, flags);
}

int32_t StorageDaemonCommunication::DeleteUserKeys(uint32_t userId)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->DeleteUserKeys(userId);
}

int32_t StorageDaemonCommunication::UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                                                   const std::vector<uint8_t> &token,
                                                   const std::vector<uint8_t> &oldSecret,
                                                   const std::vector<uint8_t> &newSecret)
{
    LOGI("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->UpdateUserAuth(userId, secureUid, token, oldSecret, newSecret);
}

int32_t StorageDaemonCommunication::ActiveUserKey(uint32_t userId,
                                                  const std::vector<uint8_t> &token,
                                                  const std::vector<uint8_t> &secret)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->ActiveUserKey(userId, token, secret);
}

int32_t StorageDaemonCommunication::InactiveUserKey(uint32_t userId)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->InactiveUserKey(userId);
}

int32_t StorageDaemonCommunication::LockUserScreen(uint32_t userId)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->LockUserScreen(userId);
}

int32_t StorageDaemonCommunication::GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->GetFileEncryptStatus(userId, isEncrypted, needCheckDirMount);
}

int32_t StorageDaemonCommunication::UnlockUserScreen(uint32_t userId,
                                                     const std::vector<uint8_t> &token,
                                                     const std::vector<uint8_t> &secret)
{
    LOGI("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->UnlockUserScreen(userId, token, secret);
}

int32_t StorageDaemonCommunication::GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus)
{
    LOGI("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->GetLockScreenStatus(userId, lockScreenStatus);
}

int32_t StorageDaemonCommunication::UpdateKeyContext(uint32_t userId)
{
    LOGI("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->UpdateKeyContext(userId);
}

int32_t StorageDaemonCommunication::ResetSdProxy()
{
    LOGI("enter");
    std::lock_guard<std::mutex> lock(mutex_);
    if ((storageDaemon_ != nullptr) && (storageDaemon_->AsObject() != nullptr)) {
        storageDaemon_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }
    storageDaemon_ = nullptr;

    return E_OK;
}

void StorageDaemonCommunication::ForceLockUserScreen()
{
    LOGI("StorageDaemonCommunication::ForceLockUserScreen, storage_daemon process maybe has died.");
#ifdef ENABLE_SCREENLOCK_MANAGER
    std::vector<int32_t> ids;
    int32_t ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(ids);
    if (ret != ERR_OK || ids.empty()) {
        LOGE("Query active userid failed, ret = %{public}u", ret);
        return;
    }
    int reasonFlag = static_cast<int>(ScreenLock::StrongAuthReasonFlags::ACTIVE_REQUEST);
    ret = ScreenLock::ScreenLockManager::GetInstance()->RequestStrongAuth(reasonFlag, ids[0]);
    if (ret != ScreenLock::E_SCREENLOCK_OK) {
        LOGE("Request strong auth by screen lock manager failed.");
        return;
    }
    ret = ScreenLock::ScreenLockManager::GetInstance()->Lock(ids[0]);
    if (ret != ScreenLock::E_SCREENLOCK_OK) {
        LOGE("Lock user screen by screen lock manager failed.");
    }
    LOGI("Force lock user screen and request strong auth success for userId = %{public}d.", ids[0]);
#endif
}

void SdDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    LOGE("StorageDaemonCommunication::OnRemoteDied, storage_daemon process has died.");
    DelayedSingleton<StorageDaemonCommunication>::GetInstance()->ResetSdProxy();
    DelayedSingleton<StorageDaemonCommunication>::GetInstance()->ForceLockUserScreen();
}

std::vector<int32_t> StorageDaemonCommunication::CreateShareFile(const std::vector<std::string> &uriList,
                                                                 uint32_t tokenId, uint32_t flag)
{
    LOGI("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return std::vector<int32_t>{err};
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return std::vector<int32_t>{err};
    }
    return storageDaemon_->CreateShareFile(uriList, tokenId, flag);
}

int32_t StorageDaemonCommunication::DeleteShareFile(uint32_t tokenId, const std::vector<std::string> &uriList)
{
    LOGI("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->DeleteShareFile(tokenId, uriList);
}

int32_t StorageDaemonCommunication::SetBundleQuota(const std::string &bundleName, int32_t uid,
    const std::string &bundleDataDirPath, int32_t limitSizeMb)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->SetBundleQuota(bundleName, uid, bundleDataDirPath, limitSizeMb);
}

int32_t StorageDaemonCommunication::GetOccupiedSpace(int32_t idType, int32_t id, int64_t &size)
{
    LOGD("enter");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->GetOccupiedSpace(idType, id, size);
}

int32_t StorageDaemonCommunication::MountCryptoPathAgain(int32_t userId)
{
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->MountCryptoPathAgain(userId);
}

int32_t StorageDaemonCommunication::GenerateAppkey(uint32_t userId, uint32_t hashId, std::string &keyId)
{
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->GenerateAppkey(userId, hashId, keyId);
}

int32_t StorageDaemonCommunication::DeleteAppkey(uint32_t userId, const std::string keyId)
{
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->DeleteAppkey(userId, keyId);
}

int32_t StorageDaemonCommunication::GetBundleStatsForIncrease(uint32_t userId,
    const std::vector<std::string> &bundleNames, const std::vector<int64_t> &incrementalBackTimes,
    std::vector<int64_t> &pkgFileSizes, std::vector<int64_t> &incPkgFileSizes)
{
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->GetBundleStatsForIncrease(userId, bundleNames, incrementalBackTimes, pkgFileSizes,
        incPkgFileSizes);
}

int32_t StorageDaemonCommunication::UpdateMemoryPara(int32_t size, int32_t &oldSize)
{
    LOGI("StorageDaemonCommunication::UpdateMemoryPara");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->UpdateMemoryPara(size, oldSize);
}

int32_t StorageDaemonCommunication::MountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    LOGI("StorageDaemonCommunication::MountDfsDocs start.");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->MountDfsDocs(userId, relativePath, networkId, deviceId);
}

int32_t StorageDaemonCommunication::UMountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    LOGI("StorageDaemonCommunication::UMountDfsDocs start.");
    int32_t err = Connect();
    if (err != E_OK) {
        LOGE("Connect failed");
        return err;
    }
    if (storageDaemon_ == nullptr) {
        LOGE("StorageDaemonCommunication::Connect service nullptr");
        return E_SERVICE_IS_NULLPTR;
    }
    return storageDaemon_->UMountDfsDocs(userId, relativePath, networkId, deviceId);
}

} // namespace StorageManager
} // namespace OHOS
