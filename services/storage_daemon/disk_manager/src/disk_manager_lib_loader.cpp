/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "disk_manager/disk_manager_lib_loader.h"

#include <dlfcn.h>
#include <unistd.h>

#include <chrono>

#include "storage_service_errno.h"
#include "storage_service_log.h"

namespace OHOS {
namespace StorageDaemon {
namespace {
constexpr const char *DISK_MGR_SO_NAME = "libstorage_disk_mgr_ext.z.so";
constexpr const char *DISK_MGR_GET_API_FUNC = "DiskMgrGetApi";
constexpr int32_t MAX_DLOPEN_RETRY = 3;
constexpr useconds_t RETRY_INTERVAL_US = 100 * 1000; // 100ms
constexpr int64_t RETRY_COOLDOWN_MS = 1000; // min interval between failed load rounds
}

DiskManagerLibLoader::~DiskManagerLibLoader()
{
    Deinit();
}

bool DiskManagerLibLoader::EnsureLoaded()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (api_ != nullptr) {
        return true;
    }
    auto now = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAttemptTime_).count();
    if (elapsedMs < RETRY_COOLDOWN_MS) {
        return false;
    }
    lastAttemptTime_ = now;
    Init();
    return api_ != nullptr;
}

void DiskManagerLibLoader::Init()
{
    if (diskMgrHandle_ != nullptr) {
        LOGW("[L2:DiskManagerLibLoader] Init: already loaded, skip");
        return;
    }
    for (int32_t retry = 0; retry < MAX_DLOPEN_RETRY; retry++) {
        diskMgrHandle_ = dlopen(DISK_MGR_SO_NAME, RTLD_LAZY);
        if (diskMgrHandle_ != nullptr) {
            break;
        }
        LOGW("[L2:DiskManagerLibLoader] Init: dlopen retry %{public}d failed: %{public}s",
             retry, dlerror());
        if (retry < MAX_DLOPEN_RETRY - 1) {
            usleep(RETRY_INTERVAL_US);
        }
    }
    if (diskMgrHandle_ == nullptr) {
        LOGE("[L2:DiskManagerLibLoader] Init: dlopen %{public}s failed after %{public}d retries",
             DISK_MGR_SO_NAME, MAX_DLOPEN_RETRY);
        return;
    }

    dlerror();
    using GetApiFunc = const DiskMgrApi *(*)();
    auto getApi = reinterpret_cast<GetApiFunc>(
        dlsym(diskMgrHandle_, DISK_MGR_GET_API_FUNC));
    if (dlerror() != nullptr || getApi == nullptr) {
        LOGE("[L2:DiskManagerLibLoader] Init: dlsym %{public}s failed", DISK_MGR_GET_API_FUNC);
        Deinit();
        return;
    }

    api_ = getApi();
    if (api_ == nullptr) {
        LOGE("[L2:DiskManagerLibLoader] Init: DiskMgrGetApi returned null");
        Deinit();
        return;
    }

    LOGI("[L2:DiskManagerLibLoader] Init: disk_mgr_ext loaded on demand");
}

void DiskManagerLibLoader::Deinit()
{
    if (diskMgrHandle_ != nullptr) {
        dlclose(diskMgrHandle_);
        diskMgrHandle_ = nullptr;
    }
    api_ = nullptr;
}

int32_t DiskManagerLibLoader::QueryUsbIsInUse(const std::string &diskPath, bool &isInUse)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->QueryUsbIsInUse(diskPath, isInUse);
}

int32_t DiskManagerLibLoader::CreateBlockDeviceNode(const std::string &devPath, uint32_t mode,
                                                    int32_t major, int32_t minor)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->CreateBlockDeviceNode(devPath, mode, major, minor);
}

int32_t DiskManagerLibLoader::DestroyBlockDeviceNode(const std::string &devPath)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->DestroyBlockDeviceNode(devPath);
}

int32_t DiskManagerLibLoader::ReadPartitionTable(const std::string &devPath, std::string &output,
                                                 int32_t &maxVolume)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->ReadPartitionTable(devPath, output, maxVolume);
}

int32_t DiskManagerLibLoader::QueryCDStatus(const std::string &devPath, int32_t &status)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->QueryCDStatus(devPath, status);
}

int32_t DiskManagerLibLoader::Partition(const std::string &diskPath, const std::string &partitionType)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->Partition(diskPath, partitionType);
}

int32_t DiskManagerLibLoader::GetPartitionTableInfo(const std::string &devPath, std::string &execRet)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->GetPartitionTableInfo(devPath, execRet);
}

int32_t DiskManagerLibLoader::CreatePartition(const std::string &devPath, int32_t partitionNum,
                                              int64_t startSector, int64_t endSector, const std::string &typeCode)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->CreatePartition(devPath, partitionNum, startSector, endSector, typeCode);
}

int32_t DiskManagerLibLoader::DeletePartitionInfo(const std::string &devPath, const std::string &diskId,
                                                  int32_t partitionNum)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->DeletePartitionInfo(devPath, diskId, partitionNum);
}

int32_t DiskManagerLibLoader::FormatPartition(const std::string &devPath, const std::string &fsType,
                                              const std::string &volumeName,
                                              const std::vector<std::string> &cmd, bool quickFormat)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->FormatPartition(devPath, fsType, volumeName, cmd, quickFormat);
}

int32_t DiskManagerLibLoader::Erase(const std::string &devPath)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->Erase(devPath);
}

int32_t DiskManagerLibLoader::Eject(const std::string &devName)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->Eject(devName);
}

int32_t DiskManagerLibLoader::ParseBurnOptions(const std::string &burnOptions, BurnOptions &parsedOptions)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->ParseBurnOptions(burnOptions, parsedOptions);
}

int32_t DiskManagerLibLoader::ValidateBurnOptions(const BurnOptions &options)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->ValidateBurnOptions(options);
}

int32_t DiskManagerLibLoader::GetVolumeOpProcess(const std::string &volId, int32_t &progressPct)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->GetVolumeOpProcess(volId, progressPct);
}

int32_t DiskManagerLibLoader::VerifyBurnData(const std::string &devPath, int32_t verifyType)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->VerifyBurnData(devPath, verifyType);
}

int32_t DiskManagerLibLoader::GetCapacity(const std::string &devPath, int64_t &totalSize, int64_t &freeSize)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->GetCapacity(devPath, totalSize, freeSize);
}

uint64_t DiskManagerLibLoader::GetDiskPowerCycleCount(const std::string &devName)
{
    if (!EnsureLoaded()) {
        return 0;
    }
    return api_->GetDiskPowerCycleCount(devName);
}

uint64_t DiskManagerLibLoader::GetDiskPowerOnHours(const std::string &devName)
{
    if (!EnsureLoaded()) {
        return 0;
    }
    return api_->GetDiskPowerOnHours(devName);
}

int32_t DiskManagerLibLoader::MountVolume(const std::string &devPath, const std::string &mountPath,
                                          const std::string &fsType, unsigned long mountFlags,
                                          const std::string &mountData)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->MountVolume(devPath, mountPath, fsType, mountFlags, mountData);
}

int32_t DiskManagerLibLoader::UnmountVolume(const std::string &mountPath, const std::string &fsType, bool force)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->UnmountVolume(mountPath, fsType, force);
}

int32_t DiskManagerLibLoader::FormatVolume(const std::string &devPath, const std::string &diskPath,
                                           const std::string &fsType, const std::string &partitionType,
                                           int32_t partitionNum)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->FormatVolume(devPath, diskPath, fsType, partitionType, partitionNum);
}

int32_t DiskManagerLibLoader::CheckVolume(const std::string &devPath, const std::string &fsType)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->CheckVolume(devPath, fsType);
}

int32_t DiskManagerLibLoader::RepairVolume(const std::string &devPath, const std::string &fsType)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->RepairVolume(devPath, fsType);
}

int32_t DiskManagerLibLoader::SetVolumeLabel(const std::string &devPath, const std::string &fsType,
                                             const std::string &label)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->SetVolumeLabel(devPath, fsType, label);
}

int32_t DiskManagerLibLoader::CreateIsoImage(const std::string &devPath, const std::string &filePath,
                                             const std::string &mountPath, const std::string &fsType)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->CreateIsoImage(devPath, filePath, mountPath, fsType);
}

int32_t DiskManagerLibLoader::BurnDisk(const std::string &devPath, const std::string &fsType,
                                       const BurnOptions &burnOptions)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->BurnDisk(devPath, fsType, burnOptions);
}

int32_t DiskManagerLibLoader::ReadMetadata(const std::string &devPath, std::string &uuid,
                                           std::string &type, std::string &label)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->ReadMetadata(devPath, uuid, type, label);
}

int32_t DiskManagerLibLoader::MountFuseDevice(const std::string &mountPath, int &fuseFd)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->MountFuseDevice(mountPath, fuseFd);
}

int32_t DiskManagerLibLoader::CreateDmDevice(const std::string &sourceDevPath, uint64_t startSector,
                                             uint64_t sectorCount, dev_t &dev)
{
    if (!EnsureLoaded()) {
        return E_NOT_SUPPORT;
    }
    return api_->CreateDmDevice(sourceDevPath, startSector, sectorCount, dev);
}

std::vector<BlockInfo> DiskManagerLibLoader::GetDataDisks()
{
    if (!EnsureLoaded()) {
        return {};
    }
    return api_->GetDataDisks();
}

std::vector<BlockInfo> DiskManagerLibLoader::GetExternalDisks(const std::string &devName,
                                                              const std::string &diskId)
{
    if (!EnsureLoaded()) {
        return {};
    }
    return api_->GetExternalDisks(devName, diskId);
}

uint64_t DiskManagerLibLoader::GetDiskSize(const std::string &deviceName)
{
    if (!EnsureLoaded()) {
        return 0;
    }
    return api_->GetDiskSize(deviceName);
}

} // namespace StorageDaemon
} // namespace OHOS
