/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "mtpfs_fuse.h"

#include <fuse_opt.h>
#include <thread>
#include <unistd.h>

#include "mtpfs_util.h"
#include "storage_service_log.h"
#include "string_utils.h"

#define PERMISSION_ONE 0775
#define PERMISSION_TWO 0644

constexpr int UPLOAD_RECORD_FALSE_LEN = 5;
constexpr int UPLOAD_RECORD_TRUE_LEN = 4;
constexpr int32_t UPLOAD_RECORD_SUCCESS_LEN = 7;

constexpr int32_t ST_NLINK_TWO = 2;
constexpr int32_t FILE_SIZE = 512;
constexpr int32_t BS_SIZE = 1024;
constexpr int32_t ARG_SIZE = 2;
constexpr const char *MTP_FILE_FLAG = "?MTP_THM";
constexpr const char *MTP_CLIENT_WRITE = "constraint.mtp.client.write";
std::shared_ptr<AccountSubscriber> osAccountSubscriber_ = nullptr;

int WrapGetattr(const char *path, struct stat *buf, struct fuse_file_info *fi)
{
    (void)fi;
    int ret = E_OK;
    if (OHOS::StorageDaemon::IsEndWith(path, MTP_FILE_FLAG)) {
        ret = DelayedSingleton<MtpFileSystem>::GetInstance()->GetThumbAttr(std::string(path), buf);
    } else {
        ret = DelayedSingleton<MtpFileSystem>::GetInstance()->GetAttr(path, buf);
    }
    return ret;
}

