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

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <mutex>
#include <linux/cdrom.h>
#include <regex>
#include <sstream>
#include <cinttypes>
#include <scsi/sg.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#include "securec.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "disk_manager/disk/disk_common_utils.h"
#include "utils/file_utils.h"
#include "utils/storage_radar.h"

namespace OHOS {
namespace StorageDaemon {
using namespace std;
using namespace OHOS::StorageService;
constexpr int32_t MIN_UUID_LENGTH = 1;
constexpr int32_t MAX_UUID_LENGTH = 40;
constexpr int32_t DEF_TIMEOUT = 120000;
constexpr int32_t SENSE_BUFF_LEN = 64;
constexpr int32_t READ_DISC_INFO_OPCODE = 0x51;
constexpr int32_t READ_DISC_INFO_SECTOR = 0x46;
constexpr uint8_t DISC_TYPE_OFFSET_HIGH = 6;
constexpr uint8_t DISC_TYPE_OFFSET_LOW = 7;
constexpr uint8_t DISC_TYPE_OFFSET = 8;
constexpr uint8_t BYTE_SHIFT_24 = 24;
constexpr uint8_t BYTE_SHIFT_16 = 16;
constexpr uint8_t BYTE_SHIFT_8 = 8;
constexpr uint8_t BYTE_MASK = 0xff;
constexpr uint8_t LAST_LBA_BYTE_0 = 0;
constexpr uint8_t LAST_LBA_BYTE_1 = 1;
constexpr uint8_t LAST_LBA_BYTE_2 = 2;
constexpr uint8_t LAST_LBA_BYTE_3 = 3;
constexpr uint8_t BLOCK_SIZE_BYTE_0 = 4;
constexpr uint8_t BLOCK_SIZE_BYTE_1 = 5;
constexpr uint8_t BLOCK_SIZE_BYTE_2 = 6;
constexpr uint8_t BLOCK_SIZE_BYTE_3 = 7;
constexpr int32_t CDB_ALLOCATION_LENGTH_HIGH = 7;
constexpr int32_t CDB_ALLOCATION_LENGTH_LOW = 8;
constexpr int32_t MAX_ALLOC_LEN = 0xFFFF;
constexpr int32_t MAX_CDB_LEN = 16;
constexpr int32_t READ_DISC_INFO_CDB_LEN = 10;
constexpr int32_t GET_CONFIG_CDB_LEN = 10;
constexpr int32_t GET_CONFIG_OPCODE = 0x46;
constexpr const char *MMC_MAX_VOLUMES_PATH = "/sys/module/mmcblk/parameters/perdev_minors";
constexpr uint8_t GET_CAPACITY_CMD_BUF_LEN = 16;
constexpr uint8_t GET_CAPACITY_DATA_BUF_LEN = 48;
constexpr uint8_t GET_CD_USED_CAPACITY_CMD_LEN = 10;
constexpr uint8_t GET_CD_USED_CAPACITY_DATA_LEN = 28;
constexpr uint8_t GET_CD_TOTAL_CAPACITY_CMD_LEN = 10;
constexpr uint8_t GET_CD_TOTAL_CAPACITY_DATA_LEN = 4;
constexpr uint8_t GET_DVD_USED_CAPACITY_CMD_LEN = 10;
constexpr uint8_t GET_DVD_USED_CAPACITY_DATA_LEN = 8;
constexpr uint8_t GET_DVD_TOTAL_CAPACITY_CMD_LEN = 12;
constexpr uint8_t GET_DVD_TOTAL_CAPACITY_DATA_LEN = 36;
constexpr uint16_t ODD_LOGICAL_SECTOR_SIZE = 2048;
constexpr uint8_t CD_SECTORS_PER_SECOND = 75;
constexpr uint8_t SECONDS_PER_MINUTES = 60;
constexpr uint8_t SCSI_SENSE_KEY_OFFSET = 2;
constexpr uint8_t SCSI_SENSE_KEY_ILLEGAL_REQUEST = 0x05;
constexpr uint8_t SCSI_ASC_OFFSET = 12;
constexpr uint8_t SCSI_ASCQ_OFFSET = 13;
constexpr uint8_t DISC_STATUS_COMPLETE = 2;
constexpr uint8_t TRACK_INFO_NWA_VALID_OFFSET = 7;
constexpr uint8_t TRACK_INFO_NWA_VALID_MASK = 0x01;
constexpr uint8_t TRACK_INFO_NWA_OFFSET = 12;
constexpr uint8_t TRACK_INFO_FREE_BLOCKS_OFFSET = 16;
constexpr uint8_t TRACK_INFO_TRACK_SIZE_OFFSET = 20;
constexpr uint8_t CDB_TRACK_NUMBER_OFFSET = 5;
constexpr uint8_t TRACK_INFO_INVISIBLE_TRACK = 0xff;
constexpr uint8_t CDB_LBA_BYTE_0 = 2;
constexpr uint8_t CDB_LBA_BYTE_1 = 3;
constexpr uint8_t CDB_LBA_BYTE_2 = 4;
constexpr uint8_t CDB_LBA_BYTE_3 = 5;
constexpr uint8_t DISC_INFO_DATA_LEN = 32;
constexpr uint8_t DISC_INFO_SESSIONS_OFFSET = 4;
constexpr uint8_t DISC_INFO_FIRST_TRACK_IN_LAST_SESSION_OFFSET = 5;
constexpr uint8_t DISC_INFO_LAST_TRACK_IN_LAST_SESSION_OFFSET = 6;
constexpr uint8_t DISC_INFO_FLAGS_OFFSET = 7;
constexpr uint8_t DISC_INFO_LAST_TRACK_ALT_OFFSET = 8;
constexpr size_t DISC_INFO_MAX_NEXT_TRACKS = 8;
constexpr uint8_t DISC_INFO_DISC_STATUS_OFFSET = 2;
constexpr size_t DISC_INFO_RAW_DUMP_LEN = 12;
constexpr uint8_t HEX_BYTE_WIDTH = 2;
constexpr int32_t READ_DISC_INFO_CMD_LEN = 10;

int GetMaxVolume(dev_t device)
{
    LOGD("[L8:DiskUtils] GetMaxVolume: >>> ENTER <<<");
    unsigned int majorId = major(device);
    if (majorId == DISK_MMC_MAJOR) {
        std::string str;
        if (!ReadFile(MMC_MAX_VOLUMES_PATH, &str)) {
            LOGE("[L8:DiskUtils] GetMaxVolume: <<< EXIT FAILED <<< Get MmcMaxVolumes failed");
            return E_ERR;
        }
        errno = 0;
        char *endptr = nullptr;
        long result = strtol(str.c_str(), &endptr, 10);
        if (errno == ERANGE || endptr == str.c_str() || *endptr != '\0' || result <= 0 || result > INT_MAX) {
            LOGE("[L8:DiskUtils] GetMaxVolume: invalid value '%{public}s'", str.c_str());
            return E_ERR;
        }
        return static_cast<int>(result);
    } else {
        LOGD("[L8:DiskUtils] GetMaxVolume: <<< EXIT SUCCESS <<< MAX_SCSI_VOLUMES");
        return MAX_SCSI_VOLUMES;
    }
}

bool IsAcceptableUuid(const std::string &uuid)
{
    if (uuid.empty()) {
        return false;
    }
    if (uuid.length() < MIN_UUID_LENGTH || uuid.length() > MAX_UUID_LENGTH) {
        return false;
    }
    if (CheckUuidInvalid(uuid)) {
        LOGE("[L8:DiskUtils] IsAcceptableUuid: uuid contains invalid characters");
        return false;
    }
    return true;
}

bool CheckUuidInvalid(const std::string &uuid)
{
    static const std::regex validPattern(R"([A-Za-z0-9\-]+)");
    return !std::regex_match(uuid, validPattern);
}

bool HasUtilLinuxKeyword(const std::vector<std::string> &output)
{
    for (const auto &line : output) {
        if (line.find("util-linux") != std::string::npos) {
            return true;
        }
    }
    return false;
}

static bool IsUtilLinuxBlkid()
{
    static bool isUtilLinux = false;
    static std::once_flag probeOnce;
    std::call_once(probeOnce, [&]() {
        std::vector<std::string> probeCmd = { "blkid", "-v" };
        std::vector<std::string> output;
        ForkExec(probeCmd, &output);
        isUtilLinux = HasUtilLinuxKeyword(output);
        LOGI("[L8:DiskUtils] IsUtilLinuxBlkid: %{public}d", static_cast<int>(isUtilLinux));
    });
    return isUtilLinux;
}

std::vector<std::string> BuildBlkidCmd(bool isUtilLinux, const std::string &type, const std::string &devPath)
{
    if (isUtilLinux) {
        return {
            "blkid",
            "-p",
            "-u",
            "noraid",
            "-s",
            type,
            "-o",
            "value",
            devPath
        };
    }
    return {
        "blkid",
        "-n",
        "mdraid",
        "-s",
        type,
        "-o",
        "value",
        devPath
    };
}

std::string GetBlkidData(const std::string &devPath, const std::string &type)
{
    LOGD("[L8:DiskUtils] GetBlkidData: >>> ENTER <<< devPath=%{public}s, type=%{public}s",
        devPath.c_str(), type.c_str());
    if (devPath.empty()) {
        LOGE("[L8:DiskUtils] GetBlkidData: devPath is empty");
        return "";
    }
    if (devPath[0] == '-') {
        LOGE("[L8:DiskUtils] GetBlkidData: devPath starts with dash, possible argument injection");
        return "";
    }
    std::vector<std::string> cmd = BuildBlkidCmd(IsUtilLinuxBlkid(), type, devPath);
    return GetBlkidDataByCmd(cmd);
}

std::string GetBlkidDataByCmd(std::vector<std::string> &cmd)
{
    std::vector<std::string> output;

    int32_t err = ForkExec(cmd, &output);
    for (auto str : output) {
        LOGI("GetBlkidDataByCmd output: %{public}s", str.c_str());
    }
    if (err) {
        LOGE("[L8:DiskUtils] GetBlkidDataByCmd: <<< EXIT FAILED <<< ForkExec failed, err=%{public}d", err);
        return "";
    }

    if (output.size() > 0) {
        size_t sep = output[0].find_first_of("\n");
        if (sep != string::npos)
            output[0].resize(sep);
        LOGD("[L8:DiskUtils] GetBlkidDataByCmd: <<< EXIT SUCCESS <<<");
        return output[0];
    }
    LOGD("[L8:DiskUtils] GetBlkidDataByCmd: <<< EXIT SUCCESS <<< empty result");
    return "";
}

int SendScsiCmd(int fd, const ScsiCmdInfo &cmdInfo)
{
    return SendScsiCmd(fd, cmdInfo, nullptr, 0);
}

int SendScsiCmd(int fd, const ScsiCmdInfo &cmdInfo, uint8_t *senseBuf, int senseBufLen)
{
    LOGD("[L8:DiskUtils] SendScsiCmd: >>> ENTER <<< fd=%{public}d", fd);
    sg_io_hdr_t ioHdr;
    uint8_t sense[SENSE_BUFF_LEN];
    if (memset_s(&ioHdr, sizeof(ioHdr), 0, sizeof(ioHdr)) != 0) {
        LOGE("[L8:DiskUtils] SendScsiCmd: <<< EXIT FAILED <<< ioHdr memset_s failed");
        return E_ERR;
    }
    if (memset_s(sense, sizeof(sense), 0, sizeof(sense)) != 0) {
        LOGE("[L8:DiskUtils] SendScsiCmd: <<< EXIT FAILED <<< sense memset_s failed");
        return E_ERR;
    }
    ioHdr.interface_id = 'S';
    ioHdr.dxfer_direction = SG_DXFER_FROM_DEV;
    ioHdr.cmdp = cmdInfo.cdb;
    ioHdr.cmd_len = cmdInfo.cdbLen;
    ioHdr.dxferp = cmdInfo.dxferp;
    ioHdr.dxfer_len = static_cast<unsigned int>(cmdInfo.dxferLen);
    ioHdr.mx_sb_len = sizeof(sense);
    ioHdr.sbp = sense;
    ioHdr.timeout = DEF_TIMEOUT;
    if (ioctl(fd, SG_IO, &ioHdr) < 0) {
        LOGE("[L8:DiskUtils] SendScsiCmd: <<< EXIT FAILED <<< ioctl SG_IO failed");
        return E_ERR;
    }
    if ((ioHdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        std::string senseStr;
        for (int i = 0; i < ioHdr.mx_sb_len && i < SENSE_BUFF_LEN; i++) {
            senseStr += (i > 0 ? "," : "") + std::to_string(sense[i]);
        }
        LOGE("[L8:DiskUtils] SendScsiCmd: <<< EXIT FAILED <<< SG_INFO not OK, sense=[%{public}s]", senseStr.c_str());
        if (senseBuf != nullptr && senseBufLen > 0) {
            int copyLen = std::min(static_cast<int>(ioHdr.sb_len_wr), senseBufLen);
            if (memcpy_s(senseBuf, senseBufLen, sense, copyLen) != 0) {
                LOGE("[L8:DiskUtils] SendScsiCmd: sense buffer memcpy_s failed");
            }
        }
        return E_ERR;
    }
    LOGD("[L8:DiskUtils] SendScsiCmd: <<< EXIT SUCCESS <<<");
    return E_OK;
}

int GetCDStatus(const char *device, int &status)
{
    char realPath[PATH_MAX] = { 0 };
    if (realpath(device, realPath) == nullptr) {
        LOGE("realpath failed.");
        return E_FILE_PATH_INVALID;
    }
    FILE* file = fopen(realPath, "rb");
    if (file == nullptr) {
        LOGE("fopen failed errStr:%{public}s errno:%{public}d", strerror(errno), errno);
        return E_SYS_KERNEL_ERR;
    }
    int fd = fileno(file);
    if (fd < 0) {
        LOGE("fileno failed, errStr:%{public}s errno:%{public}d", strerror(errno), errno);
        (void)fclose(file);
        return E_SYS_KERNEL_ERR;
    }

    /* Get CD status by ioctl
     * CDS_NO_DISC - 1 - No disk.
     * CDS_TRAY_OPEN - 2 - Tray open.
     * CDS_DRIVE_NOT_READY - 3 - CD-Rom in tray but not ready, wait init.
     * CDS_DISC_OK - 4 - CD-Rom is ready.
     */
    auto startTime = StorageService::StorageRadar::RecordCurrentTime();
    int slot = INT_MAX;
    status = ioctl(fd, CDROM_DRIVE_STATUS, &slot);
    if (status < 0) {
        LOGE("CD status:%{public}d errStr:%{public}s errno:%{public}d", status, strerror(errno), errno);
        (void)fclose(file);
        return E_ERR;
    }
    (void)fclose(file);
    auto delay = StorageService::StorageRadar::ReportDuration("GET CD STATUS: CD ROM",
        startTime, StorageService::DELAY_TIME_THRESH_HIGH, 0);
    LOGW("SD_DURATION: GET CD STATUS: device=%{public}s, delay = %{public}s", device, delay.c_str());
    return E_OK;
}

int SendScsiCmdByPath(const std::string &diskPath, uint8_t *cdb, int cdbLen, uint8_t *buf, int len)
{
    LOGI("[L8:DiskUtils] SendScsiCmdByPath: >>> ENTER <<< diskPath=%{public}s, cdbLen=%{public}d, len=%{public}d",
        diskPath.c_str(), cdbLen, len);
    if (cdb == nullptr || buf == nullptr) {
        LOGE("[L8:DiskUtils] SendScsiCmdByPath: cdb or buf is nullptr");
        return E_ERR;
    }
    if (cdbLen <= 0 || cdbLen > MAX_CDB_LEN || len < 0) {
        LOGE("[L8:DiskUtils] SendScsiCmdByPath: invalid cdbLen=%{public}d or len=%{public}d", cdbLen, len);
        return E_ERR;
    }
    std::string cdbStr;
    for (int i = 0; i < cdbLen; i++) {
        cdbStr += (i > 0 ? "," : "") + std::to_string(cdb[i]);
    }
    LOGI("[L8:DiskUtils] SendScsiCmdByPath: cdb=%{public}s", cdbStr.c_str());

    char realPath[PATH_MAX] = { 0 };
    if (realpath(diskPath.c_str(), realPath) == nullptr) {
        LOGE("[L8:DiskUtils] SendScsiCmdByPath: <<< EXIT FAILED <<< realpath failed");
        return E_ERR;
    }
    FILE* file = fopen(realPath, "rb");
    if (file == nullptr) {
        return E_ERR;
    }
    int fd = fileno(file);
    if (fd < 0) {
        (void)fclose(file);
        return E_ERR;
    }

    ScsiCmdInfo cmdInfo = { cdb, cdbLen, buf, len };
    int ret = SendScsiCmd(fd, cmdInfo);
    (void)fclose(file);
    LOGI("[L8:DiskUtils] SendScsiCmdByPath: <<< EXIT SUCCESS <<< ret=%{public}d", ret);
    return ret;
}

int ReadDiscInfo(const std::string &diskPath, int32_t cmdIndex, uint8_t *buf, int len)
{
    LOGD("[L8:DiskUtils] ReadDiscInfo: >>> ENTER <<< diskPath=%{public}s, len=%{public}d", diskPath.c_str(), len);
    uint8_t cdb[READ_DISC_INFO_CDB_LEN] = { static_cast<uint8_t>(cmdIndex) };
    cdb[CDB_ALLOCATION_LENGTH_HIGH] = static_cast<uint8_t>(static_cast<uint32_t>(len) >> CDB_ALLOCATION_LENGTH_LOW);
    cdb[CDB_ALLOCATION_LENGTH_LOW] = static_cast<uint8_t>(static_cast<uint32_t>(len) & MAX_ALLOC_LEN);

    int ret = SendScsiCmdByPath(diskPath, cdb, sizeof(cdb), buf, len);
    if (ret != 0) {
        LOGE("[L8:DiskUtils] ReadDiscInfo: <<< EXIT FAILED <<< SendScsiCmdByPath failed, err=%{public}d", ret);
        return ret;
    }
    LOGD("[L8:DiskUtils] ReadDiscInfo: <<< EXIT SUCCESS <<<");
    return E_OK;
}

int ReadConfiguration(const std::string &diskPath, uint8_t *buf, int len)
{
    LOGI("[L8:DiskUtils] ReadConfiguration: >>> ENTER <<< diskPath=%{public}s, len=%{public}d", diskPath.c_str(), len);
    if (len <= 0 || buf == nullptr) {
        LOGE("[L8:DiskUtils] ReadConfiguration: invalid params, len=%{public}d", len);
        return E_PARAMS_INVALID;
    }
    uint32_t allocLen = static_cast<uint32_t>(len);
    uint8_t cdb[GET_CONFIG_CDB_LEN] = { GET_CONFIG_OPCODE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        static_cast<uint8_t>((allocLen >> 8) & 0xFF), static_cast<uint8_t>(allocLen & 0xFF) };

    int ret = SendScsiCmdByPath(diskPath, cdb, sizeof(cdb), buf, len);
    if (ret == 0) {
        std::string respStr;
        for (int i = 0; i < std::min(len, SENSE_BUFF_LEN); i++) {
            respStr += (i > 0 ? "," : "") + std::to_string(buf[i]);
        }
        LOGI("[L8:DiskUtils] ReadConfiguration: RESP(first 64 bytes)=[%{public}s]", respStr.c_str());
    }
    LOGI("[L8:DiskUtils] ReadConfiguration: <<< EXIT SUCCESS <<< ret=%{public}d", ret);
    return ret;
}

int IsExistCD(const std::string &diskPath, bool &isExistCD)
{
    LOGD("[L8:DiskUtils] IsExistCD: >>> ENTER <<< diskPath=%{public}s", diskPath.c_str());
    int status = 0;
    isExistCD = false;
    int ret = GetCDStatus(diskPath.c_str(), status);
    if (ret != E_OK) {
        LOGE("Unknown cd status!");
        return E_ERR;
    }
    isExistCD = (status == CDS_DRIVE_NOT_READY || status == CDS_DISC_OK);
    LOGI("[L8:DiskUtils] IsExistCD:Current CD statue, isExistCD: %{public}d", isExistCD);
    return E_OK;
}

int IsBlankCD(const std::string &diskPath, bool &isBlankCD)
{
    isBlankCD = false;
    bool isExistCD = false;
    int existRet = IsExistCD(diskPath, isExistCD);
    if (existRet != E_OK || !isExistCD) {
        LOGE("CD is not exist, please check ! existRet:%{public}d, isExistCD:%{public}d", existRet, isExistCD);
        return E_ERR;
    }

    uint8_t buf[MAX_BUF];
    if (ReadDiscInfo(diskPath, READ_DISC_INFO_OPCODE, buf, sizeof(buf)) == E_OK) {
        uint8_t discStatus = buf[DISC_STATUS_BYTE_INDEX] & DISC_STATUS_MASK;
        isBlankCD = (discStatus == 0);
        LOGI("[L8:DiskUtils] IsBlankCD: <<< EXIT SUCCESS <<< isBlank=%{public}d", isBlankCD);
        return E_OK;
    } else {
        LOGE("[L8:DiskUtils] IsBlankCD: <<< EXIT FAILED <<< Unable to read disc information");
    }
    LOGE("Unable to read disc information.");
    return E_ERR;
}

std::string DiskType2Str(uint8_t diskType)
{
    LOGD("[L8:DiskUtils] DiskType2Str: >>> ENTER <<< diskType=0x%{public}x", diskType);
    switch (diskType) {
        case 0x08: // CD-ROM (read only)
            return "CD-ROM";
        case 0x09:
            return "CD-R";
        case 0x0A:
            return "CD-RW";
        case 0x10:
            return "DVD-ROM";
        case 0x11: // DVD-R sequential
            return "DVD-R";
        case 0x12:
            return "DVD-RAM";
        case 0x13: // DVD-RW restricted overwrite
        case 0x14: // DVD-RW sequential
            return "DVD-RW";
        case 0x15: // DVD-R/DL sequential
        case 0x16: // DVD-R/DL layer jump
            return "DVD-R/DL";
        case 0x1A:
            return "DVD+RW";
        case 0x1B: // DVD+R
        case 0x1C: // DVD+R dual layer
            return "DVD+R";
        case 0x1D: // DVD+RW dual layer
            return "DVD+RW";
        case 0x2A: // DVD+RW/DL
              return "DVD+RW/DL";
        case 0x2B: // DVD+R/DL
              return "DVD+R/DL";
        case 0x40:
            return "BD-ROM";
        case 0x41:
            return "BD-R";
        case 0x42:
        case 0x43:
            return "BD-RE";
        default:
            LOGW("[L8:DiskUtils] DiskType2Str: unknown diskType=0x%{public}x", diskType);
            return "";
    }
}

std::string GetCDType(const std::string &diskPath)
{
    LOGD("[L8:DiskUtils] GetCDType: >>> ENTER <<< diskPath=%{public}s", diskPath.c_str());
    uint8_t buf[MAX_BUF];
    if (ReadDiscInfo(diskPath, READ_DISC_INFO_SECTOR, buf, sizeof(buf)) == 0) {
        return DiskType2Str((buf[DISC_TYPE_OFFSET_HIGH] << DISC_TYPE_OFFSET) | buf[DISC_TYPE_OFFSET_LOW]);
    } else {
        LOGE("[L8:DiskUtils] GetCDType: <<< EXIT FAILED <<< Unable to read disc information");
    }
    return "";
}

std::string GetUdfRev(const std::string &diskPath)
{
    LOGI("[L8:DiskUtils] GetUdfRev: >>> ENTER <<< diskPath=%{public}s", diskPath.c_str());
    if (diskPath.empty() || diskPath[0] == '-') {
        LOGE("[L8:DiskUtils] GetUdfRev: invalid diskPath");
        return "";
    }
    std::vector<std::string> cmd = { "udfinfo", diskPath };
    std::vector<std::string> output;
    int32_t err = ForkExec(cmd, &output);
    if (err != E_OK) {
        LOGE("[L8:DiskUtils] GetUdfRev: ForkExec failed err=%{public}d", err);
        return "";
    }
    constexpr const char *udfrevPrefix = "udfrev=";
    const size_t prefixLen = std::string(udfrevPrefix).size();
    for (const auto &line : output) {
        auto pos = line.find(udfrevPrefix);
        if (pos == std::string::npos) {
            continue;
        }
        std::string rev = line.substr(pos + prefixLen);
        auto nl = rev.find_first_of("\r\n");
        if (nl != std::string::npos) {
            rev.erase(nl);
        }
        std::string result = "udf" + rev;
        LOGI("[L8:DiskUtils] GetUdfRev: <<< EXIT SUCCESS <<< udfrev=%{public}s", result.c_str());
        return result;
    }
    LOGW("[L8:DiskUtils] GetUdfRev: udfrev not found in udfinfo output");
    return "";
}

std::string GetOpticalDriveType(const std::string &diskPath)
{
    LOGI("[L8:DiskUtils] GetOpticalDriveType: >>> ENTER <<< diskPath=%{public}s", diskPath.c_str());
    uint8_t buf[MAX_BUF];
    const size_t FEATURE_START = 8;
    const size_t LOW_BYTE_OFFSET = 1;
    const size_t ADDITIONAL_LEN = 3;
    const int FEATURE_CODE_LENGTH = 4;
    if (ReadConfiguration(diskPath, buf, sizeof(buf)) != E_OK) {
        LOGE("[L8:DiskUtils] GetOpticalDriveType: <<< EXIT FAILED <<< GetConfiguration not supported");
        return "";
    }
    uint32_t dataLen = (static_cast<uint32_t>(buf[0]) << 24) | (static_cast<uint32_t>(buf[1]) << 16) |
                       (static_cast<uint32_t>(buf[2]) << 8) | static_cast<uint32_t>(buf[3]);
    if (dataLen < FEATURE_CODE_LENGTH || dataLen > MAX_BUF - FEATURE_CODE_LENGTH) {
        LOGE("[L8:DiskUtils] GetOpticalDriveType: <<< EXIT FAILED <<< Invalid data length=%{public}d",
             dataLen);
        return "";
    }
    uint16_t currentProfile = (buf[6] << 8) | buf[7];
    LOGI("[L8:DiskUtils] GetOpticalDriveType: currentProfile=0x%{public}x", currentProfile);
    bool hasDvdRw = false;
    bool hasDvdR = false;
    bool hasBdRe = false;
    bool hasBdR = false;
    bool hasBdRom = false;
    for (size_t i = FEATURE_START;
         i + ADDITIONAL_LEN < std::min(static_cast<size_t>(dataLen + FEATURE_CODE_LENGTH), sizeof(buf));) {
        uint16_t feature = (buf[i] << FEATURE_START) | buf[i + LOW_BYTE_OFFSET];
        uint8_t additionalLen = buf[i + ADDITIONAL_LEN];
        if (feature == 0x002D) {
            hasDvdRw = true;
        } else if (feature == 0x002E) {
            hasDvdR = true;
        } else if (feature == 0x0042) {
            hasBdRe = true;
        } else if (feature == 0x0041) {
            hasBdR = true;
        } else if (feature == 0x0040) {
            hasBdRom = true;
        }
        i += FEATURE_CODE_LENGTH + additionalLen;
    }
    if (hasBdRe || hasBdR || hasBdRom) {return "BD-RE";}
    if (hasDvdRw || hasDvdR) {return "DVD RW";}
    if (currentProfile != 0) {
        return DiskType2Str(static_cast<uint8_t>(currentProfile & 0xFF));
    }
    LOGW("[L8:DiskUtils] GetOpticalDriveType: <<< EXIT NO MATCH <<< No matching profile found");
    return "";
}

int GetCdTotalCapacity(int fd, int64_t &cdTotalCapacity)
{
    unsigned char cmd_buf[GET_CAPACITY_CMD_BUF_LEN] = {0};
    int cmd_len = GET_CD_TOTAL_CAPACITY_CMD_LEN;
    unsigned char data_buf[GET_CAPACITY_DATA_BUF_LEN] = {0};
    unsigned int data_len = GET_CD_TOTAL_CAPACITY_DATA_LEN;
    unsigned int actual_len = 0;
    int ret = 0;
    double total_seconds = 0;
    /**
      * 功能：通过 SCSI READ TOC 指令获取cd光盘的 ATIP 信息。
      * ATIP 包含光盘介质类型、制造商及容量等底层参数。
      * 参数说明：
      * cmd_buf[2] = 0x04: 指定读取格式为 ATIP。
      * cmd_buf[7-8]: 定义设备返回数据的最大长度。
    */
    cmd_buf[0] = GPCMD_READ_TOC_PMA_ATIP;
    cmd_buf[2] = 0x04;
    cmd_buf[7] = (data_len >> 8) & 0xff;
    cmd_buf[8] = data_len & 0xff;
    ScsiCmdInfo cmdInfo = { cmd_buf, cmd_len, data_buf, data_len };
    ret = SendScsiCmd(fd, cmdInfo);
    if (ret != 0) {
        LOGE("get atip data len failed, ret val is %{public}d", ret);
        return E_ERR;
    }
    actual_len = (int)(data_buf[0] << 8) | data_buf[1];
    actual_len += 2;
    LOGI("actual_len: %{public}d", actual_len);
    if (actual_len > GET_CAPACITY_DATA_BUF_LEN) {
        LOGE("actual_len=%{public}d exceeds buffer size", actual_len);
        return E_ERR;
    }
    cmd_buf[7] = (actual_len >> 8) & 0xff;
    cmd_buf[8] = actual_len & 0xff;
    cmdInfo = { cmd_buf, cmd_len, data_buf, actual_len };
    ret = SendScsiCmd(fd, cmdInfo);
    if (ret != 0) {
        LOGE("get atip data failed, ret is %{public}d", ret);
        return E_ERR;
    }

    total_seconds = (double)data_buf[12] * SECONDS_PER_MINUTES + data_buf[13] +
                    (double)data_buf[14] / CD_SECTORS_PER_SECOND;
    cdTotalCapacity = static_cast<int64_t>(total_seconds * CD_SECTORS_PER_SECOND * ODD_LOGICAL_SECTOR_SIZE);
    LOGI("cd total_seconds: %{public}f, total_capacity: %{public}" PRIu64, total_seconds, cdTotalCapacity);
    return E_OK;
}

/*
 * 已封盘(finalize)的光盘没有 Invisible Track，READ TRACK INFORMATION 会返回
 * ILLEGAL_REQUEST 错误。此时通过 READ DISC INFORMATION (0x51) 检查盘片状态，
 * 若 disc_status == DS_COMPLETE(2) 则确认为封盘，已用容量等于总容量。
 */
static int HandleFinalizedDisc(int fd, int64_t totalSize, int64_t &discUsedCapacity)
{
    unsigned char discInfoBuf[MAX_BUF] = {0};
    unsigned char discCmd[READ_DISC_INFO_CMD_LEN] = {0};
    discCmd[0] = static_cast<unsigned char>(READ_DISC_INFO_OPCODE);
    discCmd[CDB_ALLOCATION_LENGTH_HIGH] = (sizeof(discInfoBuf) >> BYTE_SHIFT_8) & 0xff;
    discCmd[CDB_ALLOCATION_LENGTH_LOW] = sizeof(discInfoBuf) & 0xff;
    ScsiCmdInfo discCmdInfo = { discCmd, static_cast<int>(sizeof(discCmd)), discInfoBuf,
                                static_cast<int>(sizeof(discInfoBuf)) };
    int discRet = SendScsiCmd(fd, discCmdInfo);
    if (discRet != 0) {
        LOGE("read disc info also failed, cannot determine finalize status");
        return E_ERR;
    }

    uint8_t discStatus = discInfoBuf[DISC_STATUS_BYTE_INDEX] & DISC_STATUS_MASK;
    if (discStatus != DISC_STATUS_COMPLETE) {
        LOGE("disc is not finalized but read track info failed, discStatus=%{public}d", discStatus);
        return E_ERR;
    }

    if (totalSize > 0) {
        discUsedCapacity = totalSize;
        LOGI("disc is finalized, used_capacity equals total_capacity: %{public}" PRId64, discUsedCapacity);
        return E_OK;
    }
    int64_t totalCapacity = 0;
    if (GetCdTotalCapacity(fd, totalCapacity) == E_OK && totalCapacity > 0) {
        discUsedCapacity = totalCapacity;
        LOGI("cd is finalized, used_capacity equals total_capacity: %{public}" PRId64, discUsedCapacity);
        return E_OK;
    }
    if (GetDvdTotalCapacity(fd, totalCapacity) == E_OK && totalCapacity > 0) {
        discUsedCapacity = totalCapacity;
        LOGI("dvd is finalized, used_capacity equals total_capacity: %{public}" PRId64, discUsedCapacity);
        return E_OK;
    }
    LOGE("disc is finalized but failed to get total capacity");
    return E_ERR;
}

/*
 * 从 READ TRACK INFORMATION 返回数据中解析已用容量
 * 优先使用 NWA (Next Writable Address)，NWA 之前的区域即为已写入区域
 * 当 NWA 无效时（如 Windows 擦除后部分格式化的盘片），fallback 到
 * Track Size - Free Blocks 计算已用容量
 */
static int ParseNextWritableAddr(const unsigned char *dataBuf, int64_t &discUsedCapacity)
{
    if (dataBuf[TRACK_INFO_NWA_VALID_OFFSET] & TRACK_INFO_NWA_VALID_MASK) {
        int32_t nextAddr = static_cast<int32_t>(
            (static_cast<uint32_t>(dataBuf[TRACK_INFO_NWA_OFFSET]) << 24) |
            (static_cast<uint32_t>(dataBuf[TRACK_INFO_NWA_OFFSET + 1]) << 16) |
            (static_cast<uint32_t>(dataBuf[TRACK_INFO_NWA_OFFSET + 2]) << 8) |
            (static_cast<uint32_t>(dataBuf[TRACK_INFO_NWA_OFFSET + 3])));
        if (nextAddr < 0) {
            LOGW("next writable address is negative (%{public}d), clamping to 0", nextAddr);
            nextAddr = 0;
        }
        discUsedCapacity = static_cast<int64_t>(nextAddr) * ODD_LOGICAL_SECTOR_SIZE;
        LOGI("disc used_capacity (NWA): %{public}" PRId64, discUsedCapacity);
        return E_OK;
    }

    LOGW("NWA is not valid, fallback to Track Size - Free Blocks");
    uint32_t freeBlocks = (static_cast<uint32_t>(dataBuf[TRACK_INFO_FREE_BLOCKS_OFFSET]) << 24) |
                          (static_cast<uint32_t>(dataBuf[TRACK_INFO_FREE_BLOCKS_OFFSET + 1]) << 16) |
                          (static_cast<uint32_t>(dataBuf[TRACK_INFO_FREE_BLOCKS_OFFSET + 2]) << 8) |
                          static_cast<uint32_t>(dataBuf[TRACK_INFO_FREE_BLOCKS_OFFSET + 3]);
    uint32_t trackSize = (static_cast<uint32_t>(dataBuf[TRACK_INFO_TRACK_SIZE_OFFSET]) << 24) |
                         (static_cast<uint32_t>(dataBuf[TRACK_INFO_TRACK_SIZE_OFFSET + 1]) << 16) |
                         (static_cast<uint32_t>(dataBuf[TRACK_INFO_TRACK_SIZE_OFFSET + 2]) << 8) |
                         static_cast<uint32_t>(dataBuf[TRACK_INFO_TRACK_SIZE_OFFSET + 3]);
    if (trackSize <= freeBlocks) {
        LOGE("track_size=%{public}u <= free_blocks=%{public}u, invalid data", trackSize, freeBlocks);
        return E_ERR;
    }
    discUsedCapacity = static_cast<int64_t>(trackSize - freeBlocks) * ODD_LOGICAL_SECTOR_SIZE;
    LOGI("disc used_capacity (TrackSize-FreeBlocks): trackSize=%{public}u, freeBlocks=%{public}u, "
         "used=%{public}" PRId64, trackSize, freeBlocks, discUsedCapacity);
    return E_OK;
}

int GetDiscUsedCapacity(int fd, int64_t totalSize, int64_t &discUsedCapacity)
{
    unsigned char cmdBuf[GET_CAPACITY_CMD_BUF_LEN] = {0};
    unsigned char dataBuf[GET_CAPACITY_DATA_BUF_LEN] = {0};
    uint8_t senseBuf[SENSE_BUFF_LEN] = {0};
    unsigned int dataLen = GET_CD_USED_CAPACITY_DATA_LEN;
    /*
    * 使用 SCSI READ TRACK INFORMATION 指令 (0x52) 获取光盘轨道/逻辑分区信息
    * 适用于 CD 和 DVD 光盘，通过 NWA 或 Track Size - Free Blocks 计算已用容量
    * cmdBuf[0]: 指令操作码 0x52 (READ TRACK/RZONE INFORMATION)
    * cmdBuf[1]: 设置为 1，表示通过轨道编号 (Track Number) 寻址
    * cmdBuf[5]: 设置为 0xff，通常用于请求最后一条轨道 (Invisible Track) 或特定状态信息
    * cmdBuf[7-8]: 分别填充 dataLen 的高8位和低8位，告知驱动器返回数据的长度限制
    */
    cmdBuf[0] = GPCMD_READ_TRACK_RZONE_INFO;
    cmdBuf[1] = 1;
    cmdBuf[BLOCK_SIZE_BYTE_1] = 0xff;
    cmdBuf[CDB_ALLOCATION_LENGTH_HIGH] = (dataLen >> BYTE_SHIFT_8) & 0xff;
    cmdBuf[CDB_ALLOCATION_LENGTH_LOW] = dataLen & 0xff;
    ScsiCmdInfo cmdInfo = { cmdBuf, GET_CD_USED_CAPACITY_CMD_LEN, dataBuf,
                            GET_CD_USED_CAPACITY_DATA_LEN };
    int ret = SendScsiCmd(fd, cmdInfo, senseBuf, sizeof(senseBuf));
    if (ret != 0) {
        uint8_t senseKey = senseBuf[SCSI_SENSE_KEY_OFFSET] & 0x0F;
        uint8_t asc = senseBuf[SCSI_ASC_OFFSET];
        uint8_t ascq = senseBuf[SCSI_ASCQ_OFFSET];
        LOGE("get disc used capacity failed, ret=%{public}d, senseKey=0x%{public}02X, "
             "asc=0x%{public}02X, ascq=0x%{public}02X", ret, senseKey, asc, ascq);
        if (senseKey == SCSI_SENSE_KEY_ILLEGAL_REQUEST) {
            return HandleFinalizedDisc(fd, totalSize, discUsedCapacity);
        }
        return E_ERR;
    }

    return ParseNextWritableAddr(dataBuf, discUsedCapacity);
}

int GetDvdTotalCapacity(int fd, int64_t &dvdTotalCapacity)
{
    unsigned char cmd_buf[GET_CAPACITY_CMD_BUF_LEN] = {0};
    int cmd_len = GET_DVD_TOTAL_CAPACITY_CMD_LEN;
    unsigned char data_buf[GET_CAPACITY_DATA_BUF_LEN] = {0};
    int data_len = GET_DVD_TOTAL_CAPACITY_DATA_LEN;
    int ret = 0;
    unsigned long long phys_start = 0;
    unsigned long long phys_end = 0;
    int dvd_media = 0;
    ret = GetDvdConfiguration(fd, dvd_media);
    if (ret != 0) {
        LOGE("get dvd configuration failed,ret val is %{public}d", ret);
        return E_ERR;
    }
    /*
    * 使用 SCSI READ DVD STRUCTURE 指令 (0xAD) 获取光盘物理结构或容量信息
    * cmd_buf[0]: 操作码 0xAD (GPCMD_READ_DVD_STRUCTURE)。
    * cmd_buf[7]: 格式码 (Format Code)。
    *             如果 dvd_media <= 0x18 (通常指 DVD-ROM/R/RW)，设置为 16 (0x10)，
    *             这对应 "DVD Structure List"，用于列出支持的结构。
    *             否则设置为 0，通常对应 "Physical Format Information"，读取最基础的物理层信息。
    * cmd_buf[9]: 分配长度 (Allocation Length) 的低位。
    *             这里通常设置为数据结构的总长度 (GET_DVD_TOTAL_CAPACITY_DATA_LEN)。
    * cmd_buf[11]:保留位/控制字节，通常设为 0。
    */
    cmd_buf[0] = GPCMD_READ_DVD_STRUCTURE;
    cmd_buf[7] = dvd_media <= 0x18 ? 16 : 0;
    cmd_buf[9] = GET_DVD_TOTAL_CAPACITY_DATA_LEN;
    cmd_buf[11] = 0;
    ScsiCmdInfo cmdInfo = { cmd_buf, cmd_len, data_buf, data_len };
    ret = SendScsiCmd(fd, cmdInfo);
    if (ret != 0) {
        LOGE("get dvd total capacity failed, ret val is %{public}d", ret);
        return E_ERR;
    }

    phys_start = ((unsigned long long)(data_buf[9]) << 16) |
                 ((unsigned long long)(data_buf[10]) << 8) |
                 data_buf[11];
    if ((data_buf[6] & 0x60) == 0) {
        phys_end = ((unsigned long long)(data_buf[13]) << 16) |
                   ((unsigned long long)(data_buf[14]) << 8) |
                   data_buf[15];
    } else {
        phys_end = ((unsigned long long)(data_buf[17]) << 16) |
                    ((unsigned long long)(data_buf[18]) << 8) |
                    data_buf[19];
    }
    if (phys_end > phys_start) {
        dvdTotalCapacity = static_cast<int64_t>(phys_end - phys_start + 1) * ODD_LOGICAL_SECTOR_SIZE;
    } else {
        dvdTotalCapacity = 0;
        LOGI("phys_end is less than phys_start, error data");
    }

    LOGI("dvd dvdTotalCapacity: %{public}" PRIu64, dvdTotalCapacity);
    return E_OK;
}

int GetDvdConfiguration(int fd, int &dvdMedia)
{
    unsigned char cmd_buf[GET_CAPACITY_CMD_BUF_LEN] = {0};
    int cmd_len = GET_DVD_USED_CAPACITY_CMD_LEN;
    unsigned char data_buf[GET_CAPACITY_DATA_BUF_LEN] = {0};
    unsigned int data_len = GET_DVD_USED_CAPACITY_DATA_LEN;
    int ret = 0;
    /*
     * 使用 SCSI GET CONFIGURATION 指令 (0x46) 获取光驱设备的功能列表。
     * 字段详解:
     * cmd_buf[0]: 操作码 0x46 (GPCMD_GET_CONFIGURATION)。
     * cmd_buf[1]: RT (Request Type) 字段。
     *             设置为 1 (01b) 表示请求“当前活动的功能”(Current Features Only)，
     *             即仅列出当前插入介质所支持的功能（如刻录、读取能力）。
     * cmd_buf[7-8]: 分配长度 (Allocation Length)。
     *               指定接收配置数据（Feature Descriptors）的最大缓冲区长度。
     *               高 8 位存入 cmd_buf[7]，低 8 位存入 cmd_buf[8]。
     * 用途:
     * 常用于探测光驱是否支持特定的 Profile（如 DVD-RW, BD-RE）以及当前光盘的写入模式。
     */
    cmd_buf[0] = GPCMD_GET_CONFIGURATION;
    cmd_buf[1] = 1;
    cmd_buf[7] = (data_len >> 8) & 0xff;
    cmd_buf[8] = data_len & 0xff;
    std::string cdbStr;
    for (int i = 0; i < cmd_len; i++) {
        cdbStr += (i > 0 ? "," : "") + std::to_string(cmd_buf[i]);
    }
    LOGI("[L8:DiskUtils] GetDvdConfiguration: cdb=[%{public}s]", cdbStr.c_str());
    ScsiCmdInfo cmdInfo = { cmd_buf, cmd_len, data_buf, data_len };
    ret = SendScsiCmd(fd, cmdInfo);
    if (ret != 0) {
        LOGE("get atip data len failed, ret val is %{public}d", ret);
        return E_ERR;
    }

    dvdMedia = static_cast<int>((unsigned int)data_buf[DISC_TYPE_OFFSET_HIGH] << DISC_TYPE_OFFSET) |
        data_buf[DISC_TYPE_OFFSET_LOW];
    LOGI("dvd_media: %{public}#x", dvdMedia);
    return E_OK;
}

/*
 * 使用 SCSI READ CAPACITY 指令 (0x25) 获取蓝光光盘逻辑地址空间容量。
 * 注意：对未封盘且未预留整盘数据区的 BD-R（如第三方刻录工具顺序记录写出的盘），
 * 返回的 Last LBA 只是已记录区末尾，不是介质总容量。
 * 返回值同时用于定位 invisible track：未封盘 BD-R 的 invisible track 起始地址
 * 恰为已记录区末尾（经 dvd+rw-mediainfo 对实际盘片验证）。
 */
int GetReadCapacitySize(int fd, int64_t &bdCapacity)
{
    unsigned char cmd_buf[GET_CAPACITY_CMD_BUF_LEN] = {0};
    int cmd_len = GET_DVD_USED_CAPACITY_CMD_LEN;
    unsigned char data_buf[GET_CAPACITY_DATA_BUF_LEN] = {0};
    unsigned int data_len = GET_DVD_USED_CAPACITY_DATA_LEN;
    cmd_buf[0] = GPCMD_READ_CDVD_CAPACITY;
    cmd_buf[CDB_ALLOCATION_LENGTH_HIGH] = (data_len >> BYTE_SHIFT_8) & BYTE_MASK;
    cmd_buf[CDB_ALLOCATION_LENGTH_LOW] = data_len & BYTE_MASK;
    ScsiCmdInfo cmdInfo = { cmd_buf, cmd_len, data_buf, data_len };
    int ret = SendScsiCmd(fd, cmdInfo);
    if (ret != 0) {
        LOGE("get bd read capacity failed, ret val is %{public}d", ret);
        return E_ERR;
    }

    unsigned int blkCnt = (static_cast<unsigned int>(data_buf[LAST_LBA_BYTE_0]) << BYTE_SHIFT_24) |
            (static_cast<unsigned int>(data_buf[LAST_LBA_BYTE_1]) << BYTE_SHIFT_16) |
            (static_cast<unsigned int>(data_buf[LAST_LBA_BYTE_2]) << BYTE_SHIFT_8) |
            data_buf[LAST_LBA_BYTE_3];
    unsigned int blkSize = (static_cast<unsigned int>(data_buf[BLOCK_SIZE_BYTE_0]) << BYTE_SHIFT_24) |
            (static_cast<unsigned int>(data_buf[BLOCK_SIZE_BYTE_1]) << BYTE_SHIFT_16) |
            (static_cast<unsigned int>(data_buf[BLOCK_SIZE_BYTE_2]) << BYTE_SHIFT_8) |
            data_buf[BLOCK_SIZE_BYTE_3];
    int64_t blkCnt64 = static_cast<int64_t>(blkCnt);
    int64_t blkSize64 = static_cast<int64_t>(blkSize);
    if (blkSize64 != 0 && blkCnt64 + 1 > INT64_MAX / blkSize64) {
        LOGE("GetReadCapacitySize: multiplication overflow");
        return E_ERR;
    }
    bdCapacity = (blkCnt64 + 1) * blkSize64;
    LOGI("bd read_capacity: %{public}u * %{public}u = %{public}" PRId64 " bytes",
        blkCnt + 1, blkSize, bdCapacity);
    return E_OK;
}

/*
 * 使用 SCSI READ DISC INFORMATION (0x51) 获取候选的"当前开放轨道号"（next track）。
 * 未封盘 BD-R 的开放轨道为 invisible track，其 NWA/Free Blocks 反映真实剩余可写空间。
 *
 * 盘片信息块字段布局在不同固件/规范版本间不一致：实测 Verbatim 66768 对 BD-R 返回
 * 单字节布局——byte 4 = session 数，byte 5 = 最后 session 首轨道，byte 6 = 最后 session
 * 末轨道（即开放轨道），byte 7 = DID_V/DBC_V 等标志位（实测 0x20）。若按 MMC-5 的
 * 双字节（LSB/MSB）解析 byte 6-7，会得到 0x2004 这类垃圾值（仅低字节恰好可用）。
 * 因此这里给出多种候选布局，由调用方用 READ TRACK INFORMATION 逐个验证
 * （只有开放轨道才满足 nwaValid && freeBlocks > 0），不依赖单一布局假设。
 */
static int GetDiscInfoNextTracks(int fd, uint8_t *nextTracks, size_t maxCount, size_t &count)
{
    count = 0;
    unsigned char cmd_buf[GET_CAPACITY_CMD_BUF_LEN] = {0};
    unsigned char data_buf[GET_CAPACITY_DATA_BUF_LEN] = {0};
    uint8_t senseBuf[SENSE_BUFF_LEN] = {0};
    cmd_buf[0] = GPCMD_READ_DISC_INFO;
    cmd_buf[CDB_ALLOCATION_LENGTH_HIGH] = (DISC_INFO_DATA_LEN >> BYTE_SHIFT_8) & BYTE_MASK;
    cmd_buf[CDB_ALLOCATION_LENGTH_LOW] = DISC_INFO_DATA_LEN & BYTE_MASK;
    ScsiCmdInfo cmdInfo = { cmd_buf, GET_CD_USED_CAPACITY_CMD_LEN, data_buf, DISC_INFO_DATA_LEN };
    int ret = SendScsiCmd(fd, cmdInfo, senseBuf, sizeof(senseBuf));
    if (ret != 0) {
        LOGI("GetDiscInfoNextTracks: read disc info failed, ret=%{public}d senseKey=0x%{public}02X asc=0x%{public}02X",
             ret, senseBuf[SCSI_SENSE_KEY_OFFSET] & 0x0F, senseBuf[SCSI_ASC_OFFSET]);
        return E_ERR;
    }
    // 只打印决策相关字段：盘状态、session 数、末轨道号、标志位
    LOGI("GetDiscInfoNextTracks: discStatus=0x%{public}02X, sessions=%{public}u, "
         "lastTrackInLastSession=%{public}u, flags=0x%{public}02X",
         data_buf[DISC_INFO_DISC_STATUS_OFFSET], data_buf[DISC_INFO_SESSIONS_OFFSET],
         data_buf[DISC_INFO_LAST_TRACK_IN_LAST_SESSION_OFFSET], data_buf[DISC_INFO_FLAGS_OFFSET]);

    uint8_t lastTrack = data_buf[DISC_INFO_LAST_TRACK_IN_LAST_SESSION_OFFSET];
    uint8_t altLastTrack = data_buf[DISC_INFO_LAST_TRACK_ALT_OFFSET];
    uint8_t candidates[] = {
        // 单字节布局：最后 session 末轨道（实测 Verbatim 66768）；
        lastTrack,
        // 单字节布局：最后 session 首轨道
        data_buf[DISC_INFO_FIRST_TRACK_IN_LAST_SESSION_OFFSET],
        // 末轨道位于 byte 8-9 的固件（低字节即轨道号）
        altLastTrack,
        // 末轨道 + 1（开放轨道紧跟在最后一条已关闭轨道之后）
        static_cast<uint8_t>(lastTrack > 0 ? lastTrack + 1 : 0),
        static_cast<uint8_t>(altLastTrack > 0 ? altLastTrack + 1 : 0),
    };
    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]) && count < maxCount; ++i) {
        uint8_t track = candidates[i];
        if (track == 0 || track == TRACK_INFO_INVISIBLE_TRACK) {
            continue;
        }
        if (std::find(nextTracks, nextTracks + count, track) == nextTracks + count) {
            nextTracks[count++] = track;
        }
    }
    if (count == 0) {
        // 候选全部无效（固件布局未知）：打印原始字节，便于现场判断新布局
        std::ostringstream oss;
        for (size_t i = 0; i < DISC_INFO_RAW_DUMP_LEN; ++i) {
            oss << std::hex << std::setw(HEX_BYTE_WIDTH) << std::setfill('0')
                << static_cast<unsigned int>(data_buf[i]) << ' ';
        }
        LOGI("GetDiscInfoNextTracks: no candidate, raw=[%{public}s]", oss.str().c_str());
    }
    return count > 0 ? E_OK : E_ERR;
}

