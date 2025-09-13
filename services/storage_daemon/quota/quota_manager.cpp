/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "quota/quota_manager.h"

#include <charconv>
#include <dirent.h>
#include <fstream>
#include <linux/fs.h>
#include <linux/quota.h>
#include <stack>
#include <sys/quota.h>
#include <thread>
#include <unistd.h>

#include "file_uri.h"
#include "sandbox_helper.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "storage_service_constant.h"
#include "utils/file_utils.h"
#include "utils/storage_radar.h"
#include "utils/string_utils.h"

namespace OHOS {
namespace StorageDaemon {
constexpr const char *QUOTA_DEVICE_DATA_PATH = "/data";
constexpr const char *PROC_MOUNTS_PATH = "/proc/mounts";
constexpr const char *DEV_BLOCK_PATH = "/dev/block/";
constexpr const char *CONFIG_FILE_PATH = "/etc/passwd";
constexpr const char *DATA_DEV_PATH = "/dev/block/by-name/userdata";
constexpr uint64_t ONE_KB = 1;
constexpr uint64_t ONE_MB = 1024 * ONE_KB;
constexpr int32_t ONE_HUNDRED_M_BYTE = 1024 * 1024 * 100;
constexpr uint64_t PATH_MAX_LEN = 4096;
constexpr double DIVISOR = 1024.0 * 1024.0;
constexpr double BASE_NUMBER = 10.0;
constexpr int32_t ONE_MS = 1000;
constexpr int32_t ACCURACY_NUM = 2;
constexpr int32_t MAX_UID_COUNT = 100000;
constexpr int32_t BLOCK_BYTE = 512;
constexpr int32_t TOP_SPACE_COUNT = 20;
static std::map<std::string, std::string> mQuotaReverseMounts;
static std::vector<int32_t> SYS_UIDS = {0, 1000, 5523};
#define Q_GETNEXTQUOTA_LOCAL 0x800009
std::recursive_mutex mMountsLock;

struct NextDqBlk {
    /* Absolute limit on disk quota blocks alloc */
    uint64_t dqbHardLimit;
    /* Preferred limit on disk quota blocks */
    uint64_t dqbBSoftLimit;
    /* Current occupied space(in bytes) */
    uint64_t dqbCurSpace;
    /* Maximum number of allocated inodes */
    uint64_t dqbIHardLimit;
    /* Preferred inode limit */
    uint64_t dqbISoftLimit;
    /* Current number of allocated inodes */
    uint64_t dqbCurInodes;
    /* Time limit for excessive disk use */
    uint64_t dqbBTime;
    /* Time limit for excessive files */
    uint64_t dqbITime;
    /* Bit mask of QIF_* constants */
    uint32_t dqbValid;
    /* the next ID greater than or equal to id that has a quota set */
    uint32_t dqbId;
};

static std::vector<DirSpaceInfo> GetRootDir()
{
    return {{"/data/app/el1/0/shader_cache", 0, 0},
            {"/data/log/bbox", 0, 0},
            {"/data/log/sysres", 0, 0},
            {"/data/log/last_tee", 0, 0},
            {"/data/log/security_guard", 0, 0},
            {"/data/log/hiaudit/storageservice", 0, 0},
            {"/data/app/el1/bundle/public", 0, 0},
            {"/data/service/el1/public/udev", 0, 0},
            {"/data/service/el1/public/i18n", 0, 0},
            {"/data/service/el1/public/hosts_user", 0, 0},
            {"/data/service/el1/public/usb/mode", 0, 0},
            {"/data/service/el0/startup", 0, 0},
            {"/data/service/el0/access_token", 0, 0},
            {"/data/service/el0/storage_daemon", 0, 0},
            {"/data/service/el0/public/for-all-app", 0, 0},
            {"/data/service/el1/startup", 0, 0},
            {"/data/service/el1/public/for-all-app", 0, 0},
            {"/data/service/el1/public/startup", 0, 0},
            {"/data/service/el1/public/storage_daemon", 0, 0},
            {"/data/service/el1/public/hdc", 0, 0},
            {"/data/service/el1/public/usb/mode", 0, 0},
            {"/data/service/el1/public/udev", 0, 0},
            {"/data/service/el1/public/i18n", 0, 0},
            {"/data/service/el1/public/rgm_engine", 0, 0},
            {"/data/service/el1/network", 0, 0},
            {"/data/service/el1/0/hyperhold", 0, 0},
            {"/data/log/hiaudit/storageservice", 0, 0},
            {"/data/local/shader_cache", 0, 0},
            {"/data/local/tmp", 0, 0},
            {"/data/app/el1/0/aot_compiler", 0, 0},
            {"/data/app/el1/0/base", 0, 0},
            {"/data/app/el1/0/system_optimize", 0, 0},
            {"/data/app/el1/public/aot_compiler", 0, 0},
            {"/data/app/el1/public/shader_cache", 0, 0},
            {"/data/app/el1/%d/aot_compiler", 0, 0},
            {"/data/app/el1/%d/shader_cache", 0, 0},
            {"/data/app/el1/%d/system_optimize", 0, 0},
            {"/data/apr_dumplogs", 0, 0},
            {"/data/misc/shared_relro", 0, 0},
            {"/data/vendor/hyperhold", 0, 0},
            {"/data/vendor/log/thplog", 0, 0},
            {"/data/vendor/log/dfx_logs", 0, 0},
            {"/data/vendor/log/wifi", 0, 0},
            {"/data/vendor/log/hi110x", 0, 0},
            {"/data/hisi_logs", 0, 0},
            {"/data/uwb", 0, 0},
            {"/data/log/tcpdump", 0, 0},
            {"/data/virt_service", 0, 0},
            {"/data/updater/log", 0, 0}};
}

static std::vector<DirSpaceInfo> GetSystemDir()
{
    return {{"/data/hmos4", 1000, 0},
            {"/data/service/el1/public/for-all-app/framework_ark_cache", 1000, 0},
            {"/data/service/el1/public/shader_cache", 1000, 0},
            {"/data/service/el2/100/virt_service", 1000, 0},
            {"/data/log/faultlog/temp", 1000, 0},
            {"/data/log/hiview", 1000, 0},
            {"/data/system/RTShaderCache", 1000, 0},
            {"/data/local/shader_cache/local/system", 1000, 0},
            {"/data/local/mtp_tmp", 1000, 0},
            {"/data/virt_service", 1000, 0},
            {"/data/misc", 1000, 0},
            {"/data/vendor/log", 1000, 0},
            {"/data/data", 1000, 0}};
}

static std::vector<DirSpaceInfo> GetFoundationDir()
{
    return {{"/data/service/el1/public/AbilityManagerService", 5523, 0},
            {"/data/service/el1/public/database/bundle_manager_service", 5523, 0},
            {"/data/service/el1/public/database/notification_service", 5523, 0},
            {"/data/service/el1/public/database/form_storage", 5523, 0},
            {"/data/service/el1/public/database/common_event_service", 5523, 0},
            {"/data/service/el1/public/database/auto_startup_service", 5523, 0},
            {"/data/service/el1/public/database/app_config_data", 5523, 0},
            {"/data/service/el1/public/database/app_exit_reason", 5523, 0},
            {"/data/service/el1/public/database/ability_manager_service", 5523, 0},
            {"/data/service/el1/public/database/keep_alive_service", 5523, 0},
            {"/data/service/el1/public/database/insight_intent", 5523, 0},
            {"/data/service/el1/public/notification", 5523, 0},
            {"/data/service/el1/public/window", 5523, 0},
            {"/data/service/el1/public/ecological_rule_mgr_service", 5523, 0},
            {"/data/service/el1/public/app_domain_verify_mgr_service", 5523, 0},
            {"/data/service/el1/public/screenlock", 5523, 0},
            {"/data/service/el1/public/bms/bundle_manager_service", 5523, 0},
            {"/data/service/el1/public/bms/bundle_resources", 5523, 0},
            {"/data/service/el1/0/utdtypes", 5523, 0},
            {"/data/service/el1/%d/utdtypes", 5523, 0},
            {"/data/log/eventlog/freeze", 5523, 0}};
}

QuotaManager &QuotaManager::GetInstance()
{
    static QuotaManager instance_;
    return instance_;
}

static bool InitialiseQuotaMounts()
{
    std::lock_guard<std::recursive_mutex> lock(mMountsLock);
    mQuotaReverseMounts.clear();
    std::ifstream in(PROC_MOUNTS_PATH);

    if (!in.is_open()) {
        LOGE("Failed to open mounts file");
        return false;
    }
    std::string source;
    std::string target;
    std::string ignored;

    while (in.peek() != EOF) {
        std::getline(in, source, ' ');
        std::getline(in, target, ' ');
        std::getline(in, ignored);
        if (source.compare(0, strlen(DEV_BLOCK_PATH), DEV_BLOCK_PATH) == 0) {
            struct dqblk dq;
            if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), source.c_str(), 0, reinterpret_cast<char*>(&dq)) == 0) {
                mQuotaReverseMounts[target] = source;
            }
        }
    }

    return true;
}

static std::string GetQuotaSrcMountPath(const std::string &target)
{
    std::lock_guard<std::recursive_mutex> lock(mMountsLock);
    if (mQuotaReverseMounts.find(target) != mQuotaReverseMounts.end()) {
        return mQuotaReverseMounts[target];
    } else {
        return "";
    }
}

static int64_t GetOccupiedSpaceForUid(int32_t uid, int64_t &size)
{
    LOGE("GetOccupiedSpaceForUid uid:%{public}d", uid);
    struct dqblk dq;
#ifdef ENABLE_EMULATOR
    if (!InitialiseQuotaMounts()) {
        LOGE("Failed to initialise quota mounts");
        return E_INIT_QUOTA_MOUNTS_FAILED;
    }
    std::string device = "";
    device = GetQuotaSrcMountPath(QUOTA_DEVICE_DATA_PATH);
    if (device.empty()) {
        LOGE("skip when device no quotas present");
        return E_OK;
    }
    if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), device.c_str(), uid, reinterpret_cast<char*>(&dq)) == 0) {
        size = static_cast<int64_t>(dq.dqb_curspace);
        LOGE("get size for emulator by quota success, size is %{public}s", std::to_string(size).c_str());
        return E_OK;
    }
    LOGE("get size for emulator by quota failed, errno is %{public}d", errno);
#else
    if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), DATA_DEV_PATH, uid, reinterpret_cast<char*>(&dq)) == 0) {
        size = static_cast<int64_t>(dq.dqb_curspace);
        LOGE("get size by quota success, size is %{public}s", std::to_string(size).c_str());
        return E_OK;
    }
    LOGE("get size by quota failed, errno is %{public}d", errno);
#endif
    return E_QUOTA_CTL_KERNEL_ERR;
}

void QuotaManager::GetUidStorageStats(const std::string &storageStatus)
{
    LOGI("GetUidStorageStats begin!");
    std::vector<UidSaInfo> vec;
    auto ret = ParseConfigFile(CONFIG_FILE_PATH, vec);
    if (ret != E_OK) {
        LOGE("parsePasswd File failed.");
        return;
    }
 
    ret = GetOccupiedSpaceForUidList(vec);
    if (ret != E_OK) {
        return;
    }
 
    std::sort(vec.begin(), vec.end(), [](const UidSaInfo& a, const UidSaInfo& b) {
    return a.size > b.size;
    });
    std::ostringstream extraData;
    extraData << storageStatus <<std::endl;
    for (const auto& info : vec) {
        if (info.size < ONE_HUNDRED_M_BYTE) {
            continue;
        }
        extraData << "{uid:" << info.uid
            << ",saName:" << info.saName
            << ",size:" << ConvertBytesToMB(info.size, ACCURACY_NUM)
            << "MB}"<<std::endl;
    }
    LOGI("extraData is %{public}s", extraData.str().c_str());
    StorageService::StorageRadar::ReportSaSizeResult("QuotaManager::GetUidStorageStats", E_STORAGE_STATUS,
        extraData.str());
    LOGI("GetUidStorageStats end!");
}

double QuotaManager::ConvertBytesToMB(int64_t bytes, int32_t decimalPlaces)
{
    if (bytes < 0) {
        return 0.0;
    }
    double mb = static_cast<double>(bytes) / DIVISOR;
 
    if (decimalPlaces < 0) {
        decimalPlaces = 0;
    }
    double factor = std::pow(BASE_NUMBER, decimalPlaces);
    if (factor == 0) {
        return 0.0;
    }
    return std::round(mb * factor) / factor;
}
 
bool QuotaManager::StringToInt32(const std::string &strUid, int32_t &outUid32)
{
    if (strUid.empty()) {
        return false;
    }
    for (char ch : strUid) {
        if (!std::isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    uint64_t uid;
    auto res = std::from_chars(strUid.data(), strUid.data() + strUid.size(), uid);
    if (res.ec != std::errc()) {
        return false;
    }
    if (uid > static_cast<uint64_t>(INT32_MAX)) {
        return false;
    }
    outUid32 = static_cast<int32_t>(uid);
    return true;
}
 
bool QuotaManager::GetUid32FromEntry(const std::string &entry, int32_t &outUid32, std::string &saName)
{
    size_t firstColon = entry.find(':');
    if (firstColon == std::string::npos) {
        return false;
    }
    saName = entry.substr(0, firstColon);
    size_t secondColon = entry.find(':', firstColon + 1);
    if (secondColon == std::string::npos) {
        return false;
    }
    size_t thirdColon = entry.find(':', secondColon + 1);
    if (thirdColon == std::string::npos) {
        return false;
    }
    std::string uidStr = entry.substr(secondColon + 1, thirdColon - (secondColon + 1));
    return StringToInt32(uidStr, outUid32);
}
 
int32_t QuotaManager::ParseConfigFile(const std::string &path, std::vector<struct UidSaInfo> &vec)
{
    LOGI("pasePasswdFile begin!");
    char realPath[PATH_MAX] = {0x00};
    if (realpath(path.c_str(), realPath) == nullptr) {
        LOGE("path not valid, path = %{private}s", path.c_str());
        return E_JSON_PARSE_ERROR;
    }
 
    std::ifstream infile(std::string(realPath), std::ios::in);
    if (!infile.is_open()) {
        LOGE("Open file failed, errno = %{public}d", errno);
        return E_OPEN_JSON_FILE_ERROR;
    }
 
    std::string line;
    while (getline(infile, line)) {
        if (line == "") {
            continue;
        }
        struct UidSaInfo info;
        if (GetUid32FromEntry(line, info.uid, info.saName)) {
            vec.push_back(info);
        }
    }
    infile.close();
    LOGI("pasePasswdFile end!");
    return E_OK;
}
 
int64_t QuotaManager::GetOccupiedSpaceForUidList(std::vector<struct UidSaInfo> &vec)
{
    LOGI("GetOccupiedSpaceForUidList begin!");
    int32_t curUid = 0;
    int32_t count = 0;
    std::map<int32_t, int64_t> userAppSizeMap;
    while (count < MAX_UID_COUNT) {
        struct NextDqBlk dq;
        if (quotactl(QCMD(Q_GETNEXTQUOTA_LOCAL, USRQUOTA), DATA_DEV_PATH, curUid, reinterpret_cast<char*>(&dq)) != 0) {
            LOGE("failed to get next quota, uid is %{public}d, errno is %{public}d,", curUid, errno);
            break;
        }
        int32_t dqUid = static_cast<int32_t>(dq.dqbId);
        for (struct UidSaInfo &info : vec) {
            if (info.uid == dqUid) {
                info.size = static_cast<int64_t>(dq.dqbCurSpace);
                break;
            }
        }
        if (dqUid >= StorageService::APP_UID) {
            int32_t userId = dqUid / StorageService::USER_ID_BASE;
            if (userAppSizeMap.find(userId) != userAppSizeMap.end()) {
                userAppSizeMap[userId] += static_cast<int64_t>(dq.dqbCurSpace);
            } else {
                userAppSizeMap[userId] = static_cast<int64_t>(dq.dqbCurSpace);
            }
        }
        if (dqUid >= StorageService::ZERO_USER_MIN_UID && dqUid <= StorageService::ZERO_USER_MAX_UID) {
            int32_t userId = StorageService::ZERO_USER;
            if (userAppSizeMap.find(userId) != userAppSizeMap.end()) {
                userAppSizeMap[userId] += static_cast<int64_t>(dq.dqbCurSpace);
            } else {
                userAppSizeMap[userId] = static_cast<int64_t>(dq.dqbCurSpace);
            }
        }
        count++;
        curUid = dqUid + 1;
        if (curUid == 0) {
            break;
        }
        usleep(ONE_MS);
    }
    for (const auto &pair : userAppSizeMap) {
        UidSaInfo info = {pair.first, "userId", pair.second};
        vec.push_back(info);
    }
    LOGI("GetOccupiedSpaceForUidList end!");
    return E_OK;
}

static int64_t GetOccupiedSpaceForGid(int32_t gid, int64_t &size)
{
    LOGE("GetOccupiedSpaceForGid gid:%{public}d", gid);
    if (InitialiseQuotaMounts() != true) {
        LOGE("Failed to initialise quota mounts");
        return E_INIT_QUOTA_MOUNTS_FAILED;
    }

    std::string device = "";
    device = GetQuotaSrcMountPath(QUOTA_DEVICE_DATA_PATH);
    if (device.empty()) {
        LOGE("skip when device no quotas present");
        return E_OK;
    }

    struct dqblk dq;
    if (quotactl(QCMD(Q_GETQUOTA, GRPQUOTA), device.c_str(), gid, reinterpret_cast<char*>(&dq)) != 0) {
        LOGE("Failed to get quotactl, errno : %{public}d", errno);
        return E_QUOTA_CTL_KERNEL_ERR;
    }

    size = static_cast<int64_t>(dq.dqb_curspace);
    LOGE("GetOccupiedSpaceForGid size:%{public}s", std::to_string(size).c_str());
    return E_OK;
}


static int64_t GetOccupiedSpaceForPrjId(int32_t prjId, int64_t &size)
{
    LOGE("GetOccupiedSpaceForPrjId prjId:%{public}d", prjId);
    if (InitialiseQuotaMounts() != true) {
        LOGE("Failed to initialise quota mounts");
        return E_INIT_QUOTA_MOUNTS_FAILED;
    }

    std::string device = "";
    device = GetQuotaSrcMountPath(QUOTA_DEVICE_DATA_PATH);
    if (device.empty()) {
        LOGE("skip when device no quotas present");
        return E_OK;
    }

    struct dqblk dq;
    if (quotactl(QCMD(Q_GETQUOTA, PRJQUOTA), device.c_str(), prjId, reinterpret_cast<char*>(&dq)) != 0) {
        LOGE("Failed to get quotactl, errno : %{public}d", errno);
        return E_QUOTA_CTL_KERNEL_ERR;
    }

    size = static_cast<int64_t>(dq.dqb_curspace);
    LOGE("GetOccupiedSpaceForPrjId size:%{public}s", std::to_string(size).c_str());
    return E_OK;
}

int32_t QuotaManager::GetOccupiedSpace(int32_t idType, int32_t id, int64_t &size)
{
    switch (idType) {
        case USRID:
            return GetOccupiedSpaceForUid(id, size);
            break;
        case GRPID:
            return GetOccupiedSpaceForGid(id, size);
            break;
        case PRJID:
            return GetOccupiedSpaceForPrjId(id, size);
            break;
        default:
            return E_NON_EXIST;
    }
    return E_OK;
}

int32_t QuotaManager::SetBundleQuota(const std::string &bundleName, int32_t uid,
    const std::string &bundleDataDirPath, int32_t limitSizeMb)
{
    if (bundleName.empty() || bundleDataDirPath.empty() || uid < 0 || limitSizeMb < 0) {
        LOGE("Calling the function SetBundleQuota with invalid param");
        return E_PARAMS_INVALID;
    }

    LOGE("SetBundleQuota Start, bundleName is %{public}s, uid is %{public}d, bundleDataDirPath is %{public}s, "
         "limit is %{public}d.", bundleName.c_str(), uid, bundleDataDirPath.c_str(), limitSizeMb);
    if (InitialiseQuotaMounts() != true) {
        LOGE("Failed to initialise quota mounts");
        return E_INIT_QUOTA_MOUNTS_FAILED;
    }

    std::string device = "";
    if (bundleDataDirPath.find(QUOTA_DEVICE_DATA_PATH) == 0) {
        device = GetQuotaSrcMountPath(QUOTA_DEVICE_DATA_PATH);
    }
    if (device.empty()) {
        LOGE("skip when device no quotas present");
        return E_OK;
    }

    struct dqblk dq;
    if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), device.c_str(), uid, reinterpret_cast<char*>(&dq)) != 0) {
        LOGE("Failed to get hard quota, errno : %{public}d", errno);
        return E_QUOTA_CTL_KERNEL_ERR;
    }

    // dqb_bhardlimit is count of 1kB blocks, dqb_curspace is bytes
    struct statvfs stat;
    if (statvfs(bundleDataDirPath.c_str(), &stat) != 0) {
        LOGE("Failed to statvfs, errno : %{public}d", errno);
        return E_STAT_VFS_KERNEL_ERR;
    }

    dq.dqb_valid = QIF_LIMITS;
    dq.dqb_bhardlimit = (uint32_t)limitSizeMb * ONE_MB;
    if (quotactl(QCMD(Q_SETQUOTA, USRQUOTA), device.c_str(), uid, reinterpret_cast<char*>(&dq)) != 0) {
        LOGE("Failed to set hard quota, errno : %{public}d", errno);
        return E_QUOTA_CTL_KERNEL_ERR;
    } else {
        LOGD("Applied hard quotas ok");
        return E_OK;
    }
}

int32_t QuotaManager::SetQuotaPrjId(const std::string &path, int32_t prjId, bool inherit)
{
    struct fsxattr fsx;
    char *realPath = realpath(path.c_str(), nullptr);
    if (realPath == nullptr) {
        LOGE("realpath failed");
        return E_PARAMS_NULLPTR_ERR;
    }
    FILE *f = fopen(realPath, "r");
    free(realPath);
    if (f == nullptr) {
        LOGE("Failed to open %{public}s, errno: %{public}d", path.c_str(), errno);
        return E_SYS_KERNEL_ERR;
    }
    int fd = fileno(f);
    if (fd < 0) {
        (void)fclose(f);
        return E_SYS_KERNEL_ERR;
    }
    if (ioctl(fd, FS_IOC_FSGETXATTR, &fsx) == -1) {
        LOGE("Failed to get extended attributes of %{public}s, errno: %{public}d", path.c_str(), errno);
        (void)fclose(f);
        return E_SYS_KERNEL_ERR;
    }
    if (fsx.fsx_projid == static_cast<uint32_t>(prjId)) {
        (void)fclose(f);
        return E_OK;
    }
    fsx.fsx_projid = static_cast<uint32_t>(prjId);
    if (ioctl(fd, FS_IOC_FSSETXATTR, &fsx) == -1) {
        LOGE("Failed to set project id for %{public}s, errno: %{public}d", path.c_str(), errno);
        (void)fclose(f);
        return E_SYS_KERNEL_ERR;
    }

    if (inherit) {
        uint32_t flags;
        if (ioctl(fd, FS_IOC_GETFLAGS, &flags) == -1) {
            LOGE("Failed to get flags for %{public}s, errno:%{public}d", path.c_str(), errno);
            (void)fclose(f);
            return E_SYS_KERNEL_ERR;
        }
        flags |= FS_PROJINHERIT_FL;
        if (ioctl(fd, FS_IOC_SETFLAGS, &flags) == -1) {
            LOGE("Failed to set flags for %{public}s, errno:%{public}d", path.c_str(), errno);
            (void)fclose(f);
            return E_SYS_KERNEL_ERR;
        }
    }
    (void)fclose(f);
    return E_OK;
}

uint32_t CheckOverLongPath(const std::string &path)
{
    uint32_t len = path.length();
    if (len >= PATH_MAX_LEN) {
        size_t found = path.find_last_of('/');
        std::string sub = path.substr(found + 1);
        LOGE("Path over long, length:%{public}d, fileName:%{public}s.", len, sub.c_str());
    }
    return len;
}

int32_t QuotaManager::StatisticSysDirSpace()
{
    LOGI("statistic sys dir space start.");
    if (!IsNeedScan()) {
        LOGI("not need to statistic sys dir space.");
        return E_OK;
    }
    std::ostringstream extraData;
    std::vector<int32_t> userIds;
    GetAllUserIds(userIds);
    if (userIds.empty()) {
        userIds.push_back(StorageService::DEFAULT_USER_ID);
    }
    std::vector<DirSpaceInfo> dirs = GetRootDir();
    std::string res = AddDirSpace(dirs, userIds);
    extraData << res << std::endl;

    dirs = GetSystemDir();
    res = AddDirSpace(dirs, userIds);
    extraData << res << std::endl;

    dirs = GetFoundationDir();
    res = AddDirSpace(dirs, userIds);
    extraData << res << std::endl;
    StorageService::StorageRadar::ReportSpaceRadar("StatisticSysDirSpace", E_SYS_DIR_SPACE_STATUS, extraData.str());
    LOGI("statistic sys dir space end.");
    return E_OK;
}

bool QuotaManager::IsNeedScan()
{
    for (auto &uid : SYS_UIDS) {
        struct dqblk dq;
        if (quotactl(QCMD(Q_GETQUOTA, GRPQUOTA), DATA_DEV_PATH, uid, reinterpret_cast<char*>(&dq)) != 0) {
            int32_t errnoTmp = errno;
            std::string extraData = "uid=" + uid + ",kernelCode=" + std::to_string(errnoTmp);
            StorageService::StorageRadar::ReportSpaceRadar("IsNeedScan", E_STATISTIC_QUOTA_UID_FAILED, extraData);
            LOGE("failed to get quota, uid is %{public}d, errno is %{public}d", uid, errno);
            return true;
        }
        int64_t size = static_cast<int64_t>(dq.dqb_curspace);
        if (size > StorageService::TWO_G_BYTE) {
            LOGI("sys uid bigger than two gb, uid is %{public}d.", uid);
            return true;
        }
    }
    return false;
}

std::string QuotaManager::AddDirSpace(const std::vector<DirSpaceInfo> &dirs, const std::vector<int32_t> &userIds)
{
    std::vector<DirSpaceInfo> allPaths;
    for (const auto &dirInfo : dirs) {
        std::string path = dirInfo.path;
        uid_t type = dirInfo.type;
        if (path.find("%d") == std::string::npos) {
            int64_t blks = 0;
            AddBlksRecurse(path, blks, type);
            int64_t dirSize = blks * BLOCK_BYTE;
            allPaths.push_back({path, type, dirSize});
            continue;
        }
        for (const int32_t userId : userIds) {
            std::string userPath = StringPrintf(path.c_str(), userId);
            int64_t blks = 0;
            AddBlksRecurse(userPath, blks, type);
            int64_t dirSize = blks * BLOCK_BYTE;
            allPaths.push_back({userPath, type, dirSize});
        }
    }
    std::sort(allPaths.begin(), allPaths.end(), [](const DirSpaceInfo& a, const DirSpaceInfo& b) {
        return a.size > b.size;
    });
    std::ostringstream extraData;
    int32_t count = 1;
    for (const auto& info : allPaths) {
        if (count > TOP_SPACE_COUNT) {
            break;
        }
        extraData << "{path:" << info.path
                  << ",uid:" << info.uid
                  << ",size:" << ConvertBytesToMB(info.size, ACCURACY_NUM)
                  << "MB}" << std::endl;
        count++;
    }
    LOGI("extraData is %{public}s", extraData.str().c_str());
    return extraData.str();
}

int32_t QuotaManager::AddBlksRecurse(const std::string &path, int64_t &blks, uid_t uid)
{
    AddBlks(path, blks, uid);
    if (!IsDir(path)) {
        return E_OK;
    }
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        LOGE("open dir %{public}s failed, errno %{public}d", path.c_str(), errno);
        return E_STATISTIC_OPEN_DIR_FAILED;
    }
    int ret = E_OK;
    for (struct dirent *ent = readdir(dir); ent != nullptr; ent = readdir(dir)) {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0)) {
            continue;
        }
        std::string subPath = path + "/" + ent->d_name;
        int32_t retTmp = AddBlksRecurse(subPath, blks, uid);
        if (retTmp != E_OK) {
            ret = retTmp;
        }
    }
    (void)closedir(dir);
    return ret;
}

int32_t QuotaManager::AddBlks(const std::string &path, int64_t &blks, uid_t uid)
{
    struct stat st;
    if (lstat(path.c_str(), &st) != E_OK) {
        int32_t errnoTmp = errno;
        std::string extraData = "path=" + path + ",kernelCode=" + std::to_string(errnoTmp);
        StorageService::StorageRadar::ReportSpaceRadar("AddBlks", E_STATISTIC_STAT_FAILED, extraData);
        LOGE("lstat failed, path is %{public}s, errno is %{public}d", path.c_str(), errno);
        return E_STATISTIC_STAT_FAILED;
    }
    if (uid == st.st_uid) {
        blks += static_cast<int64_t>(st.st_blocks);
    }
    return E_OK;
}
} // namespace STORAGE_DAEMON
} // namespace OHOS
