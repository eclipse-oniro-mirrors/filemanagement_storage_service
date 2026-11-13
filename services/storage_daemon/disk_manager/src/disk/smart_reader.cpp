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

#include "disk_manager/disk/smart_reader.h"
#include "storage_service_log.h"

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <linux/hdreg.h>
#include <linux/nvme_ioctl.h>
#include <securec.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

namespace OHOS {
namespace StorageDaemon {

namespace {
constexpr const char *DEV_BLOCK_PREFIX = "/dev/block/";
constexpr const char *NVME_PREFIX = "nvme";

/* NVMe SMART log page 常量（参考 ssd_info.h NVME_LOG_SMART） */
constexpr uint8_t NVME_ADMIN_GET_LOG_PAGE = 0x02;
constexpr uint32_t NVME_NSID_ALL = 0xFFFFFFFF;
constexpr uint32_t NVME_LOG_SMART = 0x02;
constexpr uint32_t NVME_DWORD_SIZE = 4;
constexpr uint32_t IOCTL_TIMEOUT_MS = 5000;
constexpr size_t BITS_PER_BYTE = 8;
constexpr size_t NVME_SMART_LOG_SIZE = 512;

/* ATA SMART 常量（参考 hdd_info.h） */
constexpr uint8_t ATA_SMART_CMD = 0xb0;
constexpr uint8_t ATA_SMART_READ_VALUES = 0xd0;
constexpr uint8_t ATA_SMART_ENABLE = 0xd8;
constexpr uint8_t ATA_SMART_STATUS = 0xda;
constexpr size_t ATA_CDB_HEADER_SIZE = 4;
constexpr size_t ATA_CDB_DATA_SIZE = 512;
constexpr size_t ATA_CDB_TOTAL_SIZE = ATA_CDB_HEADER_SIZE + ATA_CDB_DATA_SIZE;
constexpr size_t ATA_SMART_ATTR_COUNT = 30;
constexpr size_t ATA_SMART_ATTR_SIZE = 12;
constexpr size_t ATA_SMART_VALUES_SIZE = 512;

/* ATA CDB 字段索引（HDIO_DRIVE_CMD 格式：cmd, sector_count, feature, nsect） */
constexpr size_t ATA_CDB_CMD_INDEX = 0;
constexpr size_t ATA_CDB_SECTOR_COUNT_INDEX = 1;
constexpr size_t ATA_CDB_FEATURE_INDEX = 2;
constexpr size_t ATA_CDB_NSECT_INDEX = 3;

/* ATA SMART sector count 值 */
constexpr uint8_t ATA_SMART_ENABLE_SECTOR_COUNT = 1;
constexpr uint8_t ATA_SMART_READ_SECTOR_COUNT = 1;

/* ATA SMART raw value 常量（参考 hdd_info.c CombineRawValue） */
constexpr size_t ATA_RAW_VALUE_SIZE = 6;
constexpr size_t ATA_RAW_VALUE_LAST_BYTE_INDEX = 5;
constexpr size_t ATA_RAW_VALUE_LAST_BYTE_SHIFT = 48;

/* ATA SMART 属性 ID（参考 hdd_info.h SataAttrNameTable） */
constexpr uint8_t ATA_ATTR_POWER_ON_HOURS = 0x09;
constexpr uint8_t ATA_ATTR_POWER_CYCLE_COUNT = 0x0C;

/* NVMe SMART log 结构体字段大小 */
constexpr size_t NVME_TEMP_SIZE = 2;
constexpr size_t NVME_RSVD7_SIZE = 25;
constexpr size_t NVME_TEMP_SENSOR_COUNT = 8;
constexpr size_t NVME_RSVD232_SIZE = 280;
constexpr size_t NVME_SMART_FIELD_SIZE = 16;

/* NVMe cdw10 构造常量 */
constexpr uint32_t NVME_CDW10_NUMD_MASK = 0xFFFF;
constexpr uint32_t NVME_CDW10_NUMD_SHIFT = 16;

/**
 * NVMe SMART log structure（512 字节，参考 ssd_info.h NvmeSmartLog）。
 * powerCycles 和 powerOnHours 均为 16 字节小端整数。
 */
struct NvmeSmartLog {
    uint8_t criticalWarning;
    uint8_t temperature[NVME_TEMP_SIZE];
    uint8_t availSpare;
    uint8_t spareThresh;
    uint8_t percentUsed;
    uint8_t enduGrpCritWarnSumry;
    uint8_t rsvd7[NVME_RSVD7_SIZE];
    uint8_t dataUnitsRead[NVME_SMART_FIELD_SIZE];
    uint8_t dataUnitsWritten[NVME_SMART_FIELD_SIZE];
    uint8_t hostReads[NVME_SMART_FIELD_SIZE];
    uint8_t hostWrites[NVME_SMART_FIELD_SIZE];
    uint8_t ctrlBusyTime[NVME_SMART_FIELD_SIZE];
    uint8_t powerCycles[NVME_SMART_FIELD_SIZE];
    uint8_t powerOnHours[NVME_SMART_FIELD_SIZE];
    uint8_t unsafeShutdowns[NVME_SMART_FIELD_SIZE];
    uint8_t mediaErrors[NVME_SMART_FIELD_SIZE];
    uint8_t numErrLogEntries[NVME_SMART_FIELD_SIZE];
    uint32_t warningTempTime;
    uint32_t criticalCompTime;
    uint16_t tempSensor[NVME_TEMP_SENSOR_COUNT];
    uint32_t thmTemp1TransCount;
    uint32_t thmTemp2TransCount;
    uint32_t thmTemp1TotalTime;
    uint32_t thmTemp2TotalTime;
    uint8_t rsvd232[NVME_RSVD232_SIZE];
};
static_assert(sizeof(NvmeSmartLog) == NVME_SMART_LOG_SIZE, "NvmeSmartLog must be 512 bytes");

/**
 * ATA SMART 属性结构（参考 hdd_info.h AtaSmartAttribute，12 字节，必须 pack(1)）。
 */
#pragma pack(push, 1)
struct AtaSmartAttribute {
    uint8_t id;
    uint16_t statusFlags;
    uint8_t currentValue;
    uint8_t worstValue;
    uint8_t rawValue[ATA_RAW_VALUE_SIZE];
    uint8_t reserved;
};
#pragma pack(pop)
static_assert(sizeof(AtaSmartAttribute) == ATA_SMART_ATTR_SIZE, "AtaSmartAttribute must be 12 bytes");

/**
 * ATA SMART values 结构（参考 hdd_info.h AtaSmartValues，512 字节）。
 */
#pragma pack(push, 1)
struct AtaSmartValues {
    uint16_t revnumber;
    AtaSmartAttribute vendorAttributes[ATA_SMART_ATTR_COUNT];
    uint8_t remaining[ATA_SMART_VALUES_SIZE - sizeof(uint16_t) - ATA_SMART_ATTR_COUNT * sizeof(AtaSmartAttribute)];
};
#pragma pack(pop)
static_assert(sizeof(AtaSmartValues) == ATA_SMART_VALUES_SIZE, "AtaSmartValues must be 512 bytes");

/**
 * @brief 从 16 字节小端缓冲区提取 uint64（取低 8 字节，SMART 值不会超过 uint64 范围）。
 */
uint64_t Le16ToUint64(const uint8_t *data)
{
    uint64_t value = 0;
    for (size_t i = 0; i < sizeof(uint64_t); ++i) {
        value |= static_cast<uint64_t>(data[i]) << (i * BITS_PER_BYTE);
    }
    return value;
}

/**
 * @brief 从 6 字节 raw value 提取 uint64（参考 hdd_info.c CombineRawValue）。
 */
uint64_t CombineRawValue(const uint8_t *raw)
{
    uint64_t value = 0;
    for (size_t i = 0; i < ATA_RAW_VALUE_SIZE - 1; ++i) {
        value |= static_cast<uint64_t>(raw[i]) << (i * BITS_PER_BYTE);
    }
    value |= static_cast<uint64_t>(raw[ATA_RAW_VALUE_LAST_BYTE_INDEX]) << ATA_RAW_VALUE_LAST_BYTE_SHIFT;
    return value;
}

/**
 * @brief 发送 NVMe Get Log Page 命令读 SMART log（参考 ssd_info.c NvmeGetLogCmd）。
 */
int NvmeGetSmartLogCmd(int fd, NvmeSmartLog &log)
{
    struct nvme_admin_cmd cmd = {};
    cmd.opcode = NVME_ADMIN_GET_LOG_PAGE;
    cmd.nsid = NVME_NSID_ALL;
    cmd.addr = reinterpret_cast<uint64_t>(&log);
    cmd.data_len = sizeof(log);

    /* cdw10 构造（参考 ssd_info.c NvmeGetLogCmd，logId 在低位，numdl 在高位） */
    uint32_t numd = (sizeof(log) / NVME_DWORD_SIZE) - 1; /* 127 */
    uint16_t numdl = numd & NVME_CDW10_NUMD_MASK;
    cmd.cdw10 = NVME_LOG_SMART | (static_cast<uint32_t>(numdl) << NVME_CDW10_NUMD_SHIFT);
    cmd.timeout_ms = IOCTL_TIMEOUT_MS;

    return ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
}

/**
 * @brief 发送 ATA SMART 命令（参考 hdd_info.c AtaSmartInfoCmd 等）。
 * @param fd        设备 fd
 * @param feature   ATA SMART feature（如 ATA_SMART_READ_VALUES / ATA_SMART_ENABLE / ATA_SMART_STATUS）
 * @param sectorCount sector count（读 values 时为 1，其他为 0）
 * @param outData   [out] 512 字节数据输出（仅 READ_VALUES 有效），可为 nullptr
 * @return 0 成功，非 0 失败
 */
int AtaSmartCmd(int fd, uint8_t feature, uint8_t sectorCount, uint8_t *outData)
{
    uint8_t cdb[ATA_CDB_TOTAL_SIZE] = {};
    cdb[ATA_CDB_CMD_INDEX] = ATA_SMART_CMD;
    cdb[ATA_CDB_FEATURE_INDEX] = feature;
    cdb[ATA_CDB_NSECT_INDEX] = sectorCount;

    int ret = ioctl(fd, HDIO_DRIVE_CMD, cdb);
    if (ret < 0) {
        return ret;
    }
    if (outData != nullptr) {
        errno_t ret = memcpy_s(outData, ATA_CDB_DATA_SIZE, cdb + ATA_CDB_HEADER_SIZE, ATA_CDB_DATA_SIZE);
        if (ret != EOK) {
            LOGE("AtaSmartCmd: memcpy_s failed, ret=%{public}d", ret);
            return ret;
        }
    }
    return 0;
}

/**
 * @brief 检查 SATA 盘是否支持 SMART（参考 hdd_info.c GetHddSmartSupportedState）。
 */
bool IsSataSmartSupported(int fd)
{
    uint8_t cdb[ATA_CDB_TOTAL_SIZE] = {};
    cdb[ATA_CDB_CMD_INDEX] = ATA_SMART_CMD;
    cdb[ATA_CDB_FEATURE_INDEX] = ATA_SMART_STATUS;
    int ret = ioctl(fd, HDIO_DRIVE_CMD, cdb);
    if (ret < 0) {
        LOGE("IsSataSmartSupported: ioctl failed, errno=%{public}d", errno);
        return false;
    }
    return true;
}

/**
 * @brief Enable SATA SMART（参考 hdd_info.c EnableHddSmart）。
 */
bool EnableSataSmart(int fd)
{
    uint8_t cdb[ATA_CDB_TOTAL_SIZE] = {};
    cdb[ATA_CDB_CMD_INDEX] = ATA_SMART_CMD;
    cdb[ATA_CDB_SECTOR_COUNT_INDEX] = ATA_SMART_ENABLE_SECTOR_COUNT;
    cdb[ATA_CDB_FEATURE_INDEX] = ATA_SMART_ENABLE;
    int ret = ioctl(fd, HDIO_DRIVE_CMD, cdb);
    if (ret < 0) {
        LOGE("EnableSataSmart: ioctl failed, errno=%{public}d", errno);
        return false;
    }
    return true;
}

/**
 * @brief 读取 SATA SMART values 并提取指定属性 ID 的 raw value（参考 hdd_info.c GetHddSmart + GetHddSmartAttribute）。
 */
uint64_t ReadSataSmartAttrById(int fd, uint8_t attrId)
{
    if (!IsSataSmartSupported(fd)) {
        LOGE("ReadSataSmartAttrById: SMART not supported");
        return 0;
    }
    if (!EnableSataSmart(fd)) {
        LOGE("ReadSataSmartAttrById: enable SMART failed");
        return 0;
    }

    uint8_t data[ATA_CDB_DATA_SIZE] = {};
    if (AtaSmartCmd(fd, ATA_SMART_READ_VALUES, ATA_SMART_READ_SECTOR_COUNT, data) != 0) {
        LOGE("ReadSataSmartAttrById: ATA_SMART_READ_VALUES ioctl failed, errno=%{public}d", errno);
        return 0;
    }

    AtaSmartValues *smartValues = reinterpret_cast<AtaSmartValues *>(data);
    for (size_t i = 0; i < ATA_SMART_ATTR_COUNT; ++i) {
        if (smartValues->vendorAttributes[i].id == attrId) {
            return CombineRawValue(smartValues->vendorAttributes[i].rawValue);
        }
    }
    LOGE("ReadSataSmartAttrById: attrId=0x%{public}02x not found", attrId);
    return 0;
}
} // namespace

bool SmartReader::IsNvmeDevice(const std::string &devName)
{
    return devName.find(NVME_PREFIX) == 0;
}

uint64_t SmartReader::GetDiskPowerCycleCount(const std::string &devName)
{
    if (devName.empty()) {
        return 0;
    }
    std::string devicePath = std::string(DEV_BLOCK_PREFIX) + devName;
    int fd = open(devicePath.c_str(), O_RDONLY);
    if (fd < 0) {
        LOGE("GetDiskPowerCycleCount: open %{public}s failed, errno=%{public}d", devicePath.c_str(), errno);
        return 0;
    }
    uint64_t count = IsNvmeDevice(devName) ? ReadNvmePowerCycleCount(fd) : ReadSataPowerCycleCount(fd);
    close(fd);
    LOGI("GetDiskPowerCycleCount: %{public}s = %{public}llu", devName.c_str(), (unsigned long long)count);
    return count;
}

uint64_t SmartReader::GetDiskPowerOnHours(const std::string &devName)
{
    if (devName.empty()) {
        return 0;
    }
    std::string devicePath = std::string(DEV_BLOCK_PREFIX) + devName;
    int fd = open(devicePath.c_str(), O_RDONLY);
    if (fd < 0) {
        LOGE("GetDiskPowerOnHours: open %{public}s failed, errno=%{public}d", devicePath.c_str(), errno);
        return 0;
    }
    uint64_t hours = IsNvmeDevice(devName) ? ReadNvmePowerOnHours(fd) : ReadSataPowerOnHours(fd);
    close(fd);
    LOGI("GetDiskPowerOnHours: %{public}s = %{public}llu", devName.c_str(), (unsigned long long)hours);
    return hours;
}

uint64_t SmartReader::ReadNvmePowerCycleCount(int fd)
{
    NvmeSmartLog log = {};
    if (NvmeGetSmartLogCmd(fd, log) < 0) {
        LOGE("ReadNvmePowerCycleCount: ioctl failed, errno=%{public}d", errno);
        return 0;
    }
    return Le16ToUint64(log.powerCycles);
}

uint64_t SmartReader::ReadNvmePowerOnHours(int fd)
{
    NvmeSmartLog log = {};
    if (NvmeGetSmartLogCmd(fd, log) < 0) {
        LOGE("ReadNvmePowerOnHours: ioctl failed, errno=%{public}d", errno);
        return 0;
    }
    return Le16ToUint64(log.powerOnHours);
}

uint64_t SmartReader::ReadSataPowerCycleCount(int fd)
{
    return ReadSataSmartAttrById(fd, ATA_ATTR_POWER_CYCLE_COUNT);
}

uint64_t SmartReader::ReadSataPowerOnHours(int fd)
{
    return ReadSataSmartAttrById(fd, ATA_ATTR_POWER_ON_HOURS);
}

} // namespace StorageDaemon
} // namespace OHOS