int WrapMkNod(const char *path, mode_t mode, dev_t dev)
{
    LOGI("mtp WrapMkNod, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapMkNod fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->MkNod(path, mode, dev);
    LOGI("MkNod ret = %{public}d.", ret);
    return ret;
}

int WrapMkDir(const char *path, mode_t mode)
{
    LOGI("mtp WrapMkDir, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapMkDir fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->MkDir(path, mode);
    LOGI("MkDir ret = %{public}d.", ret);
    return ret;
}

int WrapUnLink(const char *path)
{
    LOGI("mtp WrapUnLink, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapUnLink fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->UnLink(path);
    LOGI("UnLink ret = %{public}d.", ret);
    return ret;
}

int WrapRmDir(const char *path)
{
    LOGI("mtp WrapRmDir, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapRmDir fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->RmDir(path);
    LOGI("RmDir ret = %{public}d.", ret);
    return ret;
}

int WrapReName(const char *path, const char *newpath, unsigned int flags)
{
    LOGI("mtp WrapReName, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapReName fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->ReName(path, newpath, flags);
    LOGI("ReName ret = %{public}d.", ret);
    return ret;
}

int WrapChMod(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    LOGI("mtp WrapChMod, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapChMod fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->ChMods(path, mode, fi);
    LOGI("ChMods ret = %{public}d.", ret);
    return ret;
}

int WrapChown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi)
{
    LOGE("mtp WrapChown path:%{public}s ,uid:%{public}lu, gid:%{public}lu", path, uid, gid);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapChown fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->Chown(path, uid, gid, fi);
    LOGI("Chown ret = %{public}d.", ret);
    return ret;
}

int WrapUTimens(const char *path, const struct timespec tv[2], struct fuse_file_info *fi)
{
    LOGI("mtp WrapUTimens, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapUTimens fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->UTimens(path, tv, fi);
    LOGI("UTimens ret = %{public}d.", ret);
    return ret;
}

int WrapOpen(const char *path, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapOpen, path=%{public}s", path);
    int ret = E_OK;
    if (OHOS::StorageDaemon::IsEndWith(path, MTP_FILE_FLAG)) {
        ret = DelayedSingleton<MtpFileSystem>::GetInstance()->OpenThumb(path, fileInfo);
    } else {
        ret = DelayedSingleton<MtpFileSystem>::GetInstance()->OpenFile(path, fileInfo);
    }
    LOGI("Open ret = %{public}d.", ret);
    return ret;
}

int WrapRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapRead, path=%{public}s", path);
    int ret = E_OK;
    if (OHOS::StorageDaemon::IsEndWith(path, MTP_FILE_FLAG)) {
        ret = DelayedSingleton<MtpFileSystem>::GetInstance()->ReadThumb(std::string(path), buf);
    } else {
        ret = DelayedSingleton<MtpFileSystem>::GetInstance()->ReadFile(path, buf, size, offset, fileInfo);
    }
    LOGI("Read ret = %{public}d.", ret);
    return ret;
}

int WrapWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapWrite, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapWrite fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->Write(path, buf, size, offset, fileInfo);
    LOGI("Write ret = %{public}d.", ret);
    return ret;
}

int WrapStatfs(const char *path, struct statvfs *statInfo)
{
    LOGI("mtp WrapStatfs, path=%{public}s", path);
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->Statfs(path, statInfo);
    LOGI("Statfs ret = %{public}d.", ret);
    return ret;
}

int WrapFlush(const char *path, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapFlush, path=%{public}s", path);
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->Flush(path, fileInfo);
    LOGI("Flush ret = %{public}d.", ret);
    return ret;
}

int WrapRelease(const char *path, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapRelease, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapRelease fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->Release(path, fileInfo);
    LOGI("Release ret = %{public}d.", ret);
    return ret;
}

int WrapFSync(const char *path, int datasync, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapFSync, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapFSync fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->FSync(path, datasync, fileInfo);
    LOGI("FSync ret = %{public}d.", ret);
    return ret;
}

int WrapOpenDir(const char *path, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapOpenDir, path=%{public}s", path);
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->OpenDir(path, fileInfo);
    LOGI("OpenDir ret = %{public}d.", ret);
    return ret;
}

int WrapReadDir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo,
    enum fuse_readdir_flags flag)
{
    LOGI("mtp WrapReadDir, path=%{public}s", path);
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->ReadDir(path, buf, filler, offset, fileInfo, flag);
    LOGI("ReadDir ret = %{public}d.", ret);
    return ret;
}

int WrapReleaseDir(const char *path, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapReleaseDir, path=%{public}s", path);
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->ReleaseDir(path, fileInfo);
    LOGI("ReleaseDir ret = %{public}d.", ret);
    return ret;
}

int WrapFSyncDir(const char *path, int datasync, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapFSyncDir, path=%{public}s", path);
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->FSyncDir(path, datasync, fileInfo);
    LOGI("FSyncDir ret = %{public}d.", ret);
    return ret;
}

void *WrapInit(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
    LOGI("mtp WrapInit");
    std::set<OHOS::AccountSA::OsAccountState> states = { OHOS::AccountSA::OsAccountState::SWITCHED };
    bool withHandShake = true;
    OHOS::AccountSA::OsAccountSubscribeInfo subscribeInfo(states, withHandShake);
    osAccountSubscriber_ = std::make_shared<AccountSubscriber>(subscribeInfo);
    ErrCode errCode = OHOS::AccountSA::OsAccountManager::SubscribeOsAccount(osAccountSubscriber_);
    LOGI("subscribe os accouunt done errCode = %{public}d", errCode);
    DelayedSingleton<MtpFileSystem>::GetInstance()->InitCurrentUidAndCacheMap();
    return DelayedSingleton<MtpFileSystem>::GetInstance()->Init(conn, cfg);
}

int WrapCreate(const char *path, mode_t mode, fuse_file_info *fileInfo)
{
    LOGI("mtp WrapCreate, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapCreate fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->Create(path, mode, fileInfo);
    LOGI("Create ret = %{public}d.", ret);
    return ret;
}

int WrapTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo)
{
    LOGI("mtp WrapTruncate");
    return 0;
}

int WrapReadLink(const char *path, char *out, size_t size)
{
    LOGI("mtp WrapReadLink");
    return 0;
}

int WrapSymLink(const char *path, const char * mode)
{
    LOGI("mtp WrapSymLink");
    return 0;
}

int WrapLink(const char *path, const char *out)
{
    LOGI("mtp WrapLink");
    return 0;
}

int WrapSetXAttr(const char *path, const char *in, const char *out, size_t size, int flag)
{
    LOGI("mtp WrapSetXAttr, path=%{public}s", path);
    bool readOnly = DelayedSingleton<MtpFileSystem>::GetInstance()->IsCurrentUserReadOnly();
    if (readOnly) {
        LOGI("WrapSetXAttr fail");
        return E_CURRENT_USER_READONLY;
    }
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->SetXAttr(path, in);
    LOGI("WrapSetXAttr ret = %{public}d.", ret);
    return ret;
}

int WrapGetXAttr(const char *path, const char *in, char *out, size_t size)
{
    LOGD("mtp WrapGetXAttr, path=%{public}s", path);
    int ret = DelayedSingleton<MtpFileSystem>::GetInstance()->GetXAttr(path, in, out, size);
    LOGD("WrapGetXAttr ret = %{public}d.", ret);
    return ret;
}

int WrapListXAttr(const char *path, char *in, size_t size)
{
    LOGI("mtp WrapListXAttr");
    return 0;
}

int WrapRemoveXAttr(const char *path, const char *in)
{
    LOGI("mtp WrapRemoveXAttr");
    return 0;
}

void WrapDestroy(void *path)
{
    LOGI("mtp WrapDestroy");
    ErrCode errCode = OHOS::AccountSA::OsAccountManager::UnsubscribeOsAccount(osAccountSubscriber_);
    LOGI("UnsubscribeOsAccount errCode is: %{public}d", errCode);
    return;
}

int WrapAccess(const char *path, int size)
{
    LOGI("mtp WrapAccess");
    return 0;
}

MtpFileSystem::MtpFileSystemOptions::MtpFileSystemOptions()
    : good_(false),
      verBose_(false),
      enableMove_(false),
      deviceNo_(1),
      deviceFile_(nullptr),
      mountPoint_(nullptr)
{}

MtpFileSystem::MtpFileSystemOptions::~MtpFileSystemOptions()
{
    free(static_cast<void *>(deviceFile_));
    free(static_cast<void *>(mountPoint_));
}

int MtpFileSystem::MtpFileSystemOptions::OptProc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
    MtpFileSystemOptions *options = static_cast<MtpFileSystemOptions *>(data);

    if (key == FUSE_OPT_KEY_NONOPT && options != nullptr) {
        if (options->mountPoint_ && options->deviceFile_) {
            // Unknown positional argument supplied
            return -1;
        }
        if (options->deviceFile_) {
            fuse_opt_add_opt(&options->mountPoint_, arg);
            return 0;
        }

        fuse_opt_add_opt(&options->deviceFile_, arg);
        return 0;
    }
    return 1;
}

MtpFileSystem::MtpFileSystem() : args_(), tmpFilesPool_(), options_(), device_()
{
    LOGI("mtp MtpFileSystem");
    fuseOperations_.getattr = WrapGetattr;
    fuseOperations_.readlink = WrapReadLink;
    fuseOperations_.mknod = WrapMkNod;
    fuseOperations_.mkdir = WrapMkDir;
    fuseOperations_.unlink = WrapUnLink;
    fuseOperations_.rmdir = WrapRmDir;
    fuseOperations_.symlink = WrapSymLink;
    fuseOperations_.rename = WrapReName;
    fuseOperations_.link = WrapLink;
    fuseOperations_.chmod = WrapChMod;
    fuseOperations_.chown = WrapChown;
    fuseOperations_.truncate = WrapTruncate;
    fuseOperations_.utimens = WrapUTimens;
    fuseOperations_.open = WrapOpen;
    fuseOperations_.read = WrapRead;
    fuseOperations_.write = WrapWrite;
    fuseOperations_.statfs = WrapStatfs;
    fuseOperations_.flush = WrapFlush;
    fuseOperations_.release = WrapRelease;
    fuseOperations_.fsync = WrapFSync;
    fuseOperations_.setxattr = WrapSetXAttr;
    fuseOperations_.getxattr = WrapGetXAttr;
    fuseOperations_.listxattr = WrapListXAttr;
    fuseOperations_.removexattr = WrapRemoveXAttr;
    fuseOperations_.opendir = WrapOpenDir;
    fuseOperations_.readdir = WrapReadDir;
    fuseOperations_.releasedir = WrapReleaseDir;
    fuseOperations_.fsyncdir = WrapFSyncDir;
    fuseOperations_.init = WrapInit;
    fuseOperations_.destroy = WrapDestroy;
    fuseOperations_.access = WrapAccess;
    fuseOperations_.create = WrapCreate;
    fuseOperations_.ioctl = nullptr;
    fuseOperations_.bmap = nullptr;
    fuseOperations_.read_buf = nullptr;
    fuseOperations_.lseek = nullptr;
    fuseOperations_.copy_file_range = nullptr;
    fuseOperations_.fallocate = nullptr;
    fuseOperations_.lock = nullptr;
    fuseOperations_.flock = nullptr;
    fuseOperations_.poll = nullptr;
    fuseOperations_.write_buf = nullptr;
}

MtpFileSystem::~MtpFileSystem()
{
    for (auto iter : dirMap_) {
        MtpFsTypeDir *dir = const_cast<MtpFsTypeDir *>(iter.second);
        if (dir != nullptr) {
            LOGI("MtpFileSystem FreeObjectHandles, dirname=%{public}s", dir->Name().c_str());
            device_.FreeObjectHandles(dir);
        }
    }
    fuse_opt_free_args(&args_);
}

bool MtpFileSystem::ParseOptionsInner()
{
    fuse_opt_add_arg(&args_, options_.mountPoint_);

    if (options_.verBose_) {
        fuse_opt_add_arg(&args_, "-f");
    }

    --options_.deviceNo_;

    // device file and -- device are mutually exclusive, fail if both set
    if (options_.deviceNo_ && options_.deviceFile_) {
        options_.good_ = false;
        return false;
    }
    options_.good_ = true;
    return true;
}

bool MtpFileSystem::ParseOptions(int argc, char **argv)
{
    LOGI("mtp MtpFileSystem ParseOptions");
#define SMTPFS_OPT_KEY(t, p, v)                  \
    {                                            \
        t, offsetof(MtpFileSystemOptions, p), v \
    }

    static struct fuse_opt smtpfs_opts[] = {
        SMTPFS_OPT_KEY("enable-move", enableMove_, 1),
        SMTPFS_OPT_KEY("--device %i", deviceNo_, 0),
        SMTPFS_OPT_KEY("-v", verBose_, 1),
        SMTPFS_OPT_KEY("--verbose", verBose_, 1),

            FUSE_OPT_END
                                           };

    if (argc < ARG_SIZE) {
        LOGE("Wrong usage");
        options_.good_ = false;
        return false;
    }

    fuse_opt_free_args(&args_);
    args_ = FUSE_ARGS_INIT(argc, argv);
    if (fuse_opt_parse(&args_, &options_, smtpfs_opts, MtpFileSystemOptions::OptProc) == -1) {
        options_.good_ = false;
        return false;
    }

    if (options_.deviceFile_ && !options_.mountPoint_) {
        options_.mountPoint_ = options_.deviceFile_;
        options_.deviceFile_ = nullptr;
    }

    if (!options_.mountPoint_) {
        LOGE("Mount point missing");
        options_.good_ = false;
        return false;
    }
    if (!ParseOptionsInner()) {
        return false;
    }
    return true;
}

bool MtpFileSystem::Exec()
{
    if (!options_.good_) {
        return false;
    }

    if (!SmtpfsCheckDir(options_.mountPoint_)) {
        LOGE("Can not mount the device to %{public}s", options_.mountPoint_);
        return false;
    }

    if (!tmpFilesPool_.CreateTmpDir()) {
        LOGE("Can not create a temporary directory");
        return false;
    }

    if (options_.deviceFile_) {
        // Try to use device file first, if provided
        if (!device_.ConnectByDevFile(options_.deviceFile_)) {
            return false;
        }
    } else {
        // Connect to MTP device by order number, if no device file supplied
        LOGI("Attempting to connect device by order number: %d", options_.deviceNo_);
        if (!device_.ConnectByDevNo(options_.deviceNo_)) {
            return false;
        }
    }
    device_.EnableMove(options_.enableMove_);
    int ret = fuse_main(args_.argc, args_.argv, &fuseOperations_, nullptr);
    if (ret > 0) {
        LOGE("fuse_main fail, ret = %{public}d", ret);
        return false;
    }
    device_.Disconnect();

    if (!tmpFilesPool_.RemoveTmpDir()) {
        LOGE("Can not remove a temporary directory");
        return false;
    }

    return true;
}

void *MtpFileSystem::Init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
    device_.InitDevice();
    cfg->attr_timeout = 0;
    LOGI("MtpFileSystem: Init end, cfg->attr_timeout=%{public}f", cfg->attr_timeout);
    return nullptr;
}

int MtpFileSystem::GetAttr(const char *path, struct stat *buf)
{
    LOGI("MtpFileSystem: GetAttr enter, path: %{public}s", path);
    if (memset_s(buf, sizeof(struct stat), 0, sizeof(struct stat)) != EOK) {
        LOGE("memset stat fail");
    }
    struct fuse_context *fc = fuse_get_context();
    if (buf == nullptr || fc == nullptr) {
        LOGE("MtpFileSystem: buf is null or get file contxt failed");
        return -ENOENT;
    }
    buf->st_uid = fc->uid;
    buf->st_gid = fc->gid;
    if (path == std::string("/")) {
        buf->st_mode = S_IFDIR | PERMISSION_ONE;
        buf->st_nlink = ST_NLINK_TWO;
        return 0;
    } else {
        std::string tmpPath(SmtpfsDirName(path));
        std::string mtpFile(SmtpfsBaseName(path));
        const MtpFsTypeDir *content = device_.ReadDirFetchContent(tmpPath);
        if (content == nullptr) {
            LOGE("MtpFileSystem: GetAttr error, content is null, path: %{public}s", path);
            return -ENOENT;
        } else if (content->Dir(mtpFile) != nullptr) {
            const MtpFsTypeDir *dir = content->Dir(mtpFile);
            buf->st_ino = dir->Id();
            buf->st_mode = S_IFDIR | PERMISSION_ONE;
            buf->st_nlink = ST_NLINK_TWO;
            buf->st_mtime = dir->ModificationDate();
        } else if (content->File(mtpFile) != nullptr) {
            const MtpFsTypeFile *file = content->File(mtpFile);
            buf->st_ino = file->Id();
            buf->st_size = static_cast<ssize_t>(file->Size());
            buf->st_blocks = static_cast<ssize_t>(file->Size() / FILE_SIZE) + (file->Size() % FILE_SIZE > 0 ? 1 : 0);
            buf->st_nlink = 1;
            buf->st_mode = S_IFREG | PERMISSION_TWO;
            buf->st_mtime = file->ModificationDate();
            buf->st_ctime = buf->st_mtime;
            buf->st_atime = buf->st_mtime;
        } else {
            LOGE("MtpFileSystem: GetAttr error, content dir is null, path: %{public}s", path);
            return -ENOENT;
        }
    }
    LOGI("MtpFileSystem: GetAttr success, path: %{public}s", path);
    return 0;
}

int MtpFileSystem::GetThumbAttr(const std::string &path, struct stat *buf)
{
    LOGI("MtpFileSystem: GetThumbAttr enter, path: %{public}s", path.c_str());
    std::string realPath = path.substr(0, path.length() - strlen(MTP_FILE_FLAG));
    int ret = GetAttr(realPath.c_str(), buf);
    if (ret != 0) {
        LOGE("GetThumbAttr: get attr for path=%{public}s failed.", realPath.c_str());
        return ret;
    }
    char *data = nullptr;
    int bufSize = device_.GetThumbnail(realPath, data);
    if (bufSize <= 0) {
        LOGE("GetThumbAttr for path=%{public}s failed, bufSize invalid.", realPath.c_str());
        return -EIO;
    }
    buf->st_size = static_cast<ssize_t>(bufSize);
    buf->st_blocks = static_cast<ssize_t>(bufSize / FILE_SIZE) + (bufSize % FILE_SIZE > 0 ? 1 : 0);

    LOGI("MtpFileSystem: GetThumbAttr success, path=%{public}s, bufSize=%{public}d.", path.c_str(), bufSize);
    return E_OK;
}

int MtpFileSystem::MkNod(const char *path, mode_t mode, dev_t dev)
{
    if (!S_ISREG(mode)) {
        return -EINVAL;
    }
    std::string tmpPath = tmpFilesPool_.MakeTmpPath(std::string(path));
    int rval = ::open(tmpPath.c_str(), O_CREAT | O_WRONLY, mode);
    if (rval < 0) {
        return -errno;
    }
    rval = ::close(rval);
    if (rval < 0) {
        return -errno;
    }
    rval = device_.FilePush(tmpPath, std::string(path));
    ::unlink(tmpPath.c_str());

    if (rval != 0) {
        return rval;
    }
    return 0;
}

int MtpFileSystem::MkDir(const char *path, mode_t mode)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    return device_.DirCreateNew(std::string(path));
}

