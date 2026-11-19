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
#ifndef OHOS_STORAGE_DAEMON_DISK_MGR_API_H
#define OHOS_STORAGE_DAEMON_DISK_MGR_API_H

#include <cstdint>
#include <string>
#include <sys/types.h>
#include <vector>

#include "block_info.h"

namespace OHOS {
namespace StorageDaemon {

using OHOS::DiskManager::BlockInfo;

struct BurnOptions;

struct DiskMgrApi {
    int32_t (*QueryUsbIsInUse)(const std::string &diskPath, bool &isInUse);
    int32_t (*CreateBlockDeviceNode)(const std::string &devPath, uint32_t mode, int32_t major, int32_t minor);
    int32_t (*DestroyBlockDeviceNode)(const std::string &devPath);
    int32_t (*ReadPartitionTable)(const std::string &devPath, std::string &output, int32_t &maxVolume);
    int32_t (*QueryCDStatus)(const std::string &devPath, int32_t &status);
    int32_t (*Partition)(const std::string &diskPath, const std::string &partitionType);
    int32_t (*GetPartitionTableInfo)(const std::string &devPath, std::string &execRet);
    int32_t (*CreatePartition)(const std::string &devPath, int32_t partitionNum,
                               int64_t startSector, int64_t endSector, const std::string &typeCode);
    int32_t (*DeletePartitionInfo)(const std::string &devPath, const std::string &diskId, int32_t partitionNum);
    int32_t (*FormatPartition)(const std::string &devPath, const std::string &fsType,
                               const std::string &volumeName, const std::vector<std::string> &cmd, bool quickFormat);
    int32_t (*Erase)(const std::string &devPath);
    int32_t (*Eject)(const std::string &devName);
    int32_t (*ParseBurnOptions)(const std::string &burnOptions, BurnOptions &parsedOptions);
    int32_t (*ValidateBurnOptions)(const BurnOptions &options);
    int32_t (*GetVolumeOpProcess)(const std::string &volId, int32_t &progressPct);
    int32_t (*VerifyBurnData)(const std::string &devPath, int32_t verifyType);
    int32_t (*GetCapacity)(const std::string &devPath, int64_t &totalSize, int64_t &freeSize);
    uint64_t (*GetDiskPowerCycleCount)(const std::string &devName);
    uint64_t (*GetDiskPowerOnHours)(const std::string &devName);
    int32_t (*MountVolume)(const std::string &devPath, const std::string &mountPath,
                           const std::string &fsType, unsigned long mountFlags, const std::string &mountData);
    int32_t (*UnmountVolume)(const std::string &mountPath, const std::string &fsType, bool force);
    int32_t (*FormatVolume)(const std::string &devPath, const std::string &diskPath,
                            const std::string &fsType, const std::string &partitionType, int32_t partitionNum);
    int32_t (*CheckVolume)(const std::string &devPath, const std::string &fsType);
    int32_t (*RepairVolume)(const std::string &devPath, const std::string &fsType);
    int32_t (*SetVolumeLabel)(const std::string &devPath, const std::string &fsType, const std::string &label);
    int32_t (*CreateIsoImage)(const std::string &devPath, const std::string &filePath,
                              const std::string &mountPath, const std::string &fsType);
    int32_t (*BurnDisk)(const std::string &devPath, const std::string &fsType, const BurnOptions &burnOptions);
    int32_t (*ReadMetadata)(const std::string &devPath, std::string &uuid, std::string &type, std::string &label);
    int32_t (*MountFuseDevice)(const std::string &mountPath, int &fuseFd);
    std::vector<BlockInfo> (*GetDataDisks)();
    std::vector<BlockInfo> (*GetExternalDisks)(const std::string &devName, const std::string &diskId);
    uint64_t (*GetDiskSize)(const std::string &deviceName);
    int32_t (*CreateDmDevice)(const std::string &sourceDevPath, uint64_t startSector, uint64_t sectorCount, dev_t &dev);
};

} // namespace StorageDaemon
} // namespace OHOS

extern "C" const OHOS::StorageDaemon::DiskMgrApi *DiskMgrGetApi(void);

#endif // OHOS_STORAGE_DAEMON_DISK_MGR_API_H
