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

#include "utils/mount_argument_utils.h"

#include <sstream>
#include <sys/stat.h>

#include "storage_service_log.h"

namespace OHOS {
namespace StorageDaemon {
namespace Utils {
using namespace std;
namespace {
    constexpr const char *DATA_POINT = "/data/service/el2/";
    constexpr const char *BASE_MOUNT_POINT = "/mnt/hmdfs/";
    constexpr const char *SYSFS_HMDFS_PATH = "/sys/fs/hmdfs/";
    constexpr const char *SHAREFS_DATA_POINT = "/data/service/el2/";
    constexpr const char *SHAREFS_BASE_MOUNT_POINT = "/mnt/share/";
    constexpr const char *TMPFS_MNT_DATA = "/mnt/data/";
    constexpr const char *SANDBOX_PATH = "/mnt/sandbox/";
    constexpr const char *MNT_USER_PATH = "/mnt/user/";
    constexpr const char *SHAREFS_DOC_CUR_PATH = "/sharefs/docs/currentUser";
    constexpr const char *CUR_OTHER_APPDATA_PATH = "/currentUser/other/appdata";
} // namespace

string MountArgument::GetFullDst() const
{
    LOGD("[L8:MountArgumentUtils] GetFullDst: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << BASE_MOUNT_POINT << userId_ << "/" << relativePath_;

    return ss.str();
}

string MountArgument::GetFullMediaCloud() const
{
    LOGD("[L8:MountArgumentUtils] GetFullMediaCloud: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << TMPFS_MNT_DATA << userId_ << "/" << "cloud";

    return ss.str();
}

string MountArgument::GetFullCloud() const
{
    LOGD("[L8:MountArgumentUtils] GetFullCloud: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << TMPFS_MNT_DATA << userId_ << "/" << "cloud_fuse";

    return ss.str();
}

string MountArgument::GetShareSrc() const
{
    LOGD("[L8:MountArgumentUtils] GetShareSrc: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << SHAREFS_DATA_POINT << userId_ << "/share";

    return ss.str();
}




string MountArgument::GetShareDst() const
{
    LOGD("[L8:MountArgumentUtils] GetShareDst: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << SHAREFS_BASE_MOUNT_POINT << userId_;

    return ss.str();
}









string MountArgument::GetCachePath() const
{
    LOGD("[L8:MountArgumentUtils] GetCachePath: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    if (enableCloudDisk_) {
        ss << DATA_POINT << userId_ << "/hmdfs/cloud/";
    } else {
        ss << DATA_POINT << userId_ << "/hmdfs/cache/" << relativePath_ << "_cache/";
    }
    return ss.str();
}

static uint64_t MocklispHash(const string &str)
{
    struct stat statBuf;
    auto err = stat(str.c_str(), &statBuf);
    if (err != 0) {
        LOGE("[L8:MountArgumentUtils] MocklispHash: stat failed, err=%{public}d", err);
    }
    LOGD("[L8:MountArgumentUtils] MocklispHash: statBuf dev id=%{public}lu",
         static_cast<unsigned long>(statBuf.st_dev));
    return statBuf.st_dev;
}

string MountArgument::GetCtrlPath() const
{
    LOGD("[L8:MountArgumentUtils] GetCtrlPath: >>> ENTER <<< userId=%{public}d", userId_);
    auto dst = GetFullDst();
    auto res = MocklispHash(dst);

    stringstream ss;
    ss << SYSFS_HMDFS_PATH << res << "/cmd";
    return ss.str();
}

string MountArgument::GetMountPointPrefix() const
{
    LOGD("[L8:MountArgumentUtils] GetMountPointPrefix: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << DATA_POINT << userId_ << "/hmdfs";
    return ss.str();
}

std::string MountArgument::GetSandboxPath() const
{
    LOGD("[L8:MountArgumentUtils] GetSandboxPath: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << SANDBOX_PATH << userId_;
    return ss.str();
}







string MountArgument::GetSharefsDocCurPath() const
{
    LOGD("[L8:MountArgumentUtils] GetSharefsDocCurPath: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << MNT_USER_PATH << userId_ << SHAREFS_DOC_CUR_PATH;
    return ss.str();
}

string MountArgument::GetCurOtherAppdataPath() const
{
    LOGD("[L8:MountArgumentUtils] GetCurOtherAppdataPath: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << MNT_USER_PATH << userId_ << CUR_OTHER_APPDATA_PATH;
    return ss.str();
}

string MountArgument::GetMntUserPath() const
{
    LOGD("[L8:MountArgumentUtils] GetMntUserPath: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << MNT_USER_PATH << userId_;
    return ss.str();
}





#ifdef STORAGE_SERVICE_MEDIA_FUSE
string MountArgument::GetFullMediaFuse() const
{
    LOGD("[L8:MountArgumentUtils] GetFullMediaFuse: >>> ENTER <<< userId=%{public}d", userId_);
    stringstream ss;
    ss << TMPFS_MNT_DATA << userId_ << "/" << "media_fuse/Photo";
    return ss.str();
}
#endif

MountArgument MountArgumentDescriptors::Alpha(int userId, string relativePath)
{
    LOGD("[L8:MountArgumentUtils] Alpha: >>> ENTER <<< userId=%{public}d", userId);
    MountArgument mountArgument = {
        .userId_ = userId,
        .needInitDir_ = true,
        .useCache_ = true,
        .useCloudDir_ = true,
        .enableMergeView_ = true,
        .enableCloudDisk_ = false,
        .enableFixupOwnerShip_ = false,
        .enableOfflineStash_ = true,
        .isSecurityMode_ = false,
        .relativePath_ = relativePath,
    };
    return mountArgument;
}
} // namespace Utils
} // namespace StorageDaemon
} // namespace OHOS

