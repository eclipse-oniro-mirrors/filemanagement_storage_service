/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "mock/storage_manager_proxy_mock.h"
#include "storage_service_errno.h"

namespace OHOS {
namespace StorageManager {
int32_t StorageManagerProxy::PrepareAddUser(int32_t userId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageManagerProxy::RemoveUser(int32_t userId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageManagerProxy::PrepareStartUser(int32_t userId)
{
    return E_OK;
}

int32_t StorageManagerProxy::StopUser(int32_t userId)
{
    return E_OK;
}

int32_t StorageManagerProxy::CompleteAddUser(int32_t userId)
{
    return E_OK;
}

int32_t StorageManagerProxy::GenerateUserKeys(uint32_t userId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageManagerProxy::DeleteUserKeys(uint32_t userId)
{
    return E_OK;
}

int32_t StorageManagerProxy::UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                                            const std::vector<uint8_t> &token,
                                            const std::vector<uint8_t> &oldSecret,
                                            const std::vector<uint8_t> &newSecret)
{
    return E_OK;
}

int32_t StorageManagerProxy::UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                                          const std::vector<uint8_t> &newSecret,
                                                          uint64_t secureUid,
                                                          uint32_t userId,
                                                          std::vector<std::vector<uint8_t>> &plainText)
{
    return E_OK;
}

int32_t StorageManagerProxy::ActiveUserKey(uint32_t userId,
                                           const std::vector<uint8_t> &token,
                                           const std::vector<uint8_t> &secret)
{
    return E_OK;
}

int32_t StorageManagerProxy::InactiveUserKey(uint32_t userId)
{
    return E_OK;
}

int32_t StorageManagerProxy::LockUserScreen(uint32_t userId)
{
    return E_OK;
}

int32_t StorageManagerProxy::UnlockUserScreen(uint32_t userId,
                                              const std::vector<uint8_t> &token,
                                              const std::vector<uint8_t> &secret)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus)
{
    return E_OK;
}

int32_t StorageManagerProxy::UpdateKeyContext(uint32_t userId)
{
    return E_OK;
}

int32_t StorageManagerProxy::GenerateAppkey(uint32_t hashId, uint32_t userId, std::string &keyId)
{
    return E_OK;
}

int32_t StorageManagerProxy::DeleteAppkey(const std::string keyId)
{
    return E_OK;
}

int32_t StorageManagerProxy::CreateRecoverKey(uint32_t userId,
                                              uint32_t userType,
                                              const std::vector<uint8_t> &token,
                                              const std::vector<uint8_t> &secret)
{
    return E_OK;
}

int32_t StorageManagerProxy::SetRecoverKey(const std::vector<uint8_t> &key)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetFreeSizeOfVolume(std::string volumeUuid, int64_t &freeSize)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetTotalSizeOfVolume(std::string volumeUuid, int64_t &totalSize)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetBundleStats(std::string pkgName, BundleStats &bundleStats, int32_t appIndex)
{
    return E_OK;
}

int32_t StorageManagerProxy::NotifyVolumeCreated(VolumeCore vc)
{
    return E_OK;
}

int32_t StorageManagerProxy::NotifyVolumeMounted(std::string volumeId, int32_t fsType, std::string fsUuid,
    std::string path, std::string description)
{
    return E_OK;
}

int32_t StorageManagerProxy::NotifyVolumeStateChanged(std::string volumeId, VolumeState state)
{
    return E_OK;
}

int32_t StorageManagerProxy::Mount(std::string volumeId)
{
    return E_OK;
}

int32_t StorageManagerProxy::Unmount(std::string volumeId)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetAllVolumes(std::vector<VolumeExternal> &vecOfVol)
{
    return E_OK;
}

int32_t StorageManagerProxy::NotifyDiskCreated(Disk disk)
{
    return E_OK;
}

int32_t StorageManagerProxy::NotifyDiskDestroyed(std::string diskId)
{
    return E_OK;
}

int32_t StorageManagerProxy::Partition(std::string diskId, int32_t type)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetAllDisks(std::vector<Disk> &vecOfDisk)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetSystemSize(int64_t &systemSize)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetTotalSize(int64_t &totalSize)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetFreeSize(int64_t &freeSize)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetUserStorageStats(StorageStats &storageStats)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetUserStorageStats(int32_t userId, StorageStats &storageStats)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetCurrentBundleStats(BundleStats &bundleStats)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetVolumeByUuid(std::string fsUuid, VolumeExternal &vc)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetVolumeById(std::string volumeId, VolumeExternal &vc)
{
    return E_OK;
}

int32_t StorageManagerProxy::SetVolumeDescription(std::string fsUuid, std::string description)
{
    return E_OK;
}

int32_t StorageManagerProxy::Format(std::string volumeId, std::string fsType)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetDiskById(std::string diskId, Disk &disk)
{
    return E_OK;
}

std::vector<int32_t> StorageManagerProxy::CreateShareFile(const std::vector<std::string> &uriList,
                                                          uint32_t tokenId, uint32_t flag)
{
    return {E_OK};
}

int32_t StorageManagerProxy::DeleteShareFile(uint32_t tokenId, const std::vector<std::string> &uriList)
{
    return E_OK;
}
int32_t StorageManagerProxy::SetBundleQuota(const std::string &bundleName, int32_t uid,
    const std::string &bundleDataDirPath, int32_t limitSizeMb)
{
    return E_OK;
}


int32_t StorageManagerProxy::GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
    const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
    std::vector<int64_t> &incPkgFileSizes)
{
    return E_OK;
}

int32_t StorageManagerProxy::UpdateMemoryPara(int32_t size, int32_t &oldSize)
{
    return E_OK;
}

int32_t StorageManagerProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetUserStorageStatsByType(int32_t userId, StorageStats &storageStats, std::string type)
{
    return E_OK;
}

int32_t StorageManagerProxy::MountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    return E_OK;
}

int32_t StorageManagerProxy::UMountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    return E_OK;
}

int32_t StorageManagerProxy::GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount)
{
    return E_OK;
}

int32_t StorageManagerProxy::NotifyMtpMounted(const std::string &id, const std::string &path, const std::string &desc,
                                              const std::string &uuid)
{
    return E_OK;
}
 
int32_t StorageManagerProxy::NotifyMtpUnmounted(const std::string &id, const std::string &path)
{
    return E_OK;
}
} // StorageManager
} // OHOS