static uint32_t ReadBe32(const unsigned char *buf)
{
    uint32_t value = 0;
    for (size_t i = 0; i < sizeof(uint32_t); ++i) {
        value = (value << BYTE_SHIFT_8) | static_cast<uint32_t>(buf[i]);
    }
    return value;
}

/*
 * 探测单个候选轨道：命中有效 NWA/Free Blocks 时输出容量。
 * 返回 1=命中，0=未命中（继续尝试其他形式），-1=致命错误（容量溢出）。
 */
static int ProbeBdTrackInfo(int fd, uint8_t addrType, uint32_t addr, bool rcValid, int64_t rcCapacity,
                            int64_t &bdTotalCapacity, int64_t &bdUsedCapacity)
{
    unsigned char cmd_buf[GET_CAPACITY_CMD_BUF_LEN] = {0};
    unsigned char data_buf[GET_CAPACITY_DATA_BUF_LEN] = {0};
    uint8_t senseBuf[SENSE_BUFF_LEN] = {0};
    /*
     * cmd_buf[0]: 操作码 0x52 (GPCMD_READ_TRACK_RZONE_INFO)。
     * cmd_buf[1]: Address/Number Type，00b=按 LBA 寻址（地址在 byte 2-5），
     *             01b=按轨道编号寻址（编号在 byte 5）。
     * cmd_buf[7-8]: 分配长度，48 字节覆盖 BD 轨道信息块。
     */
    cmd_buf[0] = GPCMD_READ_TRACK_RZONE_INFO;
    cmd_buf[1] = addrType;
    if (addrType == 0) {
        cmd_buf[CDB_LBA_BYTE_0] = (addr >> BYTE_SHIFT_24) & BYTE_MASK;
        cmd_buf[CDB_LBA_BYTE_1] = (addr >> BYTE_SHIFT_16) & BYTE_MASK;
        cmd_buf[CDB_LBA_BYTE_2] = (addr >> BYTE_SHIFT_8) & BYTE_MASK;
        cmd_buf[CDB_LBA_BYTE_3] = addr & BYTE_MASK;
    } else {
        cmd_buf[CDB_TRACK_NUMBER_OFFSET] = static_cast<unsigned char>(addr);
    }
    cmd_buf[CDB_ALLOCATION_LENGTH_HIGH] = (GET_CAPACITY_DATA_BUF_LEN >> BYTE_SHIFT_8) & BYTE_MASK;
    cmd_buf[CDB_ALLOCATION_LENGTH_LOW] = GET_CAPACITY_DATA_BUF_LEN & BYTE_MASK;
    ScsiCmdInfo cmdInfo = { cmd_buf, GET_CD_USED_CAPACITY_CMD_LEN, data_buf, GET_CAPACITY_DATA_BUF_LEN };
    int ret = SendScsiCmd(fd, cmdInfo, senseBuf, sizeof(senseBuf));
    if (ret != 0) {
        LOGI("GetBdTrackCapacity: read track info failed, addrType=%{public}u addr=%{public}u ret=%{public}d "
             "senseKey=0x%{public}02X asc=0x%{public}02X ascq=0x%{public}02X", addrType, addr, ret,
             senseBuf[SCSI_SENSE_KEY_OFFSET] & 0x0F, senseBuf[SCSI_ASC_OFFSET], senseBuf[SCSI_ASCQ_OFFSET]);
        return 0;
    }

    bool nwaValid = (data_buf[TRACK_INFO_NWA_VALID_OFFSET] & TRACK_INFO_NWA_VALID_MASK) != 0;
    uint32_t nwa = ReadBe32(&data_buf[TRACK_INFO_NWA_OFFSET]);
    uint32_t freeBlocks = ReadBe32(&data_buf[TRACK_INFO_FREE_BLOCKS_OFFSET]);
    if (!nwaValid || freeBlocks == 0) {
        return 0;   // 未命中开放轨道：静默跳过，由调用方继续尝试下一个候选
    }

    int64_t totalBlocks = static_cast<int64_t>(nwa) + static_cast<int64_t>(freeBlocks);
    if (totalBlocks > INT64_MAX / ODD_LOGICAL_SECTOR_SIZE) {
        LOGE("GetBdTrackCapacity: multiplication overflow, totalBlocks=%{public}" PRId64, totalBlocks);
        return -1;
    }
    bdTotalCapacity = totalBlocks * ODD_LOGICAL_SECTOR_SIZE;
    bdUsedCapacity = static_cast<int64_t>(nwa) * ODD_LOGICAL_SECTOR_SIZE;
    if (rcValid && bdTotalCapacity < rcCapacity) {
        LOGW("GetBdTrackCapacity: track total %{public}" PRId64 " is less than read capacity %{public}" PRId64
             ", use read capacity", bdTotalCapacity, rcCapacity);
        bdTotalCapacity = rcCapacity;
        bdUsedCapacity = -1;
        return 1;
    }
    LOGI("bd track_capacity: addrType=%{public}u addr=%{public}u total=%{public}" PRId64 " used=%{public}" PRId64,
         addrType, addr, bdTotalCapacity, bdUsedCapacity);
    return 1;
}

