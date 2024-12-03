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

#include "utils/disk_utils.h"

#include <cerrno>
#include <fcntl.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <unordered_map>

#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "utils/file_utils.h"

using namespace std;
namespace OHOS {
namespace StorageDaemon {
static constexpr int32_t NODE_PERM = 0660;

int CreateDiskNode(const std::string &path, dev_t dev)
{
    const char *kPath = path.c_str();
    if (mknod(kPath, NODE_PERM | S_IFBLK, dev) < 0) {
        LOGE("create disk node failed");
        return E_ERR;
    }
    return E_OK;
}

int DestroyDiskNode(const std::string &path)
{
    const char *kPath = path.c_str();
    if (TEMP_FAILURE_RETRY(unlink(kPath)) < 0) {
        return E_ERR;
    }
    return E_OK;
}

int GetDevSize(const std::string &path, uint64_t *size)
{
    const char *kPath = path.c_str();
    FILE *f = fopen(kPath, "r");
    if (f == nullptr) {
        LOGE("open %{private}s failed", path.c_str());
        return E_ERR;
    }
    int fd = fileno(f);
    if (fd < 0) {
        LOGE("open %{private}s failed", path.c_str());
        return E_ERR;
    }

    if (ioctl(fd, BLKGETSIZE64, size)) {
        LOGE("get device %{private}s size failed", path.c_str());
        (void)fclose(f);
        return E_ERR;
    }

    (void)fclose(f);
    return E_OK;
}

int GetMaxVolume(dev_t device)
{
    unsigned int majorId = major(device);
    if (majorId == DISK_MMC_MAJOR) {
        std::string str;
        if (!ReadFile(MMC_MAX_VOLUMES_PATH, &str)) {
            LOGE("Get MmcMaxVolumes failed");
            return E_ERR;
        }
        return std::stoi(str);
    } else {
        return MAX_SCSI_VOLUMES;
    }
}

int32_t ReadMetadata(const std::string &devPath, std::string &uuid, std::string &type, std::string &label)
{
    uuid = GetBlkidData(devPath, "UUID");
    type = GetBlkidData(devPath, "TYPE");
    label = GetBlkidData(devPath, "LABEL");
    LOGI("ReadMetadata, fsUuid=%{public}s, fsType=%{public}s, fsLabel=%{public}s.", GetAnonyString(uuid).c_str(),
        type.c_str(), label.c_str());
    if (uuid.empty() || type.empty()) {
        LOGE("External volume ReadMetadata error.");
        return E_ERR;
    }
    if (type == "hmfs" && label != "/data") {
        LOGE("Failed to mount the partition: Unsupported");
        return E_ERR;
    }
    return E_OK;
}

std::string GetBlkidData(const std::string &devPath, const std::string &type)
{
    std::vector<std::string> cmd;
    cmd = {
        "blkid",
        "-s",
        type,
        "-o",
        "value",
        devPath
    };
    return GetBlkidDataByCmd(cmd);
}

std::string GetBlkidDataByCmd(std::vector<std::string> &cmd)
{
    std::vector<std::string> output;

    int32_t err = ForkExec(cmd, &output);
    if (err) {
        return "";
    }

    if (output.size() > 0) {
        size_t sep = output[0].find_first_of("\n");
        if (sep != string::npos)
            output[0].resize(sep);
        return output[0];
    }
    return "";
}

std::string GetAnonyString(const std::string &value)
{
    constexpr size_t INT32_SHORT_ID_LENGTH = 20;
    constexpr size_t INT32_PLAINTEXT_LENGTH = 4;
    constexpr size_t INT32_MIN_ID_LENGTH = 3;
    std::string res;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LENGTH) {
        return tmpStr;
    }

    if (strLen <= INT32_SHORT_ID_LENGTH) {
        res += value[0];
        res += tmpStr;
        res += value[strLen - 1];
    } else {
        res.append(value, 0, INT32_PLAINTEXT_LENGTH);
        res += tmpStr;
        res.append(value, strLen - INT32_PLAINTEXT_LENGTH, INT32_PLAINTEXT_LENGTH);
    }

    return res;
}
} // namespace STORAGE_DAEMON
} // namespace OHOS