int MtpFileSystem::UnLink(const char *path)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    return device_.FileRemove(std::string(path));
}

int MtpFileSystem::RmDir(const char *path)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    return device_.DirRemove(std::string(path));
}

int MtpFileSystem::ReName(const char *path, const char *newpath, unsigned int flags)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    LOGI("MtpFileSystem: ReName, path=%{public}s, newpath=%{public}s", path, newpath);
    const std::string tmpOldDirName(SmtpfsDirName(std::string(path)));
    const std::string tmpNewDirName(SmtpfsDirName(std::string(newpath)));
    if (tmpOldDirName == tmpNewDirName) {
        return device_.ReName(std::string(path), std::string(newpath));
    }
    if (!options_.enableMove_) {
        return -EPERM;
    }
    const std::string tmpFile = tmpFilesPool_.MakeTmpPath(std::string(newpath));
    int rval = device_.FilePull(std::string(path), tmpFile);
    if (rval != 0) {
        return -rval;
    }
    rval = device_.FilePush(tmpFile, std::string(newpath));
    if (rval != 0) {
        return -rval;
    }
    rval = device_.FileRemove(std::string(path));
    if (rval != 0) {
        return -rval;
    }
    const std::string tmpBaseName(SmtpfsBaseName(newpath));
    const std::string tmpDirName(SmtpfsDirName(newpath));
    const MtpFsTypeDir *dirParent = device_.DirFetchContent(tmpDirName);
    const int32_t factor = 1000;
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    time_t time = static_cast<time_t>(millisecs.count() / factor);
    if (dirParent != nullptr) {
        LOGI("MtpFileSystem: file cutted, update dirParent mtime");
        const_cast<MtpFsTypeDir *>(dirParent)->SetModificationDate(time);
    }
    return 0;
}

