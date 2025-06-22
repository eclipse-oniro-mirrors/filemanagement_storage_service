/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_STORAGE_MANAGER_STORAGE_MANAGER_PROVIDER_H
#define OHOS_STORAGE_MANAGER_STORAGE_MANAGER_PROVIDER_H

#include "istorage_manager.h"
#include "storage_manager.h"
#include "storage_manager_stub.h"
#include "system_ability.h"
#include "storage_file_raw_data.h"

namespace OHOS {
namespace StorageManager {
class StorageManagerProvider : public SystemAbility, public StorageManagerStub {
    DECLARE_SYSTEM_ABILITY(StorageManagerProvider)
public:
    StorageManagerProvider(int32_t saID, bool runOnCreate = true)
        : SystemAbility(saID, runOnCreate), manager_(std::make_shared<StorageManager>()) {};
    ~StorageManagerProvider() = default;

    void OnStart() override;
    void OnStop() override;

    int32_t PrepareAddUser(int32_t userId, uint32_t flags) override;
    int32_t RemoveUser(int32_t userId, uint32_t flags) override;
    int32_t PrepareStartUser(int32_t userId) override;
    int32_t StopUser(int32_t userId) override;
    int32_t CompleteAddUser(int32_t userId) override;

    int32_t GetFreeSizeOfVolume(const std::string &volumeUuid, int64_t &freeSize) override;
    int32_t GetTotalSizeOfVolume(const std::string &volumeUuid, int64_t &totalSize) override;
    int32_t GetBundleStats(const std::string &pkgName,
                           BundleStats &bundleStats,
                           int32_t appIndex,
                           uint32_t statFlag) override;
    int32_t GetSystemSize(int64_t &systemSize) override;
    int32_t GetTotalSize(int64_t &totalSize) override;
    int32_t GetFreeSize(int64_t &freeSize) override;
    int32_t GetUserStorageStats(StorageStats &storageStats) override;
    int32_t GetUserStorageStats(int32_t userId, StorageStats &storageStats) override;
    int32_t GetCurrentBundleStats(BundleStats &bundleStats, uint32_t statFlag) override;
    int32_t GetUserStorageStatsByType(int32_t userId, StorageStats &storageStats, const std::string &type) override;

    int32_t NotifyVolumeCreated(const VolumeCore &vc) override;
    int32_t NotifyVolumeMounted(const std::string &volumeId,
                                const std::string &fsTypeStr,
                                const std::string &fsUuid,
                                const std::string &path,
                                const std::string &description) override;
    int32_t NotifyVolumeDamaged(const std::string &volumeId,
                                const std::string &fsTypeStr,
                                const std::string &fsUuid,
                                const std::string &path,
                                const std::string &description) override;
    int32_t NotifyVolumeStateChanged(const std::string &volumeId, uint32_t state) override;

    int32_t Mount(const std::string &volumeId) override;
    int32_t Unmount(const std::string &volumeId) override;
    int32_t TryToFix(const std::string &volumeId) override;

    int32_t GetAllVolumes(std::vector<VolumeExternal> &vecOfVol) override;

    int32_t NotifyDiskCreated(const Disk &disk) override;
    int32_t NotifyDiskDestroyed(const std::string &diskId) override;
    int32_t Partition(const std::string &diskId, int32_t type) override;
    int32_t GetAllDisks(std::vector<Disk> &vecOfDisk) override;

    int32_t GetVolumeByUuid(const std::string &fsUuid, VolumeExternal &vc) override;
    int32_t GetVolumeById(const std::string &volumeId, VolumeExternal &vc) override;
    int32_t SetVolumeDescription(const std::string &fsUuid, const std::string &description) override;
    int32_t Format(const std::string &volumeId, const std::string &fsType) override;
    int32_t GetDiskById(const std::string &diskId, Disk &disk) override;
    int32_t QueryUsbIsInUse(const std::string &diskPath, bool &isInUse) override;

    int32_t NotifyMtpMounted(const std::string &id,
                             const std::string &path,
                             const std::string &desc,
                             const std::string &uuid) override;
    int32_t NotifyMtpUnmounted(const std::string &id, const std::string &path, bool isBadRemove) override;

    // fscrypt api
    int32_t GenerateUserKeys(uint32_t userId, uint32_t flags) override;
    int32_t DeleteUserKeys(uint32_t userId) override;
    int32_t UpdateUserAuth(uint32_t userId,
                           uint64_t secureUid,
                           const std::vector<uint8_t> &token,
                           const std::vector<uint8_t> &oldSecret,
                           const std::vector<uint8_t> &newSecret) override;
    int32_t UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                         const std::vector<uint8_t> &newSecret,
                                         uint64_t secureUid,
                                         uint32_t userId,
                                         const std::vector<std::vector<uint8_t>> &plainText) override;
    int32_t ActiveUserKey(uint32_t userId,
                          const std::vector<uint8_t> &token,
                          const std::vector<uint8_t> &secret) override;
    int32_t InactiveUserKey(uint32_t userId) override;
    int32_t UpdateKeyContext(uint32_t userId, bool needRemoveTmpKey = false) override;
    int32_t LockUserScreen(uint32_t userId) override;
    int32_t UnlockUserScreen(uint32_t userId,
                             const std::vector<uint8_t> &token,
                             const std::vector<uint8_t> &secret) override;
    int32_t GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus) override;
    int32_t GenerateAppkey(uint32_t hashId, uint32_t userId, std::string &keyId, bool needReSet = false) override;
    int32_t DeleteAppkey(const std::string &keyId) override;
    int32_t GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount = false) override;
    int32_t GetUserNeedActiveStatus(uint32_t userId, bool &needActive) override;
    int32_t CreateRecoverKey(uint32_t userId,
                             uint32_t userType,
                             const std::vector<uint8_t> &token,
                             const std::vector<uint8_t> &secret) override;
    int32_t SetRecoverKey(const std::vector<uint8_t> &key) override;
    int32_t ResetSecretWithRecoveryKey(uint32_t userId, uint32_t rkType, const std::vector<uint8_t> &key) override;

    // app file share api
    int32_t CreateShareFile(const StorageFileRawData &rawData,
                            uint32_t tokenId,
                            uint32_t flag,
                            std::vector<int32_t> &funcResult) override;
    int32_t DeleteShareFile(uint32_t tokenId, const StorageFileRawData &rawData) override;

    int32_t SetBundleQuota(const std::string &bundleName,
                           int32_t uid,
                           const std::string &bundleDataDirPath,
                           int32_t limitSizeMb) override;
    int32_t UpdateMemoryPara(int32_t size, int32_t &oldSize) override;

    // dfs service
    int32_t MountDfsDocs(int32_t userId,
                         const std::string &relativePath,
                         const std::string &networkId,
                         const std::string &deviceId) override;
    int32_t UMountDfsDocs(int32_t userId,
                          const std::string &relativePath,
                          const std::string &networkId,
                          const std::string &deviceId) override;

    // media fuse
    int32_t MountMediaFuse(int32_t userId, int32_t &devFd) override;
    int32_t UMountMediaFuse(int32_t userId) override;
    // file mgr fuse
    int32_t MountFileMgrFuse(int32_t userId, const std::string &path, int32_t &fuseFd) override;
    int32_t UMountFileMgrFuse(int32_t userId, const std::string &path) override;
    int32_t IsFileOccupied(const std::string &path,
                           const std::vector<std::string> &inputList,
                           std::vector<std::string> &outputList,
                           bool &isOccupy) override;
    // reset user event record api
    void ResetUserEventRecord(int32_t userId);
    int32_t MountDisShareFile(int32_t userId, const std::map<std::string, std::string> &shareFiles) override;
    int32_t UMountDisShareFile(int32_t userId, const std::string &networkId) override;
    int32_t InactiveUserPublicDirKey(uint32_t userId) override;
private:
    StorageManagerProvider();
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void SetPriority();
    static sptr<StorageManagerProvider> instance_;
    std::shared_ptr<OHOS::StorageManager::StorageManager> manager_;
    static std::mutex instanceLock_;
    std::mutex mutex_;
};
} // namespace StorageManager
} // namespace OHOS

#endif // OHOS_STORAGE_MANAGER_STORAGE_MANAGER_H
