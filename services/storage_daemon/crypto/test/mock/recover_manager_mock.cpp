/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "recover_manager_mock.h"

using namespace std;
using namespace OHOS::StorageDaemon;

RecoveryManager::RecoveryManager()
{
}

RecoveryManager::~RecoveryManager()
{
}

int RecoveryManager::CreateRecoverKey(uint32_t userId,
                                      uint32_t userType,
                                      const std::vector<uint8_t> &token,
                                      const std::vector<uint8_t> &secret,
                                      const std::vector<KeyBlob> &originIv)
{
    if (IRecoveryManager::recoveryMgrMock == nullptr) {
        return true;
    }
    return IRecoveryManager::recoveryMgrMock->CreateRecoverKey(userId, userType, token, secret, originIv);
}

int RecoveryManager::SetRecoverKey(const std::vector<uint8_t> &key)
{
    if (IRecoveryManager::recoveryMgrMock == nullptr) {
        return true;
    }
    return IRecoveryManager::recoveryMgrMock->SetRecoverKey(key);
}

int32_t RecoveryManager::ResetSecretWithRecoveryKey(uint32_t userId, uint32_t rkType,
    const std::vector<uint8_t> &key, std::vector<KeyBlob> &originIvs)
{
    if (IRecoveryManager::recoveryMgrMock == nullptr) {
        return -1;
    }
    for (uint32_t i = 0; i < rkType; i++) {
        originIvs.push_back(KeyBlob());
    }
    return IRecoveryManager::recoveryMgrMock->ResetSecretWithRecoveryKey();
}

bool RecoveryManager::IsEncryptionEnabled()
{
    if (IRecoveryManager::recoveryMgrMock == nullptr) {
        return -1;
    }
    return IRecoveryManager::recoveryMgrMock->IsEncryptionEnabled();
}
