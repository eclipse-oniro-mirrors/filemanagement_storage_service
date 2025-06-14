/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "ipc/storage_manager.h"

#include <sys/syscall.h>
#include <sys/resource.h>

#include <singleton.h>
#include "utils/storage_radar.h"
#ifdef STORAGE_STATISTICS_MANAGER
#include <storage/storage_monitor_service.h>
#include <storage/storage_status_service.h>
#include <storage/storage_total_status_service.h>
#include <storage/volume_storage_status_service.h>
#endif

#ifdef USER_CRYPTO_MANAGER
#include "account_subscriber/account_subscriber.h"
#include "crypto/filesystem_crypto.h"
#include "appspawn.h"
#include "utils/storage_radar.h"
#endif
#ifdef EXTERNAL_STORAGE_MANAGER
#include "disk/disk_manager_service.h"
#include "volume/volume_manager_service.h"
#endif
#include "storage_daemon_communication/storage_daemon_communication.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "system_ability_definition.h"
#include "utils/storage_utils.h"
#include "user/multi_user_manager_service.h"

constexpr bool DECRYPTED = false;
constexpr bool ENCRYPTED = true;

using namespace OHOS::StorageService;
namespace OHOS {
namespace StorageManager {
void StorageManager::ResetUserEventRecord(int32_t userId)
{
#ifdef USER_CRYPTO_MANAGER
    AccountSubscriber::ResetUserEventRecord(userId);
#endif
}

int32_t StorageManager::PrepareAddUser(int32_t userId, uint32_t flags)
{
    LOGI("StorageManager::PrepareAddUser start, userId: %{public}d", userId);
    std::shared_ptr<MultiUserManagerService> userManager = DelayedSingleton<MultiUserManagerService>::GetInstance();
    int32_t err = userManager->PrepareAddUser(userId, flags);
    return err;
}

int32_t StorageManager::RemoveUser(int32_t userId, uint32_t flags)
{
    LOGI("StorageManger::RemoveUser start, userId: %{public}d", userId);
    std::shared_ptr<MultiUserManagerService> userManager = DelayedSingleton<MultiUserManagerService>::GetInstance();
    int32_t err = userManager->RemoveUser(userId, flags);
    return err;
}

int32_t StorageManager::PrepareStartUser(int32_t userId)
{
    LOGI("StorageManger::PrepareStartUser start, userId: %{public}d", userId);
    std::shared_ptr<MultiUserManagerService> userManager = DelayedSingleton<MultiUserManagerService>::GetInstance();
    int32_t err = userManager->PrepareStartUser(userId);
    return err;
}

int32_t StorageManager::StopUser(int32_t userId)
{
    LOGI("StorageManger::StopUser start, userId: %{public}d", userId);
    std::shared_ptr<MultiUserManagerService> userManager = DelayedSingleton<MultiUserManagerService>::GetInstance();
    int32_t err = userManager->StopUser(userId);
    if (err != E_USERID_RANGE) {
        ResetUserEventRecord(userId);
    }
    return err;
}

int32_t StorageManager::CompleteAddUser(int32_t userId)
{
    LOGI("StorageManger::CompleteAddUser start, userId: %{public}d", userId);
    std::shared_ptr<MultiUserManagerService> userManager = DelayedSingleton<MultiUserManagerService>::GetInstance();
    int32_t err = userManager->CompleteAddUser(userId);
    return err;
}

int32_t StorageManager::GetFreeSizeOfVolume(const std::string &volumeUuid, int64_t &freeSize)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGI("StorageManger::getFreeSizeOfVolume start, volumeUuid: %{public}s",
        GetAnonyString(volumeUuid).c_str());
    std::shared_ptr<VolumeStorageStatusService> volumeStatsManager =
        DelayedSingleton<VolumeStorageStatusService>::GetInstance();
    int32_t err = volumeStatsManager->GetFreeSizeOfVolume(volumeUuid, freeSize);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("VolumeStorageStatusService::GetFreeSizeOfVolume", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetTotalSizeOfVolume(const std::string &volumeUuid, int64_t &totalSize)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGI("StorageManger::getTotalSizeOfVolume start, volumeUuid: %{public}s",
        GetAnonyString(volumeUuid).c_str());
    std::shared_ptr<VolumeStorageStatusService> volumeStatsManager =
        DelayedSingleton<VolumeStorageStatusService>::GetInstance();
    int32_t err = volumeStatsManager->GetTotalSizeOfVolume(volumeUuid, totalSize);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("VolumeStorageStatusService::GetTotalSizeOfVolume", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetBundleStats(const std::string &pkgName, BundleStats &bundleStats,
                                       int32_t appIndex, uint32_t statFlag)
{
#ifdef STORAGE_STATISTICS_MANAGER
    int32_t err = DelayedSingleton<StorageStatusService>::GetInstance()->GetBundleStats(pkgName, bundleStats,
        appIndex, statFlag);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageStatusService::GetBundleStats", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageManager::GetSystemSize(int64_t &systemSize)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGD("StorageManger::getSystemSize start");
    int32_t err = DelayedSingleton<StorageTotalStatusService>::GetInstance()->GetSystemSize(systemSize);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageTotalStatusService::GetSystemSize", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageManager::GetTotalSize(int64_t &totalSize)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGD("StorageManger::getTotalSize start");
    int32_t err = DelayedSingleton<StorageTotalStatusService>::GetInstance()->GetTotalSize(totalSize);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageTotalStatusService::GetTotalSize", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageManager::GetFreeSize(int64_t &freeSize)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGD("StorageManger::getFreeSize start");
    int32_t err = DelayedSingleton<StorageTotalStatusService>::GetInstance()->GetFreeSize(freeSize);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageTotalStatusService::GetFreeSize", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageManager::GetUserStorageStats(StorageStats &storageStats)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGD("StorageManger::GetUserStorageStats start");
    int32_t err = DelayedSingleton<StorageStatusService>::GetInstance()->GetUserStorageStats(storageStats);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageStatusService::GetUserStorageStats", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageManager::GetUserStorageStats(int32_t userId, StorageStats &storageStats)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGD("StorageManger::GetUserStorageStats start");
    int32_t err = DelayedSingleton<StorageStatusService>::GetInstance()->GetUserStorageStats(userId, storageStats);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageStatusService::GetUserStorageStats", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetCurrentBundleStats(BundleStats &bundleStats, uint32_t statFlag)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGD("StorageManger::GetCurrentBundleStats start");
    int32_t err = DelayedSingleton<StorageStatusService>::GetInstance()->GetCurrentBundleStats(bundleStats, statFlag);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageStatusService::GetCurrentBundleStats", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageManager::NotifyVolumeCreated(const VolumeCore& vc)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::NotifyVolumeCreated start, volumeId: %{public}s", vc.GetId().c_str());
    DelayedSingleton<VolumeManagerService>::GetInstance()->OnVolumeCreated(vc);
#endif

    return E_OK;
}

int32_t StorageManager::NotifyVolumeMounted(const std::string &volumeId, const std::string &fsTypeStr,
    const std::string &fsUuid, const std::string &path, const std::string &description)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::NotifyVolumeMounted start, fsType is %{public}s.", fsTypeStr.c_str());
    DelayedSingleton<VolumeManagerService>::GetInstance()->OnVolumeMounted(volumeId, fsTypeStr, fsUuid, path,
        description);
#endif

    return E_OK;
}

OHOS::StorageManager::VolumeState StorageManager::UintToState(uint32_t state)
{
    switch (state) {
        case UNMOUNTED:
            return OHOS::StorageManager::VolumeState::UNMOUNTED;
        case CHECKING:
            return OHOS::StorageManager::VolumeState::CHECKING;
        case MOUNTED:
            return OHOS::StorageManager::VolumeState::MOUNTED;
        case EJECTING:
            return OHOS::StorageManager::VolumeState::EJECTING;
        case REMOVED:
            return OHOS::StorageManager::VolumeState::REMOVED;
        case BAD_REMOVAL:
            return OHOS::StorageManager::VolumeState::BAD_REMOVAL;
        default:
            return OHOS::StorageManager::VolumeState::UNMOUNTED;
    }
}

int32_t StorageManager::NotifyVolumeStateChanged(const std::string& volumeId, uint32_t state)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::NotifyVolumeStateChanged start");
    OHOS::StorageManager::VolumeState stateService = UintToState(state);
    DelayedSingleton<VolumeManagerService>::GetInstance()->OnVolumeStateChanged(volumeId, stateService);
#endif

    return E_OK;
}

int32_t StorageManager::Mount(const std::string &volumeId)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::Mount start");
    int result = DelayedSingleton<VolumeManagerService>::GetInstance()->Mount(volumeId);
    if (result != E_OK) {
        StorageRadar::ReportVolumeOperation("VolumeManagerService::Mount", result);
    }
    return result;
#else
    return E_OK;
#endif
}

int32_t StorageManager::Unmount(const std::string &volumeId)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::Unmount start");
    int result = DelayedSingleton<VolumeManagerService>::GetInstance()->Unmount(volumeId);
    if (result != E_OK) {
        StorageRadar::ReportVolumeOperation("VolumeManagerService::Unmount", result);
    }
    return result;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetAllVolumes(std::vector<VolumeExternal> &vecOfVol)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::GetAllVolumes start");
    vecOfVol = DelayedSingleton<VolumeManagerService>::GetInstance()->GetAllVolumes();
#endif

    return E_OK;
}

int32_t StorageManager::NotifyDiskCreated(const Disk& disk)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManager::NotifyDiskCreated start, diskId: %{public}s", disk.GetDiskId().c_str());
    std::shared_ptr<DiskManagerService> diskManager = DelayedSingleton<DiskManagerService>::GetInstance();
    diskManager->OnDiskCreated(disk);
#endif

    return E_OK;
}

int32_t StorageManager::NotifyDiskDestroyed(const std::string &diskId)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManager::NotifyDiskDestroyed start, diskId: %{public}s", diskId.c_str());
    std::shared_ptr<DiskManagerService> diskManager = DelayedSingleton<DiskManagerService>::GetInstance();
    diskManager->OnDiskDestroyed(diskId);
#endif

    return E_OK;
}

int32_t StorageManager::Partition(const std::string &diskId, int32_t type)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManager::Partition start, diskId: %{public}s", diskId.c_str());
    std::shared_ptr<DiskManagerService> diskManager = DelayedSingleton<DiskManagerService>::GetInstance();
    int32_t err = diskManager->Partition(diskId, type);
    if (err != E_OK) {
        StorageRadar::ReportVolumeOperation("DiskManagerService::Partition", err);
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetAllDisks(std::vector<Disk> &vecOfDisk)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::GetAllDisks start");
    vecOfDisk = DelayedSingleton<DiskManagerService>::GetInstance()->GetAllDisks();
#endif

    return E_OK;
}

int32_t StorageManager::GetVolumeByUuid(const std::string &fsUuid, VolumeExternal &vc)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::GetVolumeByUuid start, uuid: %{public}s",
        GetAnonyString(fsUuid).c_str());
    int32_t err = DelayedSingleton<VolumeManagerService>::GetInstance()->GetVolumeByUuid(fsUuid, vc);
    if (err != E_OK) {
        StorageRadar::ReportVolumeOperation("VolumeManagerService::GetVolumeByUuid", err);
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetVolumeById(const std::string &volumeId, VolumeExternal &vc)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::GetVolumeById start, volId: %{public}s", volumeId.c_str());
    int32_t err = DelayedSingleton<VolumeManagerService>::GetInstance()->GetVolumeById(volumeId, vc);
    if (err != E_OK) {
        StorageRadar::ReportVolumeOperation("VolumeManagerService::GetVolumeById", err);
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::SetVolumeDescription(const std::string &fsUuid, const std::string &description)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::SetVolumeDescription start, uuid: %{public}s",
        GetAnonyString(fsUuid).c_str());
    int32_t err = DelayedSingleton<VolumeManagerService>::GetInstance()->SetVolumeDescription(fsUuid, description);
    if (err != E_OK) {
        StorageRadar::ReportVolumeOperation("VolumeManagerService::SetVolumeDescription", err);
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::Format(const std::string &volumeId, const std::string &fsType)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::Format start, volumeId: %{public}s, fsType: %{public}s", volumeId.c_str(), fsType.c_str());
    int32_t err = DelayedSingleton<VolumeManagerService>::GetInstance()->Format(volumeId, fsType);
    if (err != E_OK) {
        StorageRadar::ReportVolumeOperation("VolumeManagerService::Format", err);
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetDiskById(const std::string &diskId, Disk &disk)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::GetDiskById start, diskId: %{public}s", diskId.c_str());
    int32_t err = DelayedSingleton<DiskManagerService>::GetInstance()->GetDiskById(diskId, disk);
    if (err != E_OK) {
        StorageRadar::ReportVolumeOperation("DiskManagerService::GetDiskById", err);
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::GenerateUserKeys(uint32_t userId, uint32_t flags)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u, flags:  %{public}u", userId, flags);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->GenerateUserKeys(userId, flags);
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::QueryUsbIsInUse(const std::string &diskPath, bool &isInUse)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::QueryUsbIsInUse diskPath: %{public}s", diskPath.c_str());
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->QueryUsbIsInUse(diskPath, isInUse);
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageManager::DeleteUserKeys(uint32_t userId)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->DeleteUserKeys(userId);
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                                       const std::vector<uint8_t> &token,
                                       const std::vector<uint8_t> &oldSecret,
                                       const std::vector<uint8_t> &newSecret)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->UpdateUserAuth(userId, secureUid, token, oldSecret, newSecret);
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                                     const std::vector<uint8_t> &newSecret,
                                                     uint64_t secureUid,
                                                     uint32_t userId,
                                                     const std::vector<std::vector<uint8_t>> &plainText)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->UpdateUseAuthWithRecoveryKey(authToken, newSecret, secureUid, userId, plainText);
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::ActiveUserKey(uint32_t userId,
                                      const std::vector<uint8_t> &token,
                                      const std::vector<uint8_t> &secret)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->ActiveUserKey(userId, token, secret);
    if (err == E_OK) {
        int32_t ret = -1;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ret = AppSpawnClientSendUserLockStatus(userId, DECRYPTED);
        }
        LOGE("Send DECRYPTED status: userId: %{public}d, err is %{public}d", userId, ret);
        StorageRadar::ReportActiveUserKey("AppSpawnClientSendUserLockStatus:DECRYPT", userId, ret, "EL2-EL5");
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::InactiveUserKey(uint32_t userId)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->InactiveUserKey(userId);
    int32_t ret = -1;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ret = AppSpawnClientSendUserLockStatus(userId, ENCRYPTED);
    }
    LOGE("send encrypted status: userId: %{public}d, err is %{public}d", userId, ret);
    StorageRadar::ReportActiveUserKey("AppSpawnClientSendUserLockStatus:ENCRYPT", userId, ret, "EL2-EL5");
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::LockUserScreen(uint32_t userId)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->LockUserScreen(userId);
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->GetFileEncryptStatus(userId, isEncrypted, needCheckDirMount);
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetUserNeedActiveStatus(uint32_t userId, bool &needActive)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->GetUserNeedActiveStatus(userId, needActive);
#else
    return E_OK;
#endif
}

