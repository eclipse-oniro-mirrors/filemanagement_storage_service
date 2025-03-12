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

#ifndef OHOS_STORAGE_MANAGER_FILE_SYSTEM_CRYPTO_H
#define OHOS_STORAGE_MANAGER_FILE_SYSTEM_CRYPTO_H

#include <singleton.h>

namespace OHOS {
namespace StorageManager {
class FileSystemCrypto final : public NoCopyable {
    DECLARE_DELAYED_SINGLETON(FileSystemCrypto);
public:
    int32_t GenerateUserKeys(uint32_t userId, uint32_t flags);
    int32_t DeleteUserKeys(uint32_t userId);
    int32_t UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                           const std::vector<uint8_t> &token,
                           const std::vector<uint8_t> &oldSecret,
                           const std::vector<uint8_t> &newSecret);
    int32_t UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                         const std::vector<uint8_t> &newSecret,
                                         uint64_t secureUid,
                                         uint32_t userId,
                                         std::vector<std::vector<uint8_t>> &plainText);
    int32_t ActiveUserKey(uint32_t userId,
                          const std::vector<uint8_t> &token,
                          const std::vector<uint8_t> &secret);
    int32_t InactiveUserKey(uint32_t userId);
    int32_t UpdateKeyContext(uint32_t userId, bool needRemoveTmpKey = false);
    int32_t LockUserScreen(uint32_t userId);
    int32_t UnlockUserScreen(uint32_t userId,
                             const std::vector<uint8_t> &token,
                             const std::vector<uint8_t> &secret);
    int32_t GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus);
    int32_t GenerateAppkey(uint32_t hashId, uint32_t userId, std::string &keyId);
    int32_t DeleteAppkey(const std::string keyId);
    int32_t GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount = false);
    int32_t GetUserNeedActiveStatus(uint32_t userId, bool &needActive);
    int32_t CreateRecoverKey(uint32_t userId,
                             uint32_t userType,
                             const std::vector<uint8_t> &token,
                             const std::vector<uint8_t> &secret);
    int32_t SetRecoverKey(const std::vector<uint8_t> &key);
private:
    int32_t CheckUserIdRange(int32_t userId);
};
} // StorageManager
} // OHOS
#endif // OHOS_STORAGE_MANAGER_FILE_SYSTEM_CRYPTO_H