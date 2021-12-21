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
#ifndef STORAGE_DAEMON_UTILS_FILE_UTILS_H
#define STORAGE_DAEMON_UTILS_FILE_UTILS_H
#include <stdint.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <string>

namespace OHOS {
namespace StorageDaemon {
constexpr int32_t UMOUNT_RETRY_TIMES = 3;

int32_t PrepareDir(const std::string &path, mode_t mode, uid_t uid, gid_t gid);
int32_t DestroyDir(const std::string &path);
int32_t Mount(const std::string &source, const std::string &target, const char *type, unsigned long flags, const void *data);
int32_t UMount(const std::string &path);
}
}

#endif // STORAGE_DAEMON_UTILS_FILE_UTILS_H