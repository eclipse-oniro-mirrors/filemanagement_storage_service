/*
 * Copyright (c) 2021-2026 Huawei Device Co., Ltd.
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
#ifndef OHOS_STORAGE_DAEMON_DISK_COMMON_UTILS_H
#define OHOS_STORAGE_DAEMON_DISK_COMMON_UTILS_H

#include <string>
#include <vector>

#include <sys/types.h>

namespace OHOS {
namespace StorageDaemon {
constexpr int DISK_MMC_MAJOR = 179;
constexpr int MAX_SCSI_VOLUMES = 15;
constexpr int DISK_CD_MAJOR = 11;
constexpr int DISC_STATUS_BYTE_INDEX = 2;
constexpr int MAX_BUF = 4096;
constexpr uint8_t DISC_STATUS_MASK = 0x03;

struct ScsiCmdInfo {
    uint8_t *cdb;
    int cdbLen;
    uint8_t *dxferp;
    int dxferLen;
};

bool IsAcceptableUuid(const std::string &uuid);
int GetMaxVolume(dev_t device);
bool HasUtilLinuxKeyword(const std::vector<std::string> &output);
std::vector<std::string> BuildBlkidCmd(bool isUtilLinux, const std::string &type, const std::string &devPath);
std::string GetBlkidData(const std::string &devPath, const std::string &type);
std::string GetBlkidDataByCmd(std::vector<std::string> &cmd);
std::string DiskType2Str(uint8_t diskType);
std::string GetCDType(const std::string &diskPath);
std::string GetOpticalDriveType(const std::string &diskPath);
std::string GetUdfRev(const std::string &diskPath);
int SendScsiCmd(int fd, const ScsiCmdInfo &cmdInfo);
int SendScsiCmd(int fd, const ScsiCmdInfo &cmdInfo, uint8_t *senseBuf, int senseBufLen);
int SendScsiCmdByPath(const std::string &diskPath, uint8_t *cdb, int cdbLen, uint8_t *buf, int len);
int ReadDiscInfo(const std::string &diskPath, int32_t cmdIndex, uint8_t *buf, int len);
int ReadConfiguration(const std::string &diskPath, uint8_t *buf, int len);
int IsBlankCD(const std::string &diskPath, bool &isBlankCD);
int IsExistCD(const std::string &diskPath, bool &isExistCD);
int GetCDStatus(const char *device, int &status);
int GetCdTotalCapacity(int fd, int64_t &cdTotalCapacity);
int GetDiscUsedCapacity(int fd, int64_t totalSize, int64_t &discUsedCapacity);
int GetDvdTotalCapacity(int fd, int64_t &dvdTotalCapacity);
int GetDvdConfiguration(int fd, int &dvdMedia);
int GetReadCapacitySize(int fd, int64_t &bdCapacity);
int GetBdTrackCapacity(int fd, int64_t &bdTotalCapacity, int64_t &bdUsedCapacity);
std::string GetScsiBusNum(const std::string &sysPath);
std::string GetOddDriverType(const std::string &sysPath);
bool CheckUuidInvalid(const std::string &uuid);
} // namespace StorageDaemon
} // namespace OHOS

#endif // OHOS_STORAGE_DAEMON_DISK_COMMON_UTILS_H