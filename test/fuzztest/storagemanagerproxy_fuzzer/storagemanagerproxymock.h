/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_STORAGEMANAGERPROXYMOCK_H
#define OHOS_STORAGEMANAGERPROXYMOCK_H

#include "storage_manager.h"
#include "storage_service_errno.h"

namespace OHOS {
namespace StorageManager {
class StorageManagerProxyMock final : public IRemoteStub<IStorageManager> {
public:
    int code_ = 0;
    StorageManagerProxyMock() : code_(0) {}
    virtual ~StorageManagerProxyMock() {}
    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return E_OK;
    }

    int32_t PrepareAddUser(int32_t userId, uint32_t flags) override
    {
        return E_OK;
    }

    int32_t RemoveUser(int32_t userId, uint32_t flags) override
    {
        return E_OK;
    }

    int32_t PrepareStartUser(int32_t userId) override
    {
        return E_OK;
    }

    int32_t StopUser(int32_t userId) override
    {
        return E_OK;
    }

    int32_t CompleteAddUser(int32_t userId) override
    {
        return E_OK;
    }

    int32_t GetFreeSizeOfVolume(std::string volumeUuid, int64_t &freeSize) override
    {
        return E_OK;
    }

    int32_t GetTotalSizeOfVolume(std::string volumeUuid, int64_t &totalSize) override
    {
        return E_OK;
    }

    int32_t GetBundleStats(std::string pkgName, BundleStats &bundleStats, int32_t appIndex, uint32_t statFlag) override
    {
        return E_OK;
    }

    int32_t GetSystemSize(int64_t &systemSize) override
    {
        return E_OK;
    }

    int32_t GetTotalSize(int64_t &totalSize) override
    {
        return E_OK;
    }

    int32_t GetFreeSize(int64_t &freeSize) override
    {
        return E_OK;
    }

    int32_t GetUserStorageStats(StorageStats &storageStats) override
    {
        return E_OK;
    }

    int32_t GetUserStorageStats(int32_t userId, StorageStats &storageStats) override
    {
        return E_OK;
    }

    int32_t GetUserStorageStatsByType(int32_t userId, StorageStats &storageStats, std::string type) override
    {
        return E_OK;
    }

    int32_t GetCurrentBundleStats(BundleStats &bundleStats, uint32_t statFlag) override
    {
        return E_OK;
    }

    int32_t NotifyVolumeCreated(VolumeCore vc) override
    {
        return E_OK;
    }

    int32_t NotifyVolumeMounted(std::string volumeId, int fsType, std::string fsUuid,
                                        std::string path, std::string description) override
    {
        return E_OK;
    }

    int32_t NotifyVolumeStateChanged(std::string volumeId, VolumeState state) override
    {
        return E_OK;
    }

    int32_t Mount(std::string volumeId) override
    {
        return E_OK;
    }

    int32_t Unmount(std::string volumeId) override
    {
        return E_OK;
    }

    int32_t GetAllVolumes(std::vector<VolumeExternal> &vecOfVol) override
    {
        return E_OK;
    }

    int32_t NotifyDiskCreated(Disk disk) override
    {
        return E_OK;
    }

    int32_t NotifyDiskDestroyed(std::string diskId) override
    {
        return E_OK;
    }

    int32_t Partition(std::string diskId, int32_t type) override
    {
        return E_OK;
    }

    int32_t GetAllDisks(std::vector<Disk> &vecOfDisk) override
    {
        return E_OK;
    }

    int32_t GetVolumeByUuid(std::string fsUuid, VolumeExternal &vc) override
    {
        return E_OK;
    }

    int32_t GetVolumeById(std::string volumeId, VolumeExternal &vc) override
    {
        return E_OK;
    }

    int32_t SetVolumeDescription(std::string fsUuid, std::string description) override
    {
        return E_OK;
    }

    int32_t Format(std::string volumeId, std::string fsType) override
    {
        return E_OK;
    }

    int32_t GetDiskById(std::string diskId, Disk &disk) override
    {
        return E_OK;
    }

    int32_t GenerateUserKeys(uint32_t userId, uint32_t flags) override
    {
        return E_OK;
    }

    int32_t DeleteUserKeys(uint32_t userId) override
    {
        return E_OK;
    }

    int32_t UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                                   const std::vector<uint8_t> &token,
                                   const std::vector<uint8_t> &oldSecret,
                                   const std::vector<uint8_t> &newSecret) override
    {
        return E_OK;
    }

    int32_t ActiveUserKey(uint32_t userId,
                                  const std::vector<uint8_t> &token,
                                  const std::vector<uint8_t> &secret) override
    {
        return E_OK;
    }

    int32_t InactiveUserKey(uint32_t userId) override
    {
        return E_OK;
    }

    int32_t LockUserScreen(uint32_t userId) override
    {
        return E_OK;
    }

    int32_t UnlockUserScreen(uint32_t userId,
                             const std::vector<uint8_t> &token,
                             const std::vector<uint8_t> &secret) override
    {
        return E_OK;
    }

    int32_t GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus) override
    {
        return E_OK;
    }

    int32_t GenerateAppkey(uint32_t hashId, uint32_t userId, std::string &keyId) override
    {
        return E_OK;
    }

    int32_t DeleteAppkey(const std::string keyId) override
    {
        return E_OK;
    }

    int32_t UpdateKeyContext(uint32_t userId) override
    {
        return E_OK;
    }

    int32_t MountDfsDocs(int32_t userId, const std::string &relativePath,
        const std::string &networkId, const std::string &deviceId) override
    {
        return E_OK;
    }

    int32_t UMountDfsDocs(int32_t userId, const std::string &relativePath,
        const std::string &networkId, const std::string &deviceId) override
    {
        return E_OK;
    }

    std::vector<int32_t> CreateShareFile(const std::vector<std::string> &uriList,
                                                 uint32_t tokenId, uint32_t flag) override
    {
        return std::vector<int32_t>{E_OK};
    }

    int32_t DeleteShareFile(uint32_t tokenId, const std::vector<std::string> &uriList) override
    {
        return E_OK;
    }
    int32_t UpdateMemoryPara(int32_t size, int32_t &oldSize) override
    {
        return E_OK;
    }
    int32_t GetBundleStatsForIncrease(uint32_t userId,
                                      const std::vector<std::string> &bundleNames,
                                      const std::vector<int64_t> &incrementalBackTimes,
                                      std::vector<int64_t> &pkgFileSizes,
                                      std::vector<int64_t> &incPkgFileSizes) override
    {
        return E_OK;
    }
    int32_t GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount = false) override
     {
         return E_OK;
     }
};
} // namespace StorageManager
} // namespace OHOS

#endif //OHOS_STORAGEMANAGERPROXYMOCK_H
