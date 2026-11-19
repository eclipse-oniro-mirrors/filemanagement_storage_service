/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include "disk_utils_mock.h"

using namespace std;
namespace OHOS {
namespace StorageDaemon {

int GetMaxVolume(dev_t device)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetMaxVolume(device);
}

int IsBlankCD(const std::string &diskPath, bool &isBlankCD)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->IsBlankCD(diskPath, isBlankCD);
}

int IsExistCD(const std::string &diskPath, bool &isExistCD)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->IsExistCD(diskPath, isExistCD);
}

bool IsAcceptableUuid(const std::string &uuid)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return true;
    }
    return IDiskUtilMoc::diskUtilMoc->IsAcceptableUuid(uuid);
}

std::string GetBlkidData(const std::string &devPath, const std::string &type)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return " ";
    }
    return IDiskUtilMoc::diskUtilMoc->GetBlkidData(devPath, type);
}

std::string GetBlkidDataByCmd(std::vector<std::string> &cmd)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return " ";
    }
    return IDiskUtilMoc::diskUtilMoc->GetBlkidDataByCmd(cmd);
}

std::string GetAnonyString(const std::string &value)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return " ";
    }
    return IDiskUtilMoc::diskUtilMoc->GetAnonyString(value);
}

std::string GetCDType(const std::string &diskPath)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetCDType(diskPath);
}

std::string GetUdfRev(const std::string &diskPath)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return "";
    }
    return IDiskUtilMoc::diskUtilMoc->GetUdfRev(diskPath);
}

std::string GetOpticalDriveType(const std::string &diskPath)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetOpticalDriveType(diskPath);
}

int GetCDStatus(const char *device, int &status)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetCDStatus(device, status);
}

int SendScsiCmd(int fd, const ScsiCmdInfo &cmdInfo)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->SendScsiCmd(fd, cmdInfo);
}

int ReadDiscInfo(const std::string &diskPath, int32_t cmdIndex, uint8_t *buf, int len)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->ReadDiscInfo(diskPath, cmdIndex, buf, len);
}

int GetDiscUsedCapacity(int fd, int64_t totalSize, int64_t &discUsedCapacity)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetDiscUsedCapacity(fd, totalSize, discUsedCapacity);
}

std::string GetScsiBusNum(const std::string &sysPath)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetScsiBusNum(sysPath);
}

std::string GetOddDriverType(const std::string &sysPath)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetOddDriverType(sysPath);
}

int GetDvdTotalCapacity(int fd, int64_t &dvdTotalCapacity)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetDvdTotalCapacity(fd, dvdTotalCapacity);
}

int GetCdTotalCapacity(int fd, int64_t &cdTotalCapacity)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetCdTotalCapacity(fd, cdTotalCapacity);
}

int GetDvdPlusRwTotalCapacity(int fd, int64_t &dvdTotalCapacity)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetDvdPlusRwTotalCapacity(fd, dvdTotalCapacity);
}

int GetReadCapacitySize(int fd, int64_t &bdCapacity)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetReadCapacitySize(fd, bdCapacity);
}

int GetBdTrackCapacity(int fd, int64_t &bdTotalCapacity, int64_t &bdUsedCapacity)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetBdTrackCapacity(fd, bdTotalCapacity, bdUsedCapacity);
}

int GetDvdConfiguration(int fd, int &dvdMedia)
{
    if (IDiskUtilMoc::diskUtilMoc == nullptr) {
        return 0;
    }
    return IDiskUtilMoc::diskUtilMoc->GetDvdConfiguration(fd, dvdMedia);
}
}
}
