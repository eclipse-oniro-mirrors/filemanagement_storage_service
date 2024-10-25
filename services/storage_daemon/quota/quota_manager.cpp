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

#include <cstdint>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <linux/dqblk_xfs.h>
#include <linux/fs.h>
#include <linux/quota.h>
#include <map>
#include <sstream>
#include <stack>
#include <sys/ioctl.h>
#include <sys/quota.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <tuple>
#include <unique_fd.h>
#include <unistd.h>
#include <cstdio>

#include "file_uri.h"
#include "sandbox_helper.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "storage_service_constant.h"
#include "utils/file_utils.h"

namespace OHOS {
namespace StorageDaemon {
const std::string QUOTA_DEVICE_DATA_PATH = "/data";
const std::string PROC_MOUNTS_PATH = "/proc/mounts";
const std::string DEV_BLOCK_PATH = "/dev/block/";
const char LINE_SEP = '\n';
const int32_t DEV_BLOCK_PATH_LEN = DEV_BLOCK_PATH.length();
const uint64_t ONE_KB = 1;
const uint64_t ONE_MB = 1024 * ONE_KB;
const uint64_t PATH_MAX_LEN = 4096;
static std::map<std::string, std::string> mQuotaReverseMounts;
std::recursive_mutex mMountsLock;

QuotaManager* QuotaManager::instance_ = nullptr;
QuotaManager* QuotaManager::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new QuotaManager();
    }

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
        if (source.compare(0, DEV_BLOCK_PATH_LEN, DEV_BLOCK_PATH) == 0) {
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
    if (InitialiseQuotaMounts() != true) {
        LOGE("Failed to initialise quota mounts");
        return E_SYS_ERR;
    }

    std::string device = "";
    device = GetQuotaSrcMountPath(QUOTA_DEVICE_DATA_PATH);
    if (device.empty()) {
        LOGE("skip when device no quotas present");
        return E_OK;
    }

    struct dqblk dq;
    if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), device.c_str(), uid, reinterpret_cast<char*>(&dq)) != 0) {
        LOGE("Failed to get quotactl, errno : %{public}d", errno);
        return E_SYS_ERR;
    }

    size = static_cast<int64_t>(dq.dqb_curspace);
    return E_OK;
}

static int64_t GetOccupiedSpaceForGid(int32_t gid, int64_t &size)
{
    if (InitialiseQuotaMounts() != true) {
        LOGE("Failed to initialise quota mounts");
        return E_SYS_ERR;
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
        return E_SYS_ERR;
    }

    size = static_cast<int64_t>(dq.dqb_curspace);
    return E_OK;
}


static int64_t GetOccupiedSpaceForPrjId(int32_t prjId, int64_t &size)
{
    if (InitialiseQuotaMounts() != true) {
        LOGE("Failed to initialise quota mounts");
        return E_SYS_ERR;
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
        return E_SYS_ERR;
    }

    size = static_cast<int64_t>(dq.dqb_curspace);
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
        return E_NON_EXIST;
    }

    LOGE("SetBundleQuota Start, bundleName is %{public}s, uid is %{public}d, bundleDataDirPath is %{public}s, "
         "limit is %{public}d.", bundleName.c_str(), uid, bundleDataDirPath.c_str(), limitSizeMb);
    if (InitialiseQuotaMounts() != true) {
        LOGE("Failed to initialise quota mounts");
        return E_NON_EXIST;
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
        return E_SYS_CALL;
    }

    // dqb_bhardlimit is count of 1kB blocks, dqb_curspace is bytes
    struct statvfs stat;
    if (statvfs(bundleDataDirPath.c_str(), &stat) != 0) {
        LOGE("Failed to statvfs, errno : %{public}d", errno);
        return E_SYS_CALL;
    }

    dq.dqb_valid = QIF_LIMITS;
    dq.dqb_bhardlimit = (uint32_t)limitSizeMb * ONE_MB;
    if (quotactl(QCMD(Q_SETQUOTA, USRQUOTA), device.c_str(), uid, reinterpret_cast<char*>(&dq)) != 0) {
        LOGE("Failed to set hard quota, errno : %{public}d", errno);
        return E_SYS_CALL;
    } else {
        LOGE("Applied hard quotas ok");
        return E_OK;
    }
}

int32_t QuotaManager::SetQuotaPrjId(const std::string &path, int32_t prjId, bool inherit)
{
    struct fsxattr fsx;
    char *realPath = realpath(path.c_str(), nullptr);
    if (realPath == nullptr) {
        LOGE("realpath failed");
        return E_SYS_CALL;
    }
    FILE *f = fopen(realPath, "r");
    free(realPath);
    if (f == nullptr) {
        LOGE("Failed to open %{public}s, errno: %{public}d", path.c_str(), errno);
        return E_SYS_CALL;
    }
    int fd = fileno(f);
    if (fd < 0) {
        LOGE("Failed to open %{public}s, errno: %{public}d", path.c_str(), errno);
        return E_SYS_CALL;
    }
    if (ioctl(fd, FS_IOC_FSGETXATTR, &fsx) == -1) {
        LOGE("Failed to get extended attributes of %{public}s, errno: %{public}d", path.c_str(), errno);
        (void)fclose(f);
        return E_SYS_CALL;
    }
    if (fsx.fsx_projid == static_cast<uint32_t>(prjId)) {
        (void)fclose(f);
        return E_OK;
    }
    fsx.fsx_projid = static_cast<uint32_t>(prjId);
    if (ioctl(fd, FS_IOC_FSSETXATTR, &fsx) == -1) {
        LOGE("Failed to set project id for %{public}s, errno: %{public}d", path.c_str(), errno);
        (void)fclose(f);
        return E_SYS_CALL;
    }
    if (inherit) {
        uint32_t flags;
        if (ioctl(fd, FS_IOC_GETFLAGS, &flags) == -1) {
            LOGE("Failed to get flags for %{public}s, errno:%{public}d", path.c_str(), errno);
            (void)fclose(f);
            return E_SYS_CALL;
        }
        flags |= FS_PROJINHERIT_FL;
        if (ioctl(fd, FS_IOC_SETFLAGS, &flags) == -1) {
            LOGE("Failed to set flags for %{public}s, errno:%{public}d", path.c_str(), errno);
            (void)fclose(f);
            return E_SYS_CALL;
        }
    }
    (void)fclose(f);
    return E_OK;
}

static std::tuple<std::vector<std::string>, std::vector<std::string>> ReadIncludesExcludesPath(
    const std::string &bundleName, const int64_t lastBackupTime, const uint32_t userId)
{
    if (bundleName.empty()) {
        LOGE("bundleName is empty");
        return { {}, {} };
    }
    // 保存includeExclude的path
    std::string filePath = BACKUP_PATH_PREFIX + std::to_string(userId) + BACKUP_PATH_SURFFIX +
        bundleName + FILE_SEPARATOR_CHAR + BACKUP_INCEXC_SYMBOL + std::to_string(lastBackupTime);
    std::ifstream incExcFile;
    incExcFile.open(filePath.data());
    if (!incExcFile.is_open()) {
        LOGE("Cannot open include/exclude file, fail errno:%{public}d", errno);
        return { {}, {} };
    }

    std::vector<std::string> includes;
    std::vector<std::string> excludes;
    bool incOrExt = true;
    while (incExcFile) {
        std::string line;
        std::getline(incExcFile, line);
        if (line.empty()) {
            LOGI("Read Complete");
            break;
        }
        if (line == BACKUP_INCLUDE) {
            incOrExt = true;
        } else if (line == BACKUP_EXCLUDE) {
            incOrExt = false;
        }
        if (incOrExt && line != BACKUP_INCLUDE) {
            includes.emplace_back(line);
        } else if (!incOrExt && line != BACKUP_EXCLUDE) {
            excludes.emplace_back(line);
        }
    }
    incExcFile.close();
    return {includes, excludes};
}

static bool AddPathMapForPathWildCard(uint32_t userId, const std::string &bundleName, const std::string &phyPath,
    std::map<std::string, std::string> &pathMap)
{
    std::string physicalPrefixEl1 = PHY_APP + EL1 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
        bundleName + FILE_SEPARATOR_CHAR;
    std::string physicalPrefixEl2 = PHY_APP + EL2 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
        bundleName + FILE_SEPARATOR_CHAR;
    if (phyPath.find(physicalPrefixEl1) == 0) {
        std::string relatePath = phyPath.substr(physicalPrefixEl1.size());
        pathMap.insert({phyPath, BASE_EL1 + relatePath});
    } else if (phyPath.find(physicalPrefixEl2) == 0) {
        std::string relatePath = phyPath.substr(physicalPrefixEl2.size());
        pathMap.insert({phyPath, BASE_EL2 + relatePath});
    } else {
        LOGE("Invalid phyiscal path");
        return false;
    }
    return true;
}

static bool GetPathWildCard(uint32_t userId, const std::string &bundleName, const std::string &includeWildCard,
    std::vector<std::string> &includePathList, std::map<std::string, std::string> &pathMap)
{
    size_t pos = includeWildCard.rfind(WILDCARD_DEFAULT_INCLUDE);
    if (pos == std::string::npos) {
        LOGE("GetPathWildCard: path should include *");
        return false;
    }
    std::string pathBeforeWildCard = includeWildCard.substr(0, pos);
    DIR *dirPtr = opendir(pathBeforeWildCard.c_str());
    if (dirPtr == nullptr) {
        LOGE("GetPathWildCard open file dir:%{private}s fail, errno:%{public}d", pathBeforeWildCard.c_str(), errno);
        return false;
    }
    struct dirent *entry = nullptr;
    std::vector<std::string> subDirs;
    while ((entry = readdir(dirPtr)) != nullptr) {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
            continue;
        }
        std::string path = pathBeforeWildCard + entry->d_name;
        if (entry->d_type == DT_DIR) {
            subDirs.emplace_back(path);
        }
    }
    closedir(dirPtr);
    for (auto &subDir : subDirs) {
        DIR *subDirPtr = opendir(subDir.c_str());
        if (subDirPtr == nullptr) {
            LOGE("GetPathWildCard open file dir:%{private}s fail, errno:%{public}d", subDir.c_str(), errno);
            return false;
        }
        while ((entry = readdir(subDirPtr)) != nullptr) {
            if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
                continue;
            }
            std::string dirName = std::string(entry->d_name);

            std::string path = subDir + FILE_SEPARATOR_CHAR + entry->d_name;
            if (entry->d_type == DT_DIR && (dirName == DEFAULT_INCLUDE_PATH_IN_HAP_FILES ||
                dirName == DEFAULT_INCLUDE_PATH_IN_HAP_DATABASE ||
                dirName == DEFAULT_INCLUDE_PATH_IN_HAP_PREFERENCE)) {
                includePathList.emplace_back(path);
                AddPathMapForPathWildCard(userId, bundleName, path, pathMap);
            }
        }
        closedir(subDirPtr);
    }
    return true;
}

static void RecognizeSandboxWildCard(const uint32_t userId, const std::string &bundleName,
    const std::string &sandboxPathStr, std::vector<std::string> &phyIncludes,
    std::map<std::string, std::string>& pathMap)
{
    if (sandboxPathStr.find(BASE_EL1 + DEFAULT_PATH_WITH_WILDCARD) == 0) {
        std::string physicalPrefix = PHY_APP + EL1 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
            bundleName + FILE_SEPARATOR_CHAR;
        std::string relatePath = sandboxPathStr.substr(BASE_EL1.size());
        if (!GetPathWildCard(userId, bundleName, physicalPrefix + relatePath, phyIncludes, pathMap)) {
            LOGE("el1 GetPathWildCard dir path invaild");
        }
    } else if (sandboxPathStr.find(BASE_EL2 + DEFAULT_PATH_WITH_WILDCARD) == 0) {
        std::string physicalPrefix = PHY_APP + EL2 + FILE_SEPARATOR_CHAR + std::to_string(userId) + BASE +
            bundleName + FILE_SEPARATOR_CHAR;
        std::string relatePath = sandboxPathStr.substr(BASE_EL2.size());
        if (!GetPathWildCard(userId, bundleName, physicalPrefix + relatePath, phyIncludes, pathMap)) {
            LOGE("el2 GetPathWildCard dir path invaild");
        }
    }
}

static void ConvertSandboxRealPath(const uint32_t userId, const std::string &bundleName,
    const std::string &sandboxPathStr, std::vector<std::string> &realPaths,
    std::map<std::string, std::string>& pathMap)
{
    std::string uriString;
    if (sandboxPathStr.find(NORMAL_SAND_PREFIX) == 0) {
        // for normal hap, start with file://bundleName
        uriString = URI_PREFIX + bundleName;
    } else if (sandboxPathStr.find(FILE_SAND_PREFIX) == 0) {
        // for public files, start with file://docs
        uriString = URI_PREFIX + FILE_AUTHORITY;
    } else if (sandboxPathStr.find(MEDIA_SAND_PREFIX) == 0) {
        std::string physicalPath = sandboxPathStr;
        physicalPath.insert(MEDIA_SAND_PREFIX.length(), FILE_SEPARATOR_CHAR + std::to_string(userId));
        realPaths.emplace_back(physicalPath);
        pathMap.insert({physicalPath, sandboxPathStr});
        return;
    } else if (sandboxPathStr.find(MEDIA_CLOUD_SAND_PREFIX) == 0) {
        std::string physicalPath = sandboxPathStr;
        physicalPath.insert(MEDIA_CLOUD_SAND_PREFIX.length(), FILE_SEPARATOR_CHAR + std::to_string(userId));
        realPaths.emplace_back(physicalPath);
        pathMap.insert({physicalPath, sandboxPathStr});
        return;
    }

    if (!uriString.empty()) {
        uriString += sandboxPathStr;
        AppFileService::ModuleFileUri::FileUri uri(uriString);
        // files
        std::string physicalPath;
        int ret = AppFileService::SandboxHelper::GetBackupPhysicalPath(uri.ToString(), std::to_string(userId),
            physicalPath);
        if (ret != 0) {
            LOGE("Get physical path failed with %{public}d", ret);
            return;
        }
        realPaths.emplace_back(physicalPath);
        pathMap.insert({physicalPath, sandboxPathStr});
    }
}

static void WriteFileList(std::ofstream &statFile, struct FileStat fileStat, BundleStatsParas &paras)
{
    if (!statFile.is_open() || fileStat.filePath.empty()) {
        LOGE("WriteFileList Param failed");
        return;
    }
    std::string fileLine = "";
    bool encodeFlag = false;
    if (fileStat.filePath.find(LINE_SEP) != std::string::npos) {
        fileLine += AppFileService::SandboxHelper::Encode(fileStat.filePath) + FILE_CONTENT_SEPARATOR;
        encodeFlag = true;
    } else {
        fileLine += fileStat.filePath + FILE_CONTENT_SEPARATOR;
    }
    fileLine += std::to_string(fileStat.mode) + FILE_CONTENT_SEPARATOR;
    if (fileStat.isDir) {
        fileLine += std::to_string(1) + FILE_CONTENT_SEPARATOR;
    } else {
        fileLine += std::to_string(0) + FILE_CONTENT_SEPARATOR;
    }
    fileLine += std::to_string(fileStat.fileSize) + FILE_CONTENT_SEPARATOR;
    fileLine += std::to_string(fileStat.lastUpdateTime) + FILE_CONTENT_SEPARATOR;
    fileLine += FILE_CONTENT_SEPARATOR;
    if (fileStat.isIncre) {
        fileLine += std::to_string(1);
    } else {
        fileLine += std::to_string(0);
    }
    fileLine += FILE_CONTENT_SEPARATOR;
    if (encodeFlag) {
        fileLine += std::to_string(1);
    } else {
        fileLine += std::to_string(0);
    }
    // te file line
    statFile << fileLine << std::endl;
    if (fileStat.isIncre) {
        paras.incFileSizeSum += fileStat.fileSize;
    }
    paras.fileSizeSum += fileStat.fileSize;
}

static bool ExcludeFilter(std::map<std::string, bool> &excludesMap, const std::string &path)
{
    if (path.empty()) {
        LOGE("ExcludeFilter Param failed");
        return true;
    }
    std::string formatPath = path;
    for (auto exclude = excludesMap.begin(); exclude != excludesMap.end(); exclude++) {
        if (exclude->second != true) {
            if (formatPath.compare(exclude->first) == 0) {
                return true;
            }
        } else {
            if (formatPath.compare(0, exclude->first.size(), exclude->first) == 0) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Check if path in includes is directory or not
 *
 * @param path            path in includes
 * @param paras           start time for last backup and file size sum
 * @param pathMap         map for file sandbox path and physical path
 * @param statFile        target file stream pointer
 * @param excludeMap      map for exclude physical path and isDir
 *
 * @return std::tuple<bool, bool> : is success or not for system call / is directory or not
 */
static std::tuple<bool, bool> CheckIfDirForIncludes(const std::string &path, BundleStatsParas &paras,
    std::map<std::string, std::string> &pathMap, std::ofstream &statFile, std::map<std::string, bool> &excludesMap)
{
    if (!statFile.is_open() || path.empty()) {
        LOGE("CheckIfDirForIncludes Param failed");
        return {false, false};
    }
    // check whether the path exists
    struct stat fileStatInfo = {0};
    if (stat(path.c_str(), &fileStatInfo) != 0) {
        LOGE("CheckIfDirForIncludes call stat error %{private}s, fail errno:%{public}d", path.c_str(), errno);
        return {false, false};
    }
    if (S_ISDIR(fileStatInfo.st_mode)) {
        LOGI("%{private}s exists and is a directory", path.c_str());
        return {true, true};
    } else {
        std::string sandboxPath = path;
        auto it = pathMap.find(path);
        if (it != pathMap.end()) {
            sandboxPath = it->second;
        }

        struct FileStat fileStat;
        fileStat.filePath = sandboxPath;
        fileStat.fileSize = fileStatInfo.st_size;
        // mode
        fileStat.mode = static_cast<int32_t>(fileStatInfo.st_mode);
        fileStat.isDir = false;
        int64_t lastUpdateTime = static_cast<int64_t>(fileStatInfo.st_mtime);
        fileStat.lastUpdateTime = lastUpdateTime;
        if (paras.lastBackupTime == 0 || lastUpdateTime > paras.lastBackupTime) {
            fileStat.isIncre = true;
        }
        if (ExcludeFilter(excludesMap, path) == false) {
            WriteFileList(statFile, fileStat, paras);
        }
        return {true, false};
    }
}

static std::string PhysicalToSandboxPath(const std::string &dir, const std::string &sandboxDir,
    const std::string &path)
{
    std::size_t dirPos = dir.size();
    std::string pathSurffix = path.substr(dirPos);
    return sandboxDir + pathSurffix;
}

static bool AddOuterDirIntoFileStat(const std::string &dir, BundleStatsParas &paras, const std::string &sandboxDir,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap)
{
    if (!statFile.is_open() || dir.empty()) {
        LOGE("AddOuterDirIntoFileStat Param failed");
        return false;
    }
    struct stat fileInfo = {0};
    if (stat(dir.c_str(), &fileInfo) != 0) {
        LOGE("AddOuterDirIntoFileStat call stat error %{private}s, fail errno:%{public}d", dir.c_str(), errno);
        return false;
    }
    struct FileStat fileStat = {};
    fileStat.filePath = PhysicalToSandboxPath(dir, sandboxDir, dir);
    fileStat.fileSize = fileInfo.st_size;
    // mode
    fileStat.mode = static_cast<int32_t>(fileInfo.st_mode);
    int64_t lastUpdateTime = static_cast<int64_t>(fileInfo.st_mtime);
    fileStat.lastUpdateTime = lastUpdateTime;
    fileStat.isIncre = (paras.lastBackupTime == 0 || lastUpdateTime > paras.lastBackupTime) ? true : false;
    fileStat.isDir = true;
    std::string formatPath = dir;
    if (formatPath.back() != FILE_SEPARATOR_CHAR) {
        formatPath.push_back(FILE_SEPARATOR_CHAR);
    }
    if (ExcludeFilter(excludesMap, formatPath) == false) {
        WriteFileList(statFile, fileStat, paras);
    }
    return true;
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

static void InsertStatFile(const std::string &path, struct FileStat fileStat,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap, BundleStatsParas &paras)
{
    if (!statFile.is_open() || path.empty()) {
        LOGE("InsertStatFile Param failed");
        return;
    }
    std::string formatPath = path;
    if (fileStat.isDir == true && formatPath.back() != FILE_SEPARATOR_CHAR) {
        formatPath.push_back(FILE_SEPARATOR_CHAR);
    }
    if (!ExcludeFilter(excludesMap, formatPath)) {
        WriteFileList(statFile, fileStat, paras);
    }
}

static bool GetIncludesFileStats(const std::string &dir, BundleStatsParas &paras,
    std::map<std::string, std::string> &pathMap,
    std::ofstream &statFile, std::map<std::string, bool> &excludesMap)
{
    std::string sandboxDir = dir;
    auto it = pathMap.find(dir);
    if (it != pathMap.end()) {
        sandboxDir = it->second;
    }
    // stat current directory info
    AddOuterDirIntoFileStat(dir, paras, sandboxDir, statFile, excludesMap);

    std::stack<std::string> folderStack;
    std::string filePath;
    folderStack.push(dir);
    // stat files and sub-directory in current directory info
    while (!folderStack.empty()) {
        filePath = folderStack.top();
        folderStack.pop();
        DIR *dirPtr = opendir(filePath.c_str());
        if (dirPtr == nullptr) {
            LOGE("GetIncludesFileStats open file dir:%{private}s fail, errno:%{public}d", filePath.c_str(), errno);
            continue;
        }
        if (filePath.back() != FILE_SEPARATOR_CHAR) {
            filePath.push_back(FILE_SEPARATOR_CHAR);
        }

        struct dirent *entry = nullptr;
        while ((entry = readdir(dirPtr)) != nullptr) {
            if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
                continue;
            }
            std::string path = filePath + entry->d_name;
            struct stat fileInfo = {0};
            if (stat(path.c_str(), &fileInfo) != 0) {
                LOGE("GetIncludesFileStats call stat error %{private}s, errno:%{public}d", path.c_str(), errno);
                fileInfo.st_size = 0;
            }
            struct FileStat fileStat = {};
            fileStat.filePath = PhysicalToSandboxPath(dir, sandboxDir, path);
            fileStat.fileSize = fileInfo.st_size;
            CheckOverLongPath(fileStat.filePath);
            // mode
            fileStat.mode = static_cast<int32_t>(fileInfo.st_mode);
            int64_t lastUpdateTime = static_cast<int64_t>(fileInfo.st_mtime);
            fileStat.lastUpdateTime = lastUpdateTime;
            fileStat.isIncre = (paras.lastBackupTime == 0 || lastUpdateTime > paras.lastBackupTime) ? true : false;
            if (entry->d_type == DT_DIR) {
                fileStat.isDir = true;
                folderStack.push(path);
            }
            InsertStatFile(path, fileStat, statFile, excludesMap, paras);
        }
        closedir(dirPtr);
    }
    return true;
}

static void SetExcludePathMap(std::string &excludePath, std::map<std::string, bool> &excludesMap)
{
    if (excludePath.empty()) {
        LOGE("SetExcludePathMap Param failed");
        return;
    }
    struct stat fileStatInfo = {0};
    if (stat(excludePath.c_str(), &fileStatInfo) != 0) {
        LOGE("SetExcludePathMap call stat error %{private}s, errno:%{public}d", excludePath.c_str(), errno);
        return;
    }
    if (S_ISDIR(fileStatInfo.st_mode)) {
        if (excludePath.back() != FILE_SEPARATOR_CHAR) {
            excludePath.push_back(FILE_SEPARATOR_CHAR);
        }
        excludesMap.insert({excludePath, true});
    } else {
        excludesMap.insert({excludePath, false});
    }
}

static void ScanExtensionPath(BundleStatsParas &paras,
    const std::vector<std::string> &includes, const std::vector<std::string> &excludes,
    std::map<std::string, std::string> &pathMap, std::ofstream &statFile)
{
    std::map<std::string, bool> excludesMap;
    for (auto exclude : excludes) {
        SetExcludePathMap(exclude, excludesMap);
    }
    // all file with stats in include directory
    for (const auto &includeDir : includes) {
        // Check if includeDir is a file path
        auto [isSucc, isDir] = CheckIfDirForIncludes(includeDir, paras, pathMap, statFile, excludesMap);
        if (!isSucc) {
            continue;
        }
        // recognize all file in include directory
        if (isDir && !GetIncludesFileStats(includeDir, paras, pathMap, statFile, excludesMap)) {
            LOGE("Faied to get include files for includeDir");
        }
    }
}

static void DealWithIncludeFiles(const BundleStatsParas &paras, const std::vector<std::string> &includes,
    std::vector<std::string> &phyIncludes, std::map<std::string, std::string>& pathMap)
{
    uint32_t userId = paras.userId;
    std::string bundleName = paras.bundleName;
    for (const auto &include : includes) {
        std::string includeStr = include;
        if (includeStr.front() != FILE_SEPARATOR_CHAR) {
            includeStr = FILE_SEPARATOR_CHAR + includeStr;
        }
        if (includeStr.find(BASE_EL1 + DEFAULT_PATH_WITH_WILDCARD) == 0 ||
            includeStr.find(BASE_EL2 + DEFAULT_PATH_WITH_WILDCARD) == 0) {
            // recognize sandbox path to physical path with wild card
            RecognizeSandboxWildCard(userId, bundleName, includeStr, phyIncludes, pathMap);
            if (phyIncludes.empty()) {
                LOGE("DealWithIncludeFiles failed to recognize path with wildcard %{private}s", bundleName.c_str());
                continue;
            }
        } else {
            // convert sandbox to physical path
            ConvertSandboxRealPath(userId, bundleName, includeStr, phyIncludes, pathMap);
        }
    }
}

static inline bool PathSortFunc(const std::string &path1, const std::string &path2)
{
    return path1 < path2;
}

static void DeduplicationPath(std::vector<std::string> &configPaths)
{
    sort(configPaths.begin(), configPaths.end(), PathSortFunc);
    auto it = unique(configPaths.begin(), configPaths.end(), [](const std::string &path1, const std::string &path2) {
        return path1 == path2;
    });
    configPaths.erase(it, configPaths.end());
}

static void GetBundleStatsForIncreaseEach(uint32_t userId, std::string &bundleName, int64_t lastBackupTime,
    std::vector<int64_t> &pkgFileSizes, std::vector<int64_t> &incPkgFileSizes)
{
    // input parameters
    BundleStatsParas paras = {.userId = userId, .bundleName = bundleName,
                              .lastBackupTime = lastBackupTime, .fileSizeSum = 0, .incFileSizeSum = 0};

    // obtain includes, excludes in backup extension config
    auto [includes, excludes] = ReadIncludesExcludesPath(bundleName, lastBackupTime, userId);
    if (includes.empty()) {
        pkgFileSizes.emplace_back(0);
        incPkgFileSizes.emplace_back(0);
        return;
    }
    // physical paths
    std::vector<std::string> phyIncludes;
    // map about sandbox path to physical path
    std::map<std::string, std::string> pathMap;

    // recognize physical path for include directory
    DealWithIncludeFiles(paras, includes, phyIncludes, pathMap);
    if (phyIncludes.empty()) {
        LOGE("Incorrect convert for include sandbox path for %{private}s", bundleName.c_str());
        pkgFileSizes.emplace_back(0);
        incPkgFileSizes.emplace_back(0);
        return;
    }

    // recognize physical path for exclude directory
    std::vector<std::string> phyExcludes;
    for (const auto &exclude : excludes) {
        std::string excludeStr = exclude;
        if (excludeStr.front() != FILE_SEPARATOR_CHAR) {
            excludeStr = FILE_SEPARATOR_CHAR + excludeStr;
        }
        // convert sandbox to physical path
        ConvertSandboxRealPath(userId, bundleName, excludeStr, phyExcludes, pathMap);
    }

    std::string filePath = BACKUP_PATH_PREFIX + std::to_string(userId) + BACKUP_PATH_SURFFIX +
        bundleName + FILE_SEPARATOR_CHAR + BACKUP_STAT_SYMBOL + std::to_string(lastBackupTime);
    std::ofstream statFile;
    statFile.open(filePath.data(), std::ios::out | std::ios::trunc);
    if (!statFile.is_open()) {
        LOGE("creat file fail, errno:%{public}d.", errno);
        pkgFileSizes.emplace_back(0);
        incPkgFileSizes.emplace_back(0);
        return;
    }
    statFile << VER_10_LINE1 << std::endl;
    statFile << VER_10_LINE2 << std::endl;

    DeduplicationPath(phyIncludes);
    ScanExtensionPath(paras, phyIncludes, phyExcludes, pathMap, statFile);
    // calculate summary file sizes
    pkgFileSizes.emplace_back(paras.fileSizeSum);
    incPkgFileSizes.emplace_back(paras.incFileSizeSum);
    LOGI("bundleName: %{public}s, size: %{public}lld", bundleName.c_str(), static_cast<long long>(paras.fileSizeSum));
    statFile.close();
}

int32_t QuotaManager::GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
    const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
    std::vector<int64_t> &incPkgFileSizes)
{
    LOGI("GetBundleStatsForIncrease start");
    if (bundleNames.size() != incrementalBackTimes.size()) {
        LOGE("Invalid paramters, size of bundleNames should match incrementalBackTimes.");
        return E_SYS_ERR;
    }

    for (size_t i = 0; i < bundleNames.size(); i++) {
        std::string bundleName = bundleNames[i];
        int64_t lastBackupTime = incrementalBackTimes[i];
        GetBundleStatsForIncreaseEach(userId, bundleName, lastBackupTime, pkgFileSizes, incPkgFileSizes);
    }
    return E_OK;
}
} // namespace STORAGE_DAEMON
} // namespace OHOS
