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

#include "key_crypto_utils.h"

#include "os_account_manager.h"
#ifdef ENABLE_SCREENLOCK_MANAGER
#include "screenlock_manager.h"
#endif
#include "storage_service_log.h"

namespace OHOS {
namespace StorageService {
void KeyCryptoUtils::ForceLockUserScreen()
{
    LOGI("KeyCryptoUtils::ForceLockUserScreen");
#ifdef ENABLE_SCREENLOCK_MANAGER
    std::vector<int32_t> ids;
    int32_t ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(ids);
    if (ret != ERR_OK || ids.empty()) {
        LOGE("Query active userid failed, ret = %{public}u", ret);
        return;
    }
    int reasonFlag = static_cast<int>(ScreenLock::StrongAuthReasonFlags::ACTIVE_REQUEST);
    ret = ScreenLock::ScreenLockManager::GetInstance()->RequestStrongAuth(reasonFlag, ids[0]);
    if (ret != ScreenLock::E_SCREENLOCK_OK) {
        LOGE("Request strong auth by screen lock manager failed.");
        return;
    }
    ret = ScreenLock::ScreenLockManager::GetInstance()->Lock(ids[0]);
    if (ret != ScreenLock::E_SCREENLOCK_OK) {
        LOGE("Lock user screen by screen lock manager failed.");
    }
    LOGI("Force lock user screen and request strong auth success for userId = %{public}d.", ids[0]);
#endif
}
}
}