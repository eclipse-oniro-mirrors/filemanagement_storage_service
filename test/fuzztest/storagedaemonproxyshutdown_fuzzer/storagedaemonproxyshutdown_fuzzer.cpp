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
#include "storagedaemonproxyshutdown_fuzzer.h"

#include <map>
#include <vector>

#include "ipc/storage_daemon_provider.h"
#include "iservice_registry.h"
#include "securec.h"
#include "storage_daemon_proxy.h"
#include "system_ability_definition.h"

namespace OHOS {
using namespace std;

sptr<StorageDaemon::IStorageDaemon> GetStorageDaemonProxy()
{
    auto samgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        printf("samgr empty error\n");
        return nullptr;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(OHOS::STORAGE_MANAGER_DAEMON_ID);
    if (object == nullptr) {
        printf("storage daemon client samgr ablity empty error\n");
        return nullptr;
    }

    return iface_cast<StorageDaemon::IStorageDaemon>(object);
}

bool ShutdownFuzzTest(sptr<StorageDaemon::IStorageDaemon>& proxy, const uint8_t *data, size_t size)
{
    (void)data;
    proxy->Shutdown();
    proxy->InitGlobalKey();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    auto proxy = OHOS::GetStorageDaemonProxy();
    if (proxy != nullptr) {
        OHOS::ShutdownFuzzTest(proxy, data, size);
    } else {
        printf("daemon proxy is nullptr\n");
    }

    return 0;
}
