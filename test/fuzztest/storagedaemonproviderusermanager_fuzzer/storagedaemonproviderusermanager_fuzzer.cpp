/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "storagedaemonproviderusermanager_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "message_parcel.h"
#include "storage_daemon.h"
#include "storage_daemon_provider.h"
#include "storage_daemon_stub.h"
#include "user_manager.h"

using namespace OHOS::StorageDaemon;

namespace OHOS {

StorageDaemon::UserManager &userManager = StorageDaemon::UserManager::GetInstance();

bool UserManagerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t) + sizeof(uint32_t))) {
        return false;
    }

    int32_t userId = *(reinterpret_cast<const int32_t *>(data));
    uint32_t flag = *(reinterpret_cast<const uint32_t *>(data + sizeof(uint32_t)));
    userManager.PrepareUserDirs(userId, flag);
    userManager.DestroyUserDirs(userId, flag);
    userManager.StartUser(userId);
    userManager.StopUser(userId);
    userManager.CreateElxBundleDataDir(userId, static_cast<uint8_t>(flag));

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::UserManagerFuzzTest(data, size);
    return 0;
}
