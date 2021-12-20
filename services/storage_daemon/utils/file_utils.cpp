/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "utils/file_utils.h"
#include "utils/errno.h"
#include "utils/log.h"
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
namespace OHOS {
namespace StorageDaemon {
constexpr uint32_t ALL_PERMS = (S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO);

int32_t ChMod(const std::string &path, mode_t mode)
{
    return TEMP_FAILURE_RETRY(chmod(path.c_str(), mode));
}

int32_t ChOwn(const std::string &path, uid_t uid, gid_t gid)
{
    return TEMP_FAILURE_RETRY(chown(path.c_str(), uid, gid));
}

int32_t MkDir(const std::string &path, mode_t mode)
{
    return TEMP_FAILURE_RETRY(mkdir(path.c_str(), mode));
}

int32_t RmDir(const std::string &path)
{
    return TEMP_FAILURE_RETRY(rmdir(path.c_str()));
}

// On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
int32_t PrepareDir(const std::string &path, mode_t mode, uid_t uid, gid_t gid)
{
    LOGI("prepare for %{public}s", path.c_str());
    int err = E_OK;

    // check whether the path exists
    struct stat st;
    if (TEMP_FAILURE_RETRY(lstat(path.c_str(), &st)) == E_ERR) {
        if (errno != ENOENT) {
            return E_ERR;
        }
    } else {
        if (!S_ISDIR(st.st_mode)) {
            return E_ERR;
        }

        if ((st.st_mode & ALL_PERMS) != mode) {
            err = ChMod(path, mode);
        }

        if ((st.st_uid != uid) || (st.st_gid != gid)) {
            err = ChOwn(path, uid, gid);
        }

        return err;
    }

    if ((err = MkDir(path, mode))) {
        return err;
    }

    if ((err = ChOwn(path, uid, gid))) {
        return err;
    }

    return err;
}

// On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
int32_t DestroyDir(const std::string &path)
{
    LOGI("destroy for %{public}s", path.c_str());

    if(RmDir(path) == E_ERR && errno != ENOENT) {
        LOGE("failed to rmdir %{public}s, err reason %{public}s", path.c_str(), strerror(errno));
        return E_ERR;
    }

    return E_OK;
}

} // STORAGE_DAEMON
} // OHOS