/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_STORAGE_DAEMON_MOUNT_MANAGER_H
#define OHOS_STORAGE_DAEMON_MOUNT_MANAGER_H

#include <fstream>
#include <list>
#include <map>
#include <nocopyable.h>
#include <set>

#include "utils/file_utils.h"

namespace OHOS {
namespace StorageDaemon {
const std::string SANDBOX_ROOT_PATH = "/mnt/sandbox/";
const std::string CURRENT_USER_ID_FLAG = "<currentUserId>";
const std::string PACKAGE_NAME_FLAG = "<bundleName>";
const std::string MOUNT_POINT_INFO = "/proc/mounts";
const std::string MOUNT_POINT_TYPE_HMDFS = "hmdfs";
const std::string MOUNT_POINT_TYPE_HMFS = "hmfs";
const std::string MOUNT_POINT_TYPE_F2FS = "f2fs";
const std::string MOUNT_POINT_TYPE_SHAREFS = "sharefs";
const std::string EL2_BASE = "/data/storage/el2/base/";
const std::string MOUNT_SUFFIX = "_locked";
const std::string OABINDER_CTRL_STATE = "storage_oa_binder.ctl.state";
const std::string RGM_CONFIG_PATH = "/data/service/el1/public/rgm_manager/data/rgm_hmos/config";
const std::string RGM_STORAGE_FILE_PATH = RGM_CONFIG_PATH + "/storage/direnc.json";
const std::string APP_EL1_PATH = "/data/app/el1";
const std::string FILE_MGR_ROOT_PATH = "/storage/Users/currentUser/";
const std::set<std::string> SANDBOX_EXCLUDE_PATH = {
    "chipset",
    "system",
    "com.ohos.render"
};
const std::vector<std::string> CRYPTO_SANDBOX_PATH = {
    "/data/storage/el2/base/",
    "/data/storage/el2/database/",
    "/data/storage/el2/share/",
    "/data/storage/el2/log/",
    "/data/storage/el2/distributedfiles/",
    "/data/storage/el2/cloud/",
    "/data/storage/el3/base/",
    "/data/storage/el3/database/",
    "/data/storage/el4/base/",
    "/data/storage/el4/database/",
    "/data/storage/el5/base/",
    "/data/storage/el5/database/"
};
const std::vector<std::string> CRYPTO_SRC_PATH = {
    "/data/app/el2/<currentUserId>/base/<bundleName>/",
    "/data/app/el2/<currentUserId>/database/<bundleName>/",
    "/mnt/share/<currentUserId>/<bundleName>/",
    "/data/app/el2/<currentUserId>/log/<bundleName>/",
    "/mnt/hmdfs/<currentUserId>/account/merge_view/data/<bundleName>/",
    "/mnt/hmdfs/<currentUserId>/cloud/data/<bundleName>/",
    "/data/app/el3/<currentUserId>/base/<bundleName>/",
    "/data/app/el3/<currentUserId>/database/<bundleName>/",
    "/data/app/el4/<currentUserId>/base/<bundleName>/",
    "/data/app/el4/<currentUserId>/database/<bundleName>/",
    "/data/app/el5/<currentUserId>/base/<bundleName>/",
    "/data/app/el5/<currentUserId>/database/<bundleName>/"
};

const std::vector<std::string> APPDATA_DST_PATH = {
    "/mnt/user/<currentUserId>/nosharefs/appdata/el1/base/",
    "/mnt/user/<currentUserId>/nosharefs/appdata/el2/base/",
    "/mnt/user/<currentUserId>/nosharefs/appdata/el2/cloud/",
    "/mnt/user/<currentUserId>/nosharefs/appdata/el2/distributedfiles/",
    "/mnt/user/<currentUserId>/nosharefs/appdata/el5/base/"
};

const std::vector<std::string> APPDATA_SRC_PATH = {
    "/data/app/el1/<currentUserId>/base/",
    "/data/app/el2/<currentUserId>/base/",
    "/mnt/hmdfs/<currentUserId>/cloud/data/",
    "/mnt/hmdfs/<currentUserId>/account/merge_view/data/",
    "/data/app/el5/<currentUserId>/base/"
};

const std::vector<std::string> FD_PATH = {
    "/data/service/el2/<currentUserId>",
    "/data/service/el3/<currentUserId>",
    "/data/service/el4/<currentUserId>",
    "/data/service/el5/<currentUserId>",
    "/storage/media/<currentUserId>"
};

const std::vector<std::string> SYS_PATH = {
    "/mnt/hmdfs/<currentUserId>/account",
    "/mnt/hmdfs/<currentUserId>/non_account",
    "/mnt/hmdfs/<currentUserId>/cloud"
};

const std::vector<std::string> HMDFS_SUFFIX = {"account", "non_account", "cloud"};

const std::string HMDFS_SYS_CAP = "const.distributed_file_property.enabled";
const int32_t HMDFS_VAL_LEN = 6;
const int32_t HMDFS_TRUE_LEN = 5;
const std::string SHARE_PATH = "/data/service/el1/public/storage_daemon/share/public";
static constexpr int MODE_0711 = 0711;
static constexpr int MODE_0771 = 0771;
static constexpr int MODE_02771 = 02771;
const char DETERMINE_DEVICE_TYPE_KEY[] = "persist.distributed_scene.sys_settings_data_sync";
const int32_t REMOUNT_VALUE_LEN = 10;

struct DirInfo {
    const std::string path;
    mode_t mode;
    uid_t uid;
    gid_t gid;
};

constexpr uid_t OID_ROOT = 0;
constexpr uid_t OID_SYSTEM = 1000;
constexpr uid_t OID_FILE_MANAGER = 1006;
constexpr uid_t OID_USER_DATA_RW = 1008;
constexpr uid_t OID_DFS = 1009;
constexpr uid_t OID_MEDIA = 1013;
constexpr uid_t OID_BACKUP = 1089;
constexpr uid_t OID_DFS_SHARE = 3822;
constexpr uid_t OID_TEE = 6668;
constexpr uid_t OID_DEVICE_AUTH = 3333;
constexpr uid_t OID_HUKS = 3510;
constexpr uid_t OID_ASSET = 6226;
constexpr uid_t OID_DDMS = 3012;
constexpr uid_t OID_HWID = 7008;
constexpr uid_t OID_HEALTH_SPORT = 7259;
constexpr uid_t OID_DLP_CREDENTIAL = 3553;
constexpr uid_t OID_RSS = 1096;
constexpr uid_t OID_HIVIEW = 1201;
constexpr uid_t OID_PARENT_CONTROL = 7007;
constexpr uid_t OID_ACCOUNT = 3058;
constexpr uid_t OID_COLLABORATION_FWK = 5520;
constexpr uid_t OID_CLOUD_BACK = 5206;
constexpr uid_t OID_AV_SESSION = 6700;
constexpr uid_t USER_ID_BASE = 200000;
constexpr uid_t OID_CLOUD_DEVELOP_PROXY = 7996;
constexpr uid_t OID_MEDIA_ENHANCE_SERVICE = 7998;
constexpr uid_t OID_PUSH = 7023;
constexpr uid_t OID_GAMESERVICE_SERVER = 7011;
constexpr uid_t OID_HWF_SERVICE = 7700;
constexpr uid_t OID_FOUNDATION = 5523;
constexpr uid_t OID_PASTEBOARD = 3816;
constexpr uid_t OID_PRINT = 3823;
constexpr uid_t OID_FINDNETWORK = 7518;
// Proprietary service, not for open-source use.
constexpr uid_t OID_TRUSTED_RING = 65936;

class MountManager final {
public:
    MountManager();
    virtual ~MountManager() = default;
    static std::shared_ptr<MountManager> GetInstance();
    static std::vector<DirInfo> InitHmdfsDirVec();
    static std::vector<DirInfo> InitVirtualDir();
    static std::vector<DirInfo> InitSystemServiceDir();
    static std::vector<DirInfo> InitFileManagerDir();
    static std::vector<DirInfo> InitAppdataDir();
    int32_t MountByUser(int32_t userId);
    int32_t UmountByUser(int32_t userId);
    int32_t PrepareHmdfsDirs(int32_t userId);
    int32_t PrepareFileManagerDirs(int32_t userId);
    int32_t PrepareAppdataDir(int32_t userId);
    int32_t DestroyHmdfsDirs(int32_t userId);
    int32_t DestroyFileManagerDirs(int32_t userId);
    int32_t DestroySystemServiceDirs(int32_t userId);
    int32_t CloudMount(int32_t userId, const std::string &path);
    int32_t CloudTwiceMount(int32_t userId);
    int32_t MountCryptoPathAgain(uint32_t userId);
    int32_t MountDfsDocs(int32_t userId, const std::string &relativePath,
        const std::string &networkId, const std::string &deviceId);
    int32_t UMountDfsDocs(int32_t userId, const std::string &relativePath,
        const std::string &networkId, const std::string &deviceId);
    int32_t UMountAllPath(int32_t userId, std::list<std::string> &unMountFailList);
    int32_t UMountByList(std::list<std::string> &list, std::list<std::string> &unMountFailList);
    int32_t UMountByListWithDetach(std::list<std::string> &list);
    void SetCloudState(bool active);
    void SetMediaObserverState(bool active);
    int32_t RestoreconSystemServiceDirs(int32_t userId);
    int32_t FindMountPointsToMap(std::map<std::string, std::list<std::string>> &mountMap, int32_t userId);
    void MountPointToList(std::list<std::string> &hmdfsList, std::list<std::string> &hmfsList,
        std::list<std::string> &sharefsList, std::string &line, int32_t userId);
    bool CheckMaps(const std::string &path, std::list<std::string> &mountFailList);
    bool CheckSymlink(const std::string &path, std::list<std::string> &mountFailList);
    bool GetProcessInfo(const std::string &filename, ProcessInfo &info);
    bool PidUsingFlag(std::string &pidPath, std::list<std::string> &mountFailList);
    void MountSandboxPath(const std::vector<std::string> &srcPaths, const std::vector<std::string> &dstPaths,
                          const std::string &bundleName, const std::string &userId);
    bool CheckMountFileByUser(int32_t userId);
    bool CloudAndFuseDirFlag(const std::string &path);
    bool MediaFuseDirFlag(const std::string &path);
    int32_t MountMediaFuse(int32_t userId, int32_t &devFd);
    int32_t UMountMediaFuse(int32_t userId);
    int32_t FindAndKillProcess(int32_t userId, std::list<std::string> &unMountFailList, int32_t radar);
    int32_t FindAndKillProcessWithoutRadar(int32_t userId, std::list<std::string> &killList);
    int32_t CheckProcessUserId(int32_t userId, std::vector<ProcessInfo> &proInfos,
                               std::vector<ProcessInfo> &processKillInfos);
    int32_t MountFileMgrFuse(int32_t userId, const std::string &path, int32_t &fuseFd);
    int32_t UMountFileMgrFuse(int32_t userId, const std::string &path);
    int32_t IsFileOccupied(const std::string &path, const std::vector<std::string> &inputList,
        std::vector<std::string> &outputList, bool &isOccupy);
    int32_t MountDisShareFile(int32_t userId, const std::map<std::string, std::string> &shareFiles);
    int32_t UMountDisShareFile(int32_t userId, const std::string &networkId);

private:
    bool SupportHmdfs();
    int32_t CreateVirtualDirs(int32_t userId);
    int32_t HmdfsMount(int32_t userId, std::string relativePath, bool mountCloudDisk = false);
    int32_t HmdfsTwiceMount(int32_t userId, const std::string &relativePath);
    int32_t SharefsMount(int32_t userId);
    int32_t HmSharefsMount(int32_t userId, std::string &srcPath, std::string &dstPath);
    int32_t LocalMount(int32_t userId);
    int32_t LocalUMount(int32_t userId);
    int32_t SetFafQuotaProId(int32_t userId);
    int32_t CreateSystemServiceDirs(int32_t userId);
    void MountCloudForUsers(void);
    void UMountCloudForUsers(void);
    void PrepareFileManagerDir(int32_t userId);
    int32_t CloudUMount(int32_t userId);
    bool CheckPathValid(const std::string &bundleNameStr, uint32_t userId);
    int32_t MountAppdataAndSharefs(int32_t userId);
    int32_t MountAppdata(const std::string &userId);
    bool DirExist(const std::string &dir);
    void GetAllUserId(std::vector<int32_t> &userIds);
    int32_t PrepareAppdataDirByUserId(int32_t userId);
    int32_t MountSharefsAndNoSharefs(int32_t userId);
    int32_t SharedMount(int32_t userId, const std::string &path);
    int32_t BindAndRecMount(int32_t userId, std::string &srcPath, std::string &dstPath, bool isUseSlave = true);
    int32_t UmountMntUserTmpfs(int32_t userId);
    int32_t UmountFileSystem(int32_t userId);
    int32_t MountFileSystem(int32_t userId);
    int32_t FindProcess(std::list<std::string> &unMountFailList, std::vector<ProcessInfo> &proInfos,
        std::list<std::string> &excludeProcess);
    int32_t FindSaFd(int32_t userId);
    int32_t BindMount(std::string &srcPath, std::string &dstPath);
    int32_t UMountHmdfsByList(int32_t userId, std::list<std::string> &list, std::list<std::string> &unMountFailList);
    bool IsSysMountPoint(int32_t userId, std::string &path);
    bool CheckSysFs(int32_t userId);
    bool IsSysFsInUse(std::string &path);
    void ForbidOpen(int32_t userId);
    int32_t OpenProcForPath(const std::string &path, bool &isOccupy, bool isDir);
    int32_t OpenProcForMulti(const std::string &path, std::set<std::string> &occupyFiles);
    bool FindProcForPath(const std::string &pidPath, const std::string &path, bool isDir);
    void FindProcForMulti(const std::string &pidPath, const std::string &path, std::set<std::string> &occupyFiles);
    bool CheckSymlinkForPath(const std::string &fdPath, const std::string &path, bool isDir);
    void CheckSymlinkForMulti(const std::string &fdPath, const std::string &path, std::set<std::string> &occupyFiles);
    bool IsReadOnlyRemount();
    int32_t FindMountsByNetworkId(const std::string &networkId, std::list<std::string> &mounts);
    int32_t FilterNotMountedPath(std::map<std::string, std::string> &notMountPaths);

    DISALLOW_COPY_AND_MOVE(MountManager);

    static std::shared_ptr<MountManager> instance_;
    const std::vector<DirInfo> hmdfsDirVec_;
    const std::vector<DirInfo> virtualDir_;
    const std::vector<DirInfo> systemServiceDir_;
    const std::vector<DirInfo> fileManagerDir_;
    const std::vector<DirInfo> appdataDir_;
    std::mutex mountMutex_;
    std::vector<int32_t> fuseToMountUsers_;
    std::vector<int32_t> fuseMountedUsers_;
    bool cloudReady_{false};
};
} // STORAGE_DAEMON
} // OHOS

#endif // OHOS_STORAGE_DAEMON_USER_MANAGER_H