int MtpFileSystem::ChMods(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int res;
    if (fi != nullptr) {
        res = fchmod(fi->fh, mode);
    } else {
        res = chmod(path, mode);
    }
    if (res == -1) {
        return -errno;
    }
    return 0;
}

int MtpFileSystem::Chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi)
{
    LOGI("mtp Chown path:%{public}s ,uid:%{public}lu, gid:%{public}lu", path, uid, gid);
    int res;
    if (fi != nullptr) {
        res = fchown(fi->fh, uid, gid);
    } else {
        res = lchown(path, uid, gid);
    }
    if (res == -1) {
        return -errno;
    }
    return 0;
}

int MtpFileSystem::Truncate(const char *path, off_t new_size, struct fuse_file_info *fileInfo)
{
    const std::string tmpPath = tmpFilesPool_.MakeTmpPath(std::string(path));
    int rval = device_.FilePull(std::string(path), tmpPath);
    if (rval != 0) {
        ::unlink(tmpPath.c_str());
        return -rval;
    }

    rval = ::truncate(tmpPath.c_str(), new_size);
    if (rval != 0) {
        int errnoTmp = errno;
        ::unlink(tmpPath.c_str());
        return -errnoTmp;
    }

    rval = device_.FileRemove(std::string(path));
    if (rval != 0) {
        ::unlink(tmpPath.c_str());
        return -rval;
    }
    rval = device_.FilePush(tmpPath, std::string(path));
    ::unlink(tmpPath.c_str());
    if (rval != 0) {
        return -rval;
    }
    return 0;
}

int MtpFileSystem::UTimens(const char *path, const struct timespec tv[2], struct fuse_file_info *fi)
{
    std::string tmpBaseName(SmtpfsBaseName(std::string(path)));
    std::string tmpDirName(SmtpfsDirName(std::string(path)));
    const MtpFsTypeDir *parent = device_.DirFetchContent(tmpDirName);
    if (parent == nullptr) {
        LOGE("MtpFileSystem: UTimens parent is nullptr");
        return -ENOENT;
    }
    const MtpFsTypeDir *dir = parent->Dir(tmpBaseName);
    if (dir == nullptr) {
        LOGE("MtpFileSystem: UTimens dir is nullptr");
        return -ENOENT;
    }
    const_cast<MtpFsTypeDir *>(dir)->SetModificationDate(tv[1].tv_sec);
    return 0;
}


int MtpFileSystem::Create(const char *path, mode_t mode, fuse_file_info *fileInfo)
{
    const std::string tmpPath = tmpFilesPool_.MakeTmpPath(std::string(path));
    int rval = ::creat(tmpPath.c_str(), mode);
    if (rval < 0) {
        return -errno;
    }
    if (fileInfo != nullptr) {
        fileInfo->fh = static_cast<uint32_t>(rval);
    }
    tmpFilesPool_.AddFile(MtpFsTypeTmpFile(std::string(path), tmpPath, rval, true));
    rval = device_.FilePush(tmpPath, std::string(path));
    if (rval != 0) {
        LOGE("MtpFileSystem: Create, FilePush path: %{public}s fail", path);
        return rval;
    }
    return 0;
}

int MtpFileSystem::OpenFile(const char *path, struct fuse_file_info *fileInfo)
{
    LOGI("MtpFileSystem: OpenFile enter, path: %{public}s", path);
    if (fileInfo == nullptr) {
        LOGE("Missing FileInfo");
        return -ENOENT;
    }
    unsigned int flags = static_cast<unsigned int>(fileInfo->flags);
    if (flags & O_WRONLY) {
        flags |= O_TRUNC;
    }
    fileInfo->flags = static_cast<int>(flags);
    const std::string stdPath(path);
    std::string tmpPath;
    MtpFsTypeTmpFile *tmpFile = const_cast<MtpFsTypeTmpFile *>(tmpFilesPool_.GetFile(stdPath));
    if (tmpFile != nullptr) {
        tmpPath = tmpFile->PathTmp();
    } else {
        tmpPath = tmpFilesPool_.MakeTmpPath(stdPath);
        // only copy the file if needed
        if (!HasGetPartialSupport()) {
            int rval = device_.FilePull(stdPath, tmpPath);
            if (rval != 0) {
                return -rval;
            }
        } else {
            int fd = ::creat(tmpPath.c_str(), S_IRUSR | S_IWUSR);
            ::close(fd);
        }
    }

    // we create the tmp file even if we can use partial get/send to have a valid file descriptor
    char realPath[PATH_MAX] = {0};
    if (realpath(tmpPath.c_str(), realPath) == nullptr) {
        LOGE("MtpFileSystem: realpath error, errno=%{public}d", errno);
        return -errno;
    }
    int fd = ::open(tmpPath.c_str(), fileInfo->flags);
    if (fd < 0) {
        ::unlink(tmpPath.c_str());
        LOGE("MtpFileSystem: OpenFile error, errno=%{public}d", errno);
        return -errno;
    }
    fileInfo->fh = static_cast<uint32_t>(fd);

    if (tmpFile != nullptr) {
        tmpFile->AddFileDescriptor(fd);
    } else {
        tmpFilesPool_.AddFile(MtpFsTypeTmpFile(stdPath, tmpPath, fd));
    }
    LOGI("MtpFileSystem: OpenFile success, path: %{public}s", path);
    return 0;
}

int MtpFileSystem::ReadFile(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    LOGI("MtpFileSystem: ReadFile enter, path: %{public}s", path);
    if (fileInfo == nullptr) {
        LOGE("Missing FileInfo");
        return -ENOENT;
    }
    int rval = 0;
    if (HasGetPartialSupport()) {
        const std::string stdPath(path);
        rval = device_.FileRead(stdPath, buf, size, offset);
    } else {
        rval = ::pread(fileInfo->fh, buf, size, offset);
        if (rval < 0) {
            LOGE("MtpFileSystem: ReadFile error, errno=%{public}d", errno);
            return -errno;
        }
    }
    LOGI("MtpFileSystem: ReadFile success, path: %{public}s, rval=%{public}d", path, rval);
    return rval;
}

int MtpFileSystem::OpenThumb(const char *path, struct fuse_file_info *fileInfo)
{
    LOGI("MtpFileSystem: OpenThumb enter, path: %{public}s", path);
    if (fileInfo == nullptr) {
        LOGE("Missing FileInfo");
        return -ENOENT;
    }
    std::string tmpPath = tmpFilesPool_.MakeTmpPath(std::string(path));
    int fd = ::creat(tmpPath.c_str(), S_IRUSR | S_IWUSR);
    ::close(fd);

    unsigned int flags = static_cast<unsigned int>(fileInfo->flags);
    if (flags & O_WRONLY) {
        flags |= O_TRUNC;
    }
    fileInfo->flags = static_cast<int>(flags);
    char realPath[PATH_MAX] = {0};
    if (realpath(tmpPath.c_str(), realPath) == nullptr) {
        LOGE("MtpFileSystem: realpath error, errno=%{public}d", errno);
        return -errno;
    }
    fd = ::open(tmpPath.c_str(), fileInfo->flags);
    if (fd < 0) {
        ::unlink(tmpPath.c_str());
        LOGE("MtpFileSystem: OpenThumb error, errno=%{public}d", errno);
        return -errno;
    }
    fileInfo->fh = static_cast<uint32_t>(fd);
    LOGI("MtpFileSystem: OpenThumb success, path: %{public}s", path);
    return E_OK;
}

int MtpFileSystem::ReadThumb(const std::string &path, char *buf)
{
    LOGI("MtpFileSystem: ReadThumb enter, path: %{public}s", path.c_str());
    std::string realPath = path.substr(0, path.length() - strlen(MTP_FILE_FLAG));
    return device_.GetThumbnail(realPath, buf);
}

int MtpFileSystem::Write(const char *path, const char *buf, size_t size, off_t offset,
    struct fuse_file_info *fileInfo)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    LOGI("MtpFileSystem: Write enter, path: %{public}s", path);
    if (fileInfo == nullptr) {
        LOGE("Missing FileInfo");
        return -ENOENT;
    }
    int rval = 0;
    if (HasSendPartialSupport()) {
        const std::string stdPath(path);
        rval = device_.FileWrite(stdPath, buf, size, offset);
    } else {
        const MtpFsTypeTmpFile *tmpFile = tmpFilesPool_.GetFile(std::string(path));
        if (tmpFile == nullptr) {
            LOGE("MtpFileSystem: Write tmpFile error.");
            return -EINVAL;
        }
        rval = ::pwrite(fileInfo->fh, buf, size, offset);
        if (rval < 0) {
            LOGE("MtpFileSystem: Write pwrite error, errno=%{public}d", errno);
            return -errno;
        }
        const_cast<MtpFsTypeTmpFile *>(tmpFile)->SetModified();
    }
    LOGI("MtpFileSystem: Write success, path: %{public}s, rval=%{public}d", path, rval);
    return rval;
}

int MtpFileSystem::Release(const char *path, struct fuse_file_info *fileInfo)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    const std::string stdPath(path);
    LOGI("MtpFileSystem: Release enter, path: %{public}s", path);
    if (fileInfo == nullptr) {
        LOGE("Missing FileInfo");
        device_.SetUploadRecord(stdPath, "fail");
        return -ENOENT;
    }
    int rval = ::close(fileInfo->fh);
    if (rval < 0) {
        LOGE("MtpFileSystem: Release close error, errno=%{public}d", errno);
        device_.SetUploadRecord(stdPath, "fail");
        return -errno;
    }
    if (OHOS::StorageDaemon::IsEndWith(path, MTP_FILE_FLAG)) {
        device_.SetUploadRecord(stdPath, "success");
        return 0;
    }
    return HandleTemporaryFile(stdPath, fileInfo);
}

int MtpFileSystem::HandleTemporaryFile(const std::string stdPath, struct fuse_file_info *fileInfo)
{
    MtpFsTypeTmpFile *tmpFile = const_cast<MtpFsTypeTmpFile *>(tmpFilesPool_.GetFile(stdPath));
    if (tmpFile == nullptr) {
        LOGE("failed to get tmpFile.");
        device_.SetUploadRecord(stdPath, "fail");
        return -EINVAL;
    }
    tmpFile->RemoveFileDescriptor(fileInfo->fh);
    if (tmpFile->RefCnt() != 0) {
        device_.SetUploadRecord(stdPath, "success");
        return 0;
    }
    const bool modIf = tmpFile->IsModified();
    const std::string tmpPath = tmpFile->PathTmp();
    tmpFilesPool_.RemoveFile(stdPath);
    struct stat fileStat;
    stat(tmpPath.c_str(), &fileStat);
    if (modIf && fileStat.st_size != 0) {
        device_.SetUploadRecord(stdPath, "sending");
        int rval = device_.FilePush(tmpPath, stdPath);
        if (rval != 0) {
            LOGE("FilePush %{public}s to mtp device fail", stdPath.c_str());
            device_.SetUploadRecord(stdPath, "fail");
            ::unlink(tmpPath.c_str());
            return -rval;
        }
        LOGI("FilePush %{public}s to mtp device success", stdPath.c_str());
        device_.SetUploadRecord(stdPath, "success");
    } else {
        device_.SetUploadRecord(stdPath, "success");
    }
    ::unlink(tmpPath.c_str());
    LOGI("MtpFileSystem: Release success, stdPath: %{public}s", stdPath.c_str());
    return 0;
}

int MtpFileSystem::Statfs(const char *path, struct statvfs *statInfo)
{
    uint64_t bs = BS_SIZE;
    if (statInfo == nullptr) {
        LOGE("Missing statInfo");
        return -ENOENT;
    }
    // XXX: linux coreutils still use bsize member to calculate free space
    statInfo->f_bsize = static_cast<unsigned long>(bs);
    statInfo->f_frsize = static_cast<unsigned long>(bs);
    statInfo->f_blocks = device_.StorageTotalSize() / bs;
    statInfo->f_bavail = device_.StorageFreeSize() / bs;
    statInfo->f_bfree = statInfo->f_bavail;
    return 0;
}

int MtpFileSystem::Flush(const char *path, struct fuse_file_info *fileInfo)
{
    return 0;
}

int MtpFileSystem::FSync(const char *path, int datasync, struct fuse_file_info *fi)
{
    int rval = -1;
    if (fi == nullptr) {
        LOGE("Missing FileInfo");
        return -ENOENT;
    }
#ifdef HAVE_FDATASYNC
    if (datasync) {
        rval = ::fdatasync(fi->fh);
    }
#else
    rval = ::fsync(fi->fh);
#endif
    if (rval != 0) {
        return -errno;
    }
    return 0;
}

int MtpFileSystem::OpenDir(const char *path, struct fuse_file_info *fileInfo)
{
    std::lock_guard<std::mutex>lock(fuseMutex_);
    LOGI("MtpFileSystem: OpenDir, path: %{public}s", path);
    const MtpFsTypeDir *content = device_.OpenDirFetchContent(std::string(path));
    if (content == nullptr) {
        return -ENOENT;
    }
    dirMap_[std::string(path)] = content;
    return 0;
}

int MtpFileSystem::ReadDir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
    struct fuse_file_info *fileInfo, enum fuse_readdir_flags flag)
{
    LOGI("MtpFileSystem: ReadDir, path: %{public}s", path);
    enum fuse_fill_dir_flags fillFlags = FUSE_FILL_DIR_PLUS;
    const MtpFsTypeDir *content = device_.ReadDirFetchContent(std::string(path));
    if (content == nullptr) {
        return -ENOENT;
    }
    const std::set<MtpFsTypeDir> dirs = content->Dirs();
    const std::set<MtpFsTypeFile> files = content->Files();

    for (const MtpFsTypeDir &d : dirs) {
        struct stat st;
        if (memset_s(&st, sizeof(st), 0, sizeof(st)) != EOK) {
            LOGE("memset st fail");
        }
        st.st_ino = d.Id();
        st.st_mode = S_IFDIR | PERMISSION_ONE;
        filler(buf, d.Name().c_str(), &st, 0, fillFlags);
    }

    for (const MtpFsTypeFile &f : files) {
        struct stat st;
        if (memset_s(&st, sizeof(st), 0, sizeof(st)) != EOK) {
            LOGE("memset st fail");
        }
        st.st_ino = f.Id();
        st.st_mode = S_IFREG | PERMISSION_TWO;
        filler(buf, f.Name().c_str(), &st, 0, fillFlags);
    }
    return 0;
}

int MtpFileSystem::ReleaseDir(const char *path, struct fuse_file_info *fileInfo)
{
    return 0;
}

int MtpFileSystem::FSyncDir(const char *path, int datasync, struct fuse_file_info *fileInfo)
{
    return 0;
}

bool MtpFileSystem::HasGetPartialSupport()
{
    MtpFsDevice::Capabilities caps = device_.GetCapabilities();
    return caps.CanGetPartialObject();
}

bool MtpFileSystem::HasSendPartialSupport()
{
    return false;
}

int MtpFileSystem::SetXAttr(const char *path, const char *in)
{
    if (path == nullptr || in == nullptr) {
        LOGE("Param is null.");
        return -ENOENT;
    }
    if (strcmp(in, "user.rmdir") == 0) {
        LOGI("rmdir directly, dir path=%{public}s", path);
        int32_t ret = device_.DirRemoveDirectly(path);
        return ret;
    }
    if (strcmp(in, "user.fetchcontent") == 0) {
        LOGI("Refresh the mtp dir content, dir=%{public}s", path);
        return 0;
    }
    if (strcmp(in, "user.cancelcopy") == 0) {
        LOGI("Cancel the mtp transfer task, path=%{public}s", path);
        return 0;
    }
    if (strcmp(in, "user.isUploadCompleted") != 0) {
        LOGE("attrKey error, attrKey=%{public}s", in);
        return -ENOENT;
    }
    device_.AddUploadRecord(std::string(path), "sending");
    return 0;
}

int MtpFileSystem::GetXAttr(const char *path, const char *in, char *out, size_t size)
{
    if (path == nullptr || in == nullptr) {
        LOGE("Param is null.");
        return 0;
    }

    if (out == nullptr || size <= 0) {
        return UPLOAD_RECORD_SUCCESS_LEN;
    }
    if (strcmp(in, "user.isDirFetched") == 0) {
        bool fetch = device_.IsDirFetched(std::string(path));
        int ret;
        if (fetch) {
            ret = memcpy_s(out, size, "true", UPLOAD_RECORD_TRUE_LEN);
        } else {
            ret = memcpy_s(out, size, "false", UPLOAD_RECORD_FALSE_LEN);
        }
        if (ret != 0) {
            LOGE("copy fail, ret=%{public}d", ret);
            return 0;
        }
        return fetch ? UPLOAD_RECORD_TRUE_LEN : UPLOAD_RECORD_FALSE_LEN;
    } else if (strcmp(in, "user.isUploadCompleted") == 0) {
        auto [firstParam, secondParam] = device_.FindUploadRecord(std::string(path));
        if (firstParam.empty()) {
            LOGE("No record, path=%{public}s", path);
            return 0;
        }
        int32_t len = strlen(secondParam.c_str());
        if (secondParam == "success" || secondParam == "fail") {
            if (memcpy_s(out, size, secondParam.c_str(), len) != 0) {
                LOGE("memcpy_s fail");
                return 0;
            }
            device_.RemoveUploadRecord(path);
        } else if (secondParam == "sending") {
            if (memcpy_s(out, size, secondParam.c_str(), len) != 0) {
                LOGE("memcpy_s fail");
                return 0;
            }
        }
        return len;
    } else if (strcmp(in, "user.getfriendlyname") == 0) {
        return GetFriendlyName(in, out, size);
    } else {
        LOGE("attrKey error, attrKey=%{public}s", in);
        return 0;
    }
}

int MtpFileSystem::GetFriendlyName(const char *in, char *out, size_t size)
{
    LOGI("GetXAttr key=%{public}s", in);
    char *deviceName = device_.GetDeviceFriendlyName();
    if (deviceName == nullptr) {
        LOGE("GetDeviceFriendlyName from device fail");
        return -ENOENT;
    }
    int32_t nameLen = strlen(deviceName);
    LOGI("GetDeviceFriendlyName from device success, name=%{public}s, nameLen=%{public}d", deviceName, nameLen);
    int32_t ret = memcpy_s(out, size, deviceName, nameLen);
    if (ret != 0) {
        LOGE("memcpy_s devicename fail, ret=%{public}d", ret);
        return 0;
    }
    return nameLen;
}

void MtpFileSystem::HandleRemove(uint32_t handleId)
{
    std::thread([this, handleId]() { device_.HandleRemoveEvent(handleId); }).detach();
}

void MtpFileSystem::InitCurrentUidAndCacheMap()
{
    LOGI("InitCurrentUidAndCacheMap start");
    std::lock_guard<std::mutex>lock(mtpClientMutex_);
    std::vector<int> activedOsAccountIds;
    ErrCode errCode = OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activedOsAccountIds);
    LOGI("InitCurrentUidAndCacheMap QueryActiveOsAccountIds errCode is: %{public}d", errCode);
    if (activedOsAccountIds.empty()) {
        return;
    }
    currentUid = activedOsAccountIds[0];
    LOGI("InitCurrentUidAndCacheMap currentUid = %{public}d", currentUid);
    for (size_t i = 0; i < activedOsAccountIds.size(); i++) {
        bool readOnly = false;
        ErrCode errCode = OHOS::AccountSA::OsAccountManager
            ::IsOsAccountConstraintEnable(activedOsAccountIds[i], MTP_CLIENT_WRITE, readOnly);
        mtpClientWriteMap_[activedOsAccountIds[i]] = readOnly;
    }
    LOGI("InitCurrentUidAndCacheMap end");
}

bool MtpFileSystem::IsCurrentUserReadOnly()
{
    LOGI("IsCurrentUserReadOnly start");
    auto item = mtpClientWriteMap_.find(currentUid);
    if (item != mtpClientWriteMap_.end()) {
        if (item->second) {
            return true;
        }
    }
    LOGI("IsCurrentUserReadOnly end");
    return false;
}

void MtpFileSystem::SetCurrentUid(int32_t uid)
{
    LOGI("AccountSubscriber::SetCurrentUid start");
    std::lock_guard<std::mutex>lock(mtpClientMutex_);
    currentUid = uid;
    LOGI("AccountSubscriber::SetCurrentUid end");
}

void MtpFileSystem::SetMtpClientWriteMap(uid_t first, bool second)
{
    LOGI("AccountSubscriber::SetMtpClientWriteMap start");
    std::lock_guard<std::mutex>lock(mtpClientMutex_);
    mtpClientWriteMap_[first] = second;
    LOGI("AccountSubscriber::SetMtpClientWriteMap end");
}

void AccountSubscriber::OnStateChanged(const OHOS::AccountSA::OsAccountStateData &data)
{
    LOGI("AccountSubscriber::OnStateChanged start");
    DelayedSingleton<MtpFileSystem>::GetInstance()->SetCurrentUid(data.toId);
    LOGI("AccountSubscriber::OnStateChanged end");
}