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
#include "disk_manager/disk_mgr_api.h"
#include "disk_manager/disk/disk_utils.h"
#include "disk_manager/disk/smart_reader.h"
#include "disk_manager/disk/dm_device.h"
#include "disk_manager/disk/scan_device.h"
#include "disk_manager/volume/volume_operator_factory.h"
#include "disk_manager/volume/volume_utils.h"
#include "storage_service_errno.h"

namespace OHOS {
namespace StorageDaemon {

int32_t MountVolumeWrapper(const std::string &devPath, const std::string &mountPath,
                           const std::string &fsType, unsigned long mountFlags, const std::string &mountData)
{
    auto op = VolumeOperatorFactory::CreateOperator(fsType);
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->Mount(devPath, mountPath, mountFlags, mountData);
}

int32_t UnmountVolumeWrapper(const std::string &mountPath, const std::string &fsType, bool force)
{
    auto op = VolumeOperatorFactory::GetGenericOperator();
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->Unmount(mountPath, fsType, force);
}

int32_t FormatVolumeWrapper(const std::string &devPath, const std::string &diskPath,
                            const std::string &fsType, const std::string &partitionType, int32_t partitionNum)
{
    auto op = VolumeOperatorFactory::CreateOperator(fsType);
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->Format(devPath, diskPath, partitionType, partitionNum);
}

int32_t CheckVolumeWrapper(const std::string &devPath, const std::string &fsType)
{
    auto op = VolumeOperatorFactory::CreateOperator(fsType);
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->Check(devPath);
}

int32_t RepairVolumeWrapper(const std::string &devPath, const std::string &fsType)
{
    auto op = VolumeOperatorFactory::CreateOperator(fsType);
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->Repair(devPath);
}

int32_t SetVolumeLabelWrapper(const std::string &devPath, const std::string &fsType, const std::string &label)
{
    auto op = VolumeOperatorFactory::CreateOperator(fsType);
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->SetLabel(devPath, label);
}

int32_t CreateIsoImageWrapper(const std::string &devPath, const std::string &filePath,
                              const std::string &mountPath, const std::string &fsType)
{
    auto op = VolumeOperatorFactory::CreateOperator(fsType);
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->CreateIsoImage(devPath, filePath, mountPath);
}

int32_t BurnDiskWrapper(const std::string &devPath, const std::string &fsType, const BurnOptions &burnOptions)
{
    auto op = VolumeOperatorFactory::CreateOperator(fsType);
    if (op == nullptr) {
        return E_NOT_SUPPORT;
    }
    return op->Burn(devPath, burnOptions);
}

std::vector<BlockInfo> GetDataDisksWrapper()
{
    ScanDevice scanDevice;
    return scanDevice.GetDataDisks();
}

std::vector<BlockInfo> GetExternalDisksWrapper(const std::string &devName, const std::string &diskId)
{
    ScanDevice scanDevice;
    return scanDevice.GetExternalDisks(devName, diskId);
}

uint64_t GetDiskSizeWrapper(const std::string &deviceName)
{
    ScanDevice scanDevice;
    return scanDevice.GetDiskSize(deviceName);
}

int32_t CreateDmDeviceWrapper(const std::string &sourceDevPath, uint64_t startSector,
                              uint64_t sectorCount, dev_t &dev)
{
    DmDevice dmDevice(sourceDevPath, startSector, sectorCount);
    if (!dmDevice.Create()) {
        return E_NOT_SUPPORT;
    }
    dev = dmDevice.GetDeviceDev();
    return E_OK;
}

} // namespace StorageDaemon
} // namespace OHOS

extern "C" const OHOS::StorageDaemon::DiskMgrApi *DiskMgrGetApi(void)
{
    static const OHOS::StorageDaemon::DiskMgrApi api = {
        OHOS::StorageDaemon::DiskUtils::QueryUsbIsInUse,
        OHOS::StorageDaemon::DiskUtils::CreateBlockDeviceNode,
        OHOS::StorageDaemon::DiskUtils::DestroyBlockDeviceNode,
        OHOS::StorageDaemon::DiskUtils::ReadPartitionTable,
        OHOS::StorageDaemon::DiskUtils::QueryCDStatus,
        OHOS::StorageDaemon::DiskUtils::Partition,
        OHOS::StorageDaemon::DiskUtils::GetPartitionTableInfo,
        OHOS::StorageDaemon::DiskUtils::CreatePartition,
        OHOS::StorageDaemon::DiskUtils::DeletePartitionInfo,
        OHOS::StorageDaemon::DiskUtils::FormatPartition,
        OHOS::StorageDaemon::DiskUtils::Erase,
        OHOS::StorageDaemon::DiskUtils::Eject,
        OHOS::StorageDaemon::ParseBurnOptions,
        OHOS::StorageDaemon::ValidateBurnOptions,
        OHOS::StorageDaemon::DiskUtils::GetVolumeOpProcess,
        OHOS::StorageDaemon::DiskUtils::VerifyBurnData,
        OHOS::StorageDaemon::DiskUtils::GetCapacity,
        OHOS::StorageDaemon::SmartReader::GetDiskPowerCycleCount,
        OHOS::StorageDaemon::SmartReader::GetDiskPowerOnHours,
        OHOS::StorageDaemon::MountVolumeWrapper,
        OHOS::StorageDaemon::UnmountVolumeWrapper,
        OHOS::StorageDaemon::FormatVolumeWrapper,
        OHOS::StorageDaemon::CheckVolumeWrapper,
        OHOS::StorageDaemon::RepairVolumeWrapper,
        OHOS::StorageDaemon::SetVolumeLabelWrapper,
        OHOS::StorageDaemon::CreateIsoImageWrapper,
        OHOS::StorageDaemon::BurnDiskWrapper,
        OHOS::StorageDaemon::VolumeUtils::ReadMetadata,
        OHOS::StorageDaemon::VolumeUtils::MountFuseDevice,
        OHOS::StorageDaemon::GetDataDisksWrapper,
        OHOS::StorageDaemon::GetExternalDisksWrapper,
        OHOS::StorageDaemon::GetDiskSizeWrapper,
        OHOS::StorageDaemon::CreateDmDeviceWrapper,
    };
    return &api;
}