/*
 * 使用 SCSI READ TRACK INFORMATION 指令 (0x52) 查询未封盘 BD-R 的 invisible track，
 * 通过 NWA (Next Writable Address) + Free Blocks 计算数据区总容量，NWA 即已用容量。
 * 依次尝试三种寻址形式：
 * - 形式一：轨道编号 0xFF（invisible track）。标准驱动器支持；部分固件对 BD 盘拒绝
 *   该形式（ILLEGAL_REQUEST / ASC=0x24 invalid field in CDB，实测 Verbatim 66768）；
 * - 形式二：轨道编号取 READ DISC INFORMATION 给出的候选开放轨道号（可能多个候选，
 *   取决于固件字段布局）。追加刻录后原开放轨道被关闭、开放轨道前移时，该形式仍能
 *   准确定位（实测验证）；每个候选都用 nwaValid && freeBlocks > 0 校验，错误候选
 *   会因命中已关闭轨道或不存在轨道而被丢弃；
 * - 形式三：按 LBA 寻址，LBA 取 READ CAPACITY 的已记录区末尾——未封盘 BD-R 的
 *   invisible track 起始地址恰为该值，可命中开放轨道。
 * 三种形式均无法取到有效 NWA/Free Blocks 时（已封盘、整盘预留 RRM 等），
 * 回退返回 READ CAPACITY 值，bdUsedCapacity 置 -1 交由调用方处理。
 * 形式二所需的 READ DISC INFORMATION 仅在形式一失败后才读取，避免多余 SCSI 命令。
 */
int GetBdTrackCapacity(int fd, int64_t &bdTotalCapacity, int64_t &bdUsedCapacity)
{
    bdTotalCapacity = 0;
    bdUsedCapacity = -1;
    int64_t rcCapacity = 0;
    bool rcValid = (GetReadCapacitySize(fd, rcCapacity) == E_OK) && rcCapacity > 0;
    uint32_t rcEndBlocks = rcValid ? static_cast<uint32_t>(rcCapacity / ODD_LOGICAL_SECTOR_SIZE) : 0;

    int probe = ProbeBdTrackInfo(fd, 1, TRACK_INFO_INVISIBLE_TRACK, rcValid, rcCapacity,
                                 bdTotalCapacity, bdUsedCapacity);
    if (probe != 0) {
        return probe > 0 ? E_OK : E_ERR;
    }

    uint8_t nextTracks[DISC_INFO_MAX_NEXT_TRACKS] = {0};
    size_t nextTrackCount = 0;
    if (GetDiscInfoNextTracks(fd, nextTracks, DISC_INFO_MAX_NEXT_TRACKS, nextTrackCount) == E_OK) {
        for (size_t i = 0; i < nextTrackCount; ++i) {
            probe = ProbeBdTrackInfo(fd, 1, nextTracks[i], rcValid, rcCapacity,
                                     bdTotalCapacity, bdUsedCapacity);
            if (probe != 0) {
                return probe > 0 ? E_OK : E_ERR;
            }
        }
    }

    probe = ProbeBdTrackInfo(fd, 0, rcEndBlocks, rcValid, rcCapacity, bdTotalCapacity, bdUsedCapacity);
    if (probe != 0) {
        return probe > 0 ? E_OK : E_ERR;
    }

    if (rcValid) {
        LOGI("GetBdTrackCapacity: no valid track info, fallback to READ CAPACITY: %{public}" PRId64, rcCapacity);
        bdTotalCapacity = rcCapacity;
        return E_OK;
    }
    return E_ERR;
}

std::string GetScsiBusNum(const std::string &sysPath)
{
    LOGD("[L3:DiskUtils] GetScsiBusNum: >>> ENTER <<<");

    std::string deviceLinkPath = sysPath + "/device";
    char linkTarget[PATH_MAX] = {0};
    ssize_t len = readlink(deviceLinkPath.c_str(), linkTarget, sizeof(linkTarget) - 1);
    if (len > 0) {
        linkTarget[len] = '\0';
        std::string linkStr(linkTarget);
        size_t lastSlash = linkStr.find_last_of('/');
        if (lastSlash != std::string::npos && lastSlash + 1 < linkStr.length()) {
            std::string scsiAddr = linkStr.substr(lastSlash + 1);
            if (scsiAddr.find(':') != std::string::npos) {
                LOGD("[L3:DiskUtils] GetScsiBusNum: SCSI_BUS_NUM=%{public}s", scsiAddr.c_str());
                return scsiAddr;
            }
        }
    }

    LOGD("[L3:DiskUtils] GetScsiBusNum: <<< EXIT NOT FOUND <<< (empty)");
    return "";
}

std::string GetOddDriverType(const std::string &sysPath)
{
    LOGD("[L3:DiskUtils] GetOddDriverType: >>> ENTER <<<");

    if (sysPath.find("usb") != std::string::npos) {
        LOGD("[L3:DiskUtils] GetOddDriverType: ODD_DRIVER_TYPE=usb-storage");
        return "usb-storage";
    } else if (sysPath.find("sata") != std::string::npos) {
        LOGD("[L3:DiskUtils] GetOddDriverType: ODD_DRIVER_TYPE=AHCI");
        return "AHCI";
    }

    LOGD("[L3:DiskUtils] GetOddDriverType: <<< EXIT SUCCESS <<< (empty)");
    return "";
}
} // namespace StorageDaemon
} // namespace OHOS
