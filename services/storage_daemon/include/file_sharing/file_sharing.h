/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_STORAGE_DAEMON_FILE_SHARING_H
#define OHOS_STORAGE_DAEMON_FILE_SHARING_H

#include <sys/xattr.h>

#include "file_sharing/acl.h"
#include "parameter.h"
#include "storage_service_log.h"
#include "utils/file_utils.h"

namespace OHOS {
namespace StorageDaemon {
int SetupFileSharingDir();
int PrepareFileSharingDir(const std::string &fsShareParam);
int SetupDirAcl(const std::string &fsShareParam);
std::string GetFileShareDefineParameter();
} // namespace StorageDaemon
} // namespace OHOS

#endif // OHOS_STORAGE_DAEMON_FILE_SHARING_H