int32_t StorageManager::UnlockUserScreen(uint32_t userId,
                                         const std::vector<uint8_t> &token,
                                         const std::vector<uint8_t> &secret)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->UnlockUserScreen(userId, token, secret);
#else
    return E_OK;
#endif
}

int32_t StorageManager::GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->GetLockScreenStatus(userId, lockScreenStatus);
#else
    return E_OK;
#endif
}

int32_t StorageManager::GenerateAppkey(uint32_t hashId, uint32_t userId, std::string &keyId, bool needReSet)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("hashId: %{public}u", hashId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->GenerateAppkey(hashId, userId, keyId, needReSet);
#else
    return E_OK;
#endif
}

int32_t StorageManager::DeleteAppkey(const std::string &keyId)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("DeleteAppkey enter");
    LOGI("keyId :  %{public}s", keyId.c_str());
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->DeleteAppkey(keyId);
#else
    return E_OK;
#endif
}

int32_t StorageManager::CreateRecoverKey(uint32_t userId,
                                         uint32_t userType,
                                         const std::vector<uint8_t> &token,
                                         const std::vector<uint8_t> &secret)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("CreateRecoverKey enter");
    LOGI("UserId :  %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->CreateRecoverKey(userId, userType, token, secret);
#else
    return E_OK;
#endif
}

int32_t StorageManager::SetRecoverKey(const std::vector<uint8_t> &key)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("SetRecoverKey enter");
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    return fsCrypto->SetRecoverKey(key);
#else
    return E_OK;
#endif
}

int32_t StorageManager::ResetSecretWithRecoveryKey(uint32_t userId, uint32_t rkType, const std::vector<uint8_t> &key)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("ResetSecretWithRecoveryKey UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->ResetSecretWithRecoveryKey(userId, rkType, key);
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::UpdateKeyContext(uint32_t userId, bool needRemoveTmpKey)
{
#ifdef USER_CRYPTO_MANAGER
    LOGI("UserId: %{public}u", userId);
    std::shared_ptr<FileSystemCrypto> fsCrypto = DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t err = fsCrypto->UpdateKeyContext(userId, needRemoveTmpKey);
    return err;
#else
    return E_OK;
#endif
}

std::vector<int32_t> StorageManager::CreateShareFile(const StorageFileRawData &rawData,
    uint32_t tokenId, uint32_t flag)
{
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->CreateShareFile(rawData, tokenId, flag);
}

int32_t StorageManager::DeleteShareFile(uint32_t tokenId, const StorageFileRawData &rawData)
{
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->DeleteShareFile(tokenId, rawData);
}

int32_t StorageManager::SetBundleQuota(const std::string &bundleName, int32_t uid,
    const std::string &bundleDataDirPath, int32_t limitSizeMb)
{
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->SetBundleQuota(bundleName, uid, bundleDataDirPath, limitSizeMb);
}

int32_t StorageManager::GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
    const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
    std::vector<int64_t> &incPkgFileSizes)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGI("StorageManger::GetBundleStatsForIncrease start");
    int32_t err = DelayedSingleton<StorageStatusService>::GetInstance()->GetBundleStatsForIncrease(userId,
        bundleNames, incrementalBackTimes, pkgFileSizes, incPkgFileSizes);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageStatusService::GetBundleStatsForIncrease", userId, err,
            "setting");
    }
    return err;
#else
    return E_OK;
#endif
}


int32_t StorageManager::GetUserStorageStatsByType(int32_t userId, StorageStats &storageStats, const std::string &type)
{
#ifdef STORAGE_STATISTICS_MANAGER
    LOGI("StorageManger::GetUserStorageStatsByType start");
    int32_t err = DelayedSingleton<StorageStatusService>::GetInstance()->GetUserStorageStatsByType(userId,
        storageStats, type);
    if (err != E_OK) {
        StorageRadar::ReportGetStorageStatus("StorageStatusService::GetUserStorageStatsByType", DEFAULT_USERID, err,
            "setting");
    }
    return err;
#else
    return E_OK;
#endif
}

int32_t StorageManager::UpdateMemoryPara(int32_t size, int32_t &oldSize)
{
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->UpdateMemoryPara(size, oldSize);
}

int32_t StorageManager::MountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    LOGI("StorageManager::MountDfsDocs start.");
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->MountDfsDocs(userId, relativePath, networkId, deviceId);
}

int32_t StorageManager::UMountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    LOGI("StorageManager::UMountDfsDocs start.");
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->UMountDfsDocs(userId, relativePath, networkId, deviceId);
}

int32_t StorageManager::NotifyMtpMounted(const std::string &id, const std::string &path, const std::string &desc,
                                         const std::string &uuid)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::NotifyMtpMounted start, id: %{public}s, path: %{public}s, uuid: %{public}s", id.c_str(),
         path.c_str(), GetAnonyString(uuid).c_str());
    DelayedSingleton<VolumeManagerService>::GetInstance()->NotifyMtpMounted(id, path, desc, uuid);
#endif
    return E_OK;
}

int32_t StorageManager::NotifyMtpUnmounted(const std::string &id, const std::string &path, bool isBadRemove)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageManger::NotifyMtpUnmounted start, id: %{public}s, path: %{public}s", id.c_str(), path.c_str());
    DelayedSingleton<VolumeManagerService>::GetInstance()->NotifyMtpUnmounted(id, path, isBadRemove);
#endif
    return E_OK;
}

int32_t StorageManager::MountMediaFuse(int32_t userId, int32_t &devFd)
{
#ifdef STORAGE_SERVICE_MEDIA_FUSE
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->MountMediaFuse(userId, devFd);
#else
    return E_OK;
#endif
}

int32_t StorageManager::UMountMediaFuse(int32_t userId)
{
#ifdef STORAGE_SERVICE_MEDIA_FUSE
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->UMountMediaFuse(userId);
#else
    return E_OK;
#endif
}

int32_t StorageManager::MountFileMgrFuse(int32_t userId, const std::string &path, int32_t &fuseFd)
{
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->MountFileMgrFuse(userId, path, fuseFd);
}

int32_t StorageManager::UMountFileMgrFuse(int32_t userId, const std::string &path)
{
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->UMountFileMgrFuse(userId, path);
}

int32_t StorageManager::IsFileOccupied(const std::string &path, const std::vector<std::string> &inputList,
    std::vector<std::string> &outputList, bool &isOccupy)
{
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->IsFileOccupied(path, inputList, outputList, isOccupy);
}

int32_t StorageManager::MountDisShareFile(int32_t userId, const std::map<std::string, std::string> &shareFiles)
{
    if (userId <= 0) {
        LOGE("mount share file, userId %{public}d is invalid.", userId);
        return E_PARAMS_INVALID;
    }
    for (const auto &item : shareFiles) {
        if (item.first.find("..") != std::string::npos || item.second.find("..") != std::string::npos) {
            LOGE("mount share file, shareFiles<%{public}s, %{public}s> is invalid.",
                 item.first.c_str(), item.second.c_str());
            return E_PARAMS_INVALID;
        }
    }
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->MountDisShareFile(userId, shareFiles);
}

int32_t StorageManager::UMountDisShareFile(int32_t userId, const std::string &networkId)
{
    if (userId <= 0) {
        LOGE("umount share file, userId %{public}d is invalid.", userId);
        return E_PARAMS_INVALID;
    }
    if (networkId.find("..") != std::string::npos) {
        LOGE("umount share file, networkId %{public}s is invalid.", networkId.c_str());
        return E_PARAMS_INVALID;
    }
    std::shared_ptr<StorageDaemonCommunication> sdCommunication;
    sdCommunication = DelayedSingleton<StorageDaemonCommunication>::GetInstance();
    return sdCommunication->UMountDisShareFile(userId, networkId);
}
}
}