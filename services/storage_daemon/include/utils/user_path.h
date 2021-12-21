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
#ifndef STORAGE_DAEMON_UTILS_USER_PATH_H
#define STORAGE_DAEMON_UTILS_USER_PATH_H
#include <sys/types.h>
#include <string>
#include <vector>

namespace OHOS {
namespace StorageDaemon {

struct DirInfo {
        const char *path;
        mode_t mode;
        uid_t uid;
        gid_t gid;
};

extern std::vector<DirInfo> g_el1DirVec;
extern std::vector<DirInfo> g_el2DirVec;
extern std::vector<DirInfo> g_hmdfsDirVec;

const std::string BIND_MOUNT_SOURCE = "/data/service/el2/%s/hmdfs/files";
const std::string BIND_MOUNT_TARGET = "/storage/media/%s/local";

inline std::string StringPrintf(const char *c, ...)
{
        return std::string();
}



}
}
#endif // STORAGE_DAEMON_UTILS_USER_PATH_H