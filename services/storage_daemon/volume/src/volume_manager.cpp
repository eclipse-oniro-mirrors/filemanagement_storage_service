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

#include "volume/volume_manager.h"

#include <fcntl.h>
#include <sys/mount.h>
#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ipc/storage_manager_client.h"
#include "mtp/mtp_device_monitor.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "utils/storage_radar.h"
#include "utils/string_utils.h"
#include "utils/disk_utils.h"
#include "utils/file_utils.h"
#include "volume/external_volume_info.h"

#define STORAGE_MANAGER_IOC_CHK_BUSY _IOR(0xAC, 77, int)
using namespace std;
using namespace OHOS::StorageService;
namespace OHOS {
namespace StorageDaemon {
VolumeManager* VolumeManager::instance_ = nullptr;

VolumeManager* VolumeManager::Instance()
{
    if (instance_ == nullptr) {
        instance_ = new VolumeManager();
    }

    return instance_;
}

std::shared_ptr<VolumeInfo> VolumeManager::GetVolume(const std::string volId)
{
    auto it = volumes_.Find(volId);
    if (it == volumes_.End()) {
        return nullptr;
    }
    return it->second;
}

std::string VolumeManager::CreateVolume(const std::string diskId, dev_t device, bool isUserdata)
{
    std::string volId = StringPrintf("vol-%u-%u", major(device), minor(device));

    LOGI("create volume %{public}s.", volId.c_str());

    std::shared_ptr<VolumeInfo> tmp = GetVolume(volId);
    if (tmp != nullptr) {
        LOGE("volume %{public}s exist.", volId.c_str());
        return "";
    }

    auto info = std::make_shared<ExternalVolumeInfo>();
    int32_t ret = info->Create(volId, diskId, device, isUserdata);
    if (ret) {
        return "";
    }

    volumes_.Insert(volId, info);

    StorageManagerClient client;
    ret = client.NotifyVolumeCreated(info);
    if (ret != E_OK) {
        LOGE("Volume Notify Created failed");
    }

    return volId;
}

int32_t VolumeManager::DestroyVolume(const std::string volId)
{
    LOGI("destroy volume %{public}s.", volId.c_str());

    std::shared_ptr<VolumeInfo> destroyNode = GetVolume(volId);

    if (destroyNode == nullptr) {
        LOGE("the volume %{public}s does not exist", volId.c_str());
        return E_NON_EXIST;
    }

    int32_t ret = destroyNode->Destroy();
    if (ret) {
        return ret;
    }
    if (IsFuse()) {
        ret = destroyNode->DestroyUsbFuse();
        if (ret) {
            return ret;
        }
    }

    volumes_.Erase(volId);
    destroyNode.reset();

    return E_OK;
}

int32_t VolumeManager::Check(const std::string volId)
{
    std::shared_ptr<VolumeInfo> info = GetVolume(volId);
    if (info == nullptr) {
        LOGE("the volume %{public}s does not exist.", volId.c_str());
        return E_NON_EXIST;
    }

    int32_t err = info->Check();
    if (err != E_OK) {
        LOGE("the volume %{public}s check failed.", volId.c_str());
        return err;
    }
    return E_OK;
}

int32_t VolumeManager::Mount(const std::string volId, uint32_t flags)
{
    std::shared_ptr<VolumeInfo> info = GetVolume(volId);
    if (info == nullptr) {
#ifdef SUPPORT_OPEN_SOURCE_MTP_DEVICE
        return DelayedSingleton<OHOS::StorageDaemon::MtpDeviceMonitor>::GetInstance()->Mount(volId);
#else
        LOGE("the volume %{public}s does not exist.", volId.c_str());
        return E_NON_EXIST;
#endif
    }
    int32_t checkRet = info->DoTryToCheck();
    if (checkRet == E_VOL_NEED_FIX) {
        LOGE("external volume maybe damage");
        StorageManagerClient client;
        checkRet = client.NotifyVolumeDamaged(info);
    }

    int32_t err = info->Mount(flags);
    if (err != E_OK) {
        LOGE("the volume %{public}s mount failed.", volId.c_str());
        return err;
    }

    StorageManagerClient client;
    err = client.NotifyVolumeMounted(info);
    if (err) {
        LOGE("Volume Notify Mount Destroyed failed");
    }
    return E_OK;
}

int32_t VolumeManager::MountUsbFuse(std::string volumeId, std::string &fsUuid, int &fuseFd)
{
    std::shared_ptr<VolumeInfo> info = GetVolume(volumeId);
    if (info == nullptr) {
        LOGE("the volume %{public}s does not exist.", volumeId.c_str());
        return E_NON_EXIST;
    }
    int32_t result = ReadVolumUuid(volumeId, fsUuid);
    if (result != E_OK) {
        return result;
    }
    result = CreateMountUsbFusePath(fsUuid);
    if (result != E_OK) {
        return result;
    }
    fuseFd = open("/dev/fuse", O_RDWR);
    if (fuseFd < 0) {
        LOGE("open /dev/fuse fail for file mgr, errno is %{public}d.", errno);
        return E_OPEN_FUSE;
    }
    LOGI("open fuse end.");
    string opt = StringPrintf("fd=%i,"
        "rootmode=40000,"
        "default_permissions,"
        "allow_other,"
        "user_id=0,group_id=0,"
        "context=\"u:object_r:hmdfs:s0\","
        "fscontext=u:object_r:hmdfs:s0",
        fuseFd);
 
    int ret = StorageDaemon::Mount("/dev/fuse", mountUsbFusePath_.c_str(),
                                   "fuse", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME, opt.c_str());
    if (ret) {
        LOGE("failed to mount fuse for file mgr, ret is %{public}d, errno is %{public}d.", ret, errno);
        close(fuseFd);
        std::string extraData = "dstPath=" + mountUsbFusePath_ + ",kernelCode=" + to_string(errno);
        return E_MOUNT_FILE_MGR_FUSE;
    }
    return E_OK;
}
 
int32_t VolumeManager::ReadVolumUuid(std::string volumeId, std::string &fsUuid)
{
    std::string devPath = StringPrintf("/dev/block/%s", (volumeId).c_str());
    int32_t ret = OHOS::StorageDaemon::ReadVolumUuid(devPath, fsUuid);
    return ret;
}
 
int32_t VolumeManager::CreateMountUsbFusePath(std::string fsUuid)
{
    LOGI("CreateMountUsbFusePath create path");
    struct stat statbuf;
    if (fsUuid.find("..") != std::string::npos) {
        LOGE("Invalid fsUuid: %{public}s, contains path traversal characters", GetAnonyString(fsUuid).c_str());
        return E_PARAMS_INVALID;
    }
    mountUsbFusePath_ = StringPrintf("/mnt/data/external/%s", fsUuid.c_str());
    if (!lstat(mountUsbFusePath_.c_str(), &statbuf)) {
        LOGE("volume mount path %{public}s exists, please remove first", GetAnonyString(mountUsbFusePath_).c_str());
        remove(mountUsbFusePath_.c_str());
        return E_SYS_KERNEL_ERR;
    }
    if (mkdir(mountUsbFusePath_.c_str(), S_IRWXU | S_IRWXG | S_IXOTH)) {
        LOGE("the volume %{public}s create path %{public}s failed",
             GetAnonyString(fsUuid).c_str(), GetAnonyString(mountUsbFusePath_).c_str());
        return E_MKDIR_MOUNT;
    }
    LOGI("CreateMountUsbFusePath create path out");
    return E_OK;
}

int32_t VolumeManager::TryToFix(const std::string volId, uint32_t flags)
{
    std::shared_ptr<VolumeInfo> info = GetVolume(volId);
    if (info == nullptr) {
        LOGE("the volume %{public}s does not exist.", volId.c_str());
        return E_NON_EXIST;
    }
    int32_t errFix = info->DoTryToFix();
    LOGE("The volume result: %{public}d.", errFix);
    if (errFix != E_OK) {
        LOGE("the volume %{public}s fix failed, ret: %{public}d.", volId.c_str(), errFix);
    }

    int32_t currentState = info->GetState();
    if (currentState == MOUNTED) {
        LOGE("the volume has mounted.id: %{public}s", volId.c_str());
        return errFix;
    }

    int32_t errMount = info->UMount();
    if (errMount != E_OK) {
        LOGE("the volume %{public}s UMount failed, ret:%{public}d.", volId.c_str(), errMount);
    }

    errMount = info->Check();
    if (errMount != E_OK) {
        LOGE("the volume %{public}s check failed.", volId.c_str());
        return errMount;
    }

    errMount = info->Mount(flags);
    if (errMount != E_OK) {
        LOGE("the volume %{public}s mount failed.", volId.c_str());
        return errMount;
    }

    StorageManagerClient client;
    errMount = client.NotifyVolumeMounted(info);
    if (errMount) {
        LOGE("Volume Notify Mount Destroyed failed");
    }
    return E_OK;
}

int32_t VolumeManager::UMount(const std::string volId)
{
    std::shared_ptr<VolumeInfo> info = GetVolume(volId);
    if (info == nullptr) {
#ifdef SUPPORT_OPEN_SOURCE_MTP_DEVICE
        return DelayedSingleton<OHOS::StorageDaemon::MtpDeviceMonitor>::GetInstance()->Umount(volId);
#else
        LOGE("the volume %{public}s does not exist.", volId.c_str());
        return E_NON_EXIST;
#endif
    }

    int32_t err = info->UMount();
    if (err != E_OK) {
        LOGE("the volume %{public}s mount failed.", volId.c_str());
        return err;
    }
    return E_OK;
}

int32_t VolumeManager::Format(const std::string volId, const std::string fsType)
{
    std::shared_ptr<VolumeInfo> info = GetVolume(volId);
    if (info == nullptr) {
        LOGE("the volume %{public}s does not exist.", volId.c_str());
        return E_NON_EXIST;
    }

    int32_t err = info->Format(fsType);
    if (err != E_OK) {
        LOGE("the volume %{public}s format failed.", volId.c_str());
        StorageRadar::ReportVolumeOperation("VolumeInfo::Format", err);
        return err;
    }

    return E_OK;
}

int32_t VolumeManager::SetVolumeDescription(const std::string volId, const std::string description)
{
    std::shared_ptr<VolumeInfo> info = GetVolume(volId);
    if (info == nullptr) {
        LOGE("the volume %{public}s does not exist.", volId.c_str());
        return E_NON_EXIST;
    }

    int32_t err = info->SetVolumeDescription(description);
    if (err != E_OK) {
        LOGE("the volume %{public}s setVolumeDescription failed.", volId.c_str());
        StorageRadar::ReportVolumeOperation("VolumeInfo::SetVolumeDescription", err);
        return err;
    }

    return E_OK;
}

int32_t VolumeManager::QueryUsbIsInUse(const std::string &diskPath, bool &isInUse)
{
    isInUse = true;
    if (diskPath.empty()) {
        LOGE("diskPath is null");
        return E_PARAMS_NULLPTR_ERR;
    }
    char realPath[PATH_MAX] = { 0 };
    if (realpath(diskPath.c_str(), realPath) == nullptr) {
        LOGE("get realpath failed for diskPath =%{public}s.", diskPath.c_str());
        return E_PARAMS_INVALID;
    }
    if (strncmp(realPath, diskPath.c_str(), diskPath.size()) != 0) {
        LOGE("diskPath is not equal to realPath, realPath.size = %{public}zu, diskPath.size() = %{public}zu",
            string_view(realPath).size(), diskPath.size());
        return E_PARAMS_INVALID;
    }
    int fd = open(realPath, O_RDONLY);
    if (fd < 0) {
        LOGE("open file fail realPath %{public}s, errno %{public}d", realPath, errno);
        return E_OPEN_FAILED;
    }
    int inUse = -1;
    if (ioctl(fd, STORAGE_MANAGER_IOC_CHK_BUSY, &inUse) < 0) {
        LOGE("ioctl check in use failed errno %{public}d", errno);
        close(fd);
        return E_IOCTL_FAILED;
    }

    if (inUse) {
        LOGI("usb inuse number is %{public}d", inUse);
        close(fd);
        isInUse = true;
        return E_OK;
    }
    LOGI("usb not inUse");
    isInUse = false;
    close(fd);
    return E_OK;
}
} // StorageDaemon
} // OHOS
