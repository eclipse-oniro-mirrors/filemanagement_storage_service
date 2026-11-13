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

#ifndef OHOS_STORAGE_DAEMON_SMART_READER_H
#define OHOS_STORAGE_DAEMON_SMART_READER_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace StorageDaemon {

/**
 * @brief SMART 信息读取工具（storage_daemon root 进程内执行）。
 *        NVMe 走 NVME_IOCTL_ADMIN_CMD 读 SMART log page；
 *        SATA 走 HDIO_DRIVE_CMD 读 ATA SMART attributes。
 */
class SmartReader {
public:
    /**
     * @brief 获取磁盘启停次数（power cycle count）。
     * @param devName 设备名，如 "nvme0n1"、"sda"
     * @return 启停次数；读取失败返回 0
     */
    static uint64_t GetDiskPowerCycleCount(const std::string &devName);

    /**
     * @brief 获取磁盘通电时长（power on hours）。
     * @param devName 设备名，如 "nvme0n1"、"sda"
     * @return 通电小时数；读取失败返回 0
     */
    static uint64_t GetDiskPowerOnHours(const std::string &devName);

private:
    static bool IsNvmeDevice(const std::string &devName);
    static uint64_t ReadNvmePowerCycleCount(int fd);
    static uint64_t ReadNvmePowerOnHours(int fd);
    static uint64_t ReadSataPowerCycleCount(int fd);
    static uint64_t ReadSataPowerOnHours(int fd);
    static uint64_t ReadSataSmartAttr(int fd, uint8_t attrId);
};

} // namespace StorageDaemon
} // namespace OHOS

#endif // OHOS_STORAGE_DAEMON_SMART_READER_H
