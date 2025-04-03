/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "crypto/filesystem_crypto.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
namespace OHOS {
namespace StorageManager {
using namespace testing::ext;

class FileSystemCryptoTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_GenerateUserKeys_0000
 * @tc.name: Storage_manager_crypto_GenerateUserKeys_0000
 * @tc.desc: Test function of GenerateUserKeys interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_GenerateUserKeys_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_GenerateUserKeys_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 108;
    uint32_t flags = 2; // UserKeys type
    uint32_t result = fileSystemCrypto_->GenerateUserKeys(userId, flags);
    EXPECT_EQ(result, E_OK);

    fileSystemCrypto_->DeleteUserKeys(userId);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_GenerateUserKeys_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_GenerateUserKeys_0001
 * @tc.name: Storage_manager_crypto_GenerateUserKeys_0001
 * @tc.desc: Test function of GenerateUserKeys interface for Parameters ERROR which userId not in [101, 1099].
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_GenerateUserKeys_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_GenerateUserKeys_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 19999;
    uint32_t flags = 2; // UserKeys type
    uint32_t ret = fileSystemCrypto_->GenerateUserKeys(userId, flags);
    EXPECT_EQ(ret, E_USERID_RANGE);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_GenerateUserKeys_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_DeleteUserKeys_0000
 * @tc.name: Storage_manager_crypto_DeleteUserKeys_0000
 * @tc.desc: Test function of DeleteUserKeys interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_DeleteUserKeys_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_DeleteUserKeys_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 109;
    uint32_t flags = 2; // UserKeys type
    uint32_t result = fileSystemCrypto_->GenerateUserKeys(userId, flags);
    EXPECT_EQ(result, E_OK);

    fileSystemCrypto_->DeleteUserKeys(userId);
    EXPECT_EQ(result, E_OK);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_DeleteUserKeys_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_DeleteUserKeys_0001
 * @tc.name: Storage_manager_crypto_DeleteUserKeys_0001
 * @tc.desc: Test function of DeleteUserKeys interface for Parameters ERROR which userId not in [101, 1099].
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_DeleteUserKeys_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_DeleteUserKeys_0001";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 19999;
    uint32_t ret = fileSystemCrypto_->DeleteUserKeys(userId);
    EXPECT_EQ(ret, E_USERID_RANGE);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_DeleteUserKeys_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_UpdateUserAuth_0000
 * @tc.name: Storage_manager_crypto_UpdateUserAuth_0000
 * @tc.desc: Test function of UpdateUserAuth interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_UpdateUserAuth_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_UpdateUserAuth_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 110;
    uint32_t flags = 2; // UserKeys type
    uint32_t result = fileSystemCrypto_->GenerateUserKeys(userId, flags);
    EXPECT_EQ(result, E_OK);
    result = fileSystemCrypto_->UpdateUserAuth(userId, 0, {}, {}, {});
    EXPECT_EQ(result, E_OK);

    fileSystemCrypto_->DeleteUserKeys(userId);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_UpdateUserAuth_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_UpdateUserAuth_0001
 * @tc.name: Storage_manager_crypto_UpdateUserAuth_0001
 * @tc.desc: Test function of UpdateUserAuth interface for Parameters ERROR which userId not in [101, 1099].
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_UpdateUserAuth_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_UpdateUserAuth_0001";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 19999;
    uint32_t ret = fileSystemCrypto_->UpdateUserAuth(userId, 0, {}, {}, {});
    EXPECT_EQ(ret, E_USERID_RANGE);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_UpdateUserAuth_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_ActiveUserKey_0000
 * @tc.name: Storage_manager_crypto_ActiveUserKey_0000
 * @tc.desc: Test function of ActiveUserKey interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_ActiveUserKey_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_ActiveUserKey_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 111;
    uint32_t flags = 2; // UserKeys type
    uint32_t result = fileSystemCrypto_->GenerateUserKeys(userId, flags);
    EXPECT_EQ(result, E_OK);
    result = fileSystemCrypto_->ActiveUserKey(userId, {}, {});
    EXPECT_EQ(result, E_OK);

    fileSystemCrypto_->DeleteUserKeys(userId);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_ActiveUserKey_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_ActiveUserKey_0001
 * @tc.name: Storage_manager_crypto_ActiveUserKey_0001
 * @tc.desc: Test function of ActiveUserKey interface for Parameters ERROR which userId not in [101, 1099].
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_ActiveUserKey_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_ActiveUserKey_0001";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 19999;
    uint32_t ret = fileSystemCrypto_->ActiveUserKey(userId, {}, {});
    EXPECT_EQ(ret, E_USERID_RANGE);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_ActiveUserKey_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_InactiveUserKey_0000
 * @tc.name: Storage_manager_crypto_InactiveUserKey_0000
 * @tc.desc: Test function of InactiveUserKey interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_InactiveUserKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_InactiveUserKey_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t userId = 112;
    int32_t flags = 2; // UserKeys type
    int32_t ret = fileSystemCrypto_->GenerateUserKeys(userId, flags);
    ASSERT_EQ(ret, E_OK);

    ret = fileSystemCrypto_->ActiveUserKey(userId, {}, {});
    EXPECT_EQ(ret, E_OK);

    ret = fileSystemCrypto_->InactiveUserKey(userId);
    EXPECT_EQ(ret, E_OK);

    fileSystemCrypto_->DeleteUserKeys(userId);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_InactiveUserKey_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_LockUserScreen_0000
 * @tc.name: Storage_manager_crypto_LockUserScreen_0000
 * @tc.desc: Test function of LockUserScreen interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_LockUserScreen_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_LockUserScreen_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t userId = 100;

    int32_t ret = fileSystemCrypto_->LockUserScreen(userId);
    EXPECT_EQ(ret, E_OK);

    userId = 19999;
    ret = fileSystemCrypto_->LockUserScreen(userId);
    EXPECT_EQ(ret, E_USERID_RANGE);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_LockUserScreen_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_UnlockUserScreen_0000
 * @tc.name: Storage_manager_crypto_UnlockUserScreen_0000
 * @tc.desc: Test function of UnlockUserScreen interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_UnlockUserScreen_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_UnlockUserScreen_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t userId = 100;

    int32_t ret = fileSystemCrypto_->UnlockUserScreen(userId, {}, {});
    EXPECT_EQ(ret, E_OK);

    userId = 19999;
    ret = fileSystemCrypto_->UnlockUserScreen(userId, {}, {});
    EXPECT_EQ(ret, E_USERID_RANGE);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_UnlockUserScreen_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_CreateRecoverKey_0000
 * @tc.name: Storage_manager_crypto_CreateRecoverKey_0000
 * @tc.desc: Test function of CreateRecoverKey interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_CreateRecoverKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_CreateRecoverKey_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t userId = 100;
    int32_t userType = 10;

    int32_t ret = fileSystemCrypto_->CreateRecoverKey(userId, userType, {}, {});
    EXPECT_EQ(ret, E_OK);

    userId = 19999;
    ret = fileSystemCrypto_->CreateRecoverKey(userId, userType, {}, {});
    EXPECT_EQ(ret, E_USERID_RANGE);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_CreateRecoverKey_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_SetRecoverKey_0000
 * @tc.name: Storage_manager_crypto_SetRecoverKey_0000
 * @tc.desc: Test function of SetRecoverKey interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_SetRecoverKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_SetRecoverKey_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();

    int32_t ret = fileSystemCrypto_->SetRecoverKey({});
    EXPECT_EQ(ret, E_OK);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_SetRecoverKey_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_ResetSecretWithRecoveryKey_0000
 * @tc.name: Storage_manager_crypto_ResetSecretWithRecoveryKey_0000
 * @tc.desc: Test function of ResetSecretWithRecoveryKey interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_ResetSecretWithRecoveryKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_ResetSecretWithRecoveryKey_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();

    int32_t ret = fileSystemCrypto_->ResetSecretWithRecoveryKey(99, 1, {});
    EXPECT_EQ(ret, E_USERID_RANGE);

    ret = fileSystemCrypto_->ResetSecretWithRecoveryKey(100, 1, {});
    EXPECT_EQ(ret, E_OK);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_ResetSecretWithRecoveryKey_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_GetLockScreenStatus_0000
 * @tc.name: Storage_manager_crypto_GetLockScreenStatus_0000
 * @tc.desc: Test function of GetLockScreenStatus interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_GetLockScreenStatus_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_GetLockScreenStatus_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t userId = 100;
    bool lockScreenStatus = false;
    int32_t ret = fileSystemCrypto_->GetLockScreenStatus(userId, lockScreenStatus);
    EXPECT_EQ(ret, E_OK);

    userId = 19999;
    ret = fileSystemCrypto_->GetLockScreenStatus(userId, lockScreenStatus);
    EXPECT_EQ(ret, E_USERID_RANGE);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_GetLockScreenStatus_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_InactiveUserKey_0001
 * @tc.name: Storage_manager_crypto_InactiveUserKey_0001
 * @tc.desc: Test function of InactiveUserKey interface for Parameters ERROR which userId not in [101, 1099].
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_InactiveUserKey_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_InactiveUserKey_0001";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 19999;
    uint32_t ret = fileSystemCrypto_->InactiveUserKey(userId);
    EXPECT_EQ(ret, E_USERID_RANGE);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_InactiveUserKey_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_UpdateKeyContext_0000
 * @tc.name: Storage_manager_crypto_UpdateKeyContext_0000
 * @tc.desc: Test function of UpdateKeyContext interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_UpdateKeyContext_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_UpdateKeyContext_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    int32_t userId = 113;
    int32_t flags = 2; // UserKeys type
    int32_t ret = fileSystemCrypto_->GenerateUserKeys(userId, flags);
    ASSERT_EQ(ret, E_OK);

    ret = fileSystemCrypto_->UpdateUserAuth(userId, 0, {}, {}, {});
    EXPECT_EQ(ret, E_OK);

    ret = fileSystemCrypto_->UpdateKeyContext(userId);
    EXPECT_EQ(ret, E_OK);

    fileSystemCrypto_->DeleteUserKeys(userId);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_UpdateKeyContext_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_UpdateKeyContext_0001
 * @tc.name: Storage_manager_crypto_UpdateKeyContext_0001
 * @tc.desc: Test function of UpdateKeyContext interface for Parameters ERROR which userId not in [101, 1099].
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_UpdateKeyContext_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_UpdateKeyContext_0001";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 19999;
    uint32_t ret = fileSystemCrypto_->UpdateKeyContext(userId);
    EXPECT_EQ(ret, E_USERID_RANGE);

    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_UpdateKeyContext_0001";
}

/**
 * @tc.number: SUB_Storage_manager_crypto_GetFileEncryptStatus_0000
 * @tc.name: Storage_manager_crypto_GetFileEncryptStatus_0000
 * @tc.desc: Test function of GetFileEncryptStatus interface for Parameters ERROR which userId not in [101, 1099].
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_GetFileEncryptStatus_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_UpdateKeyContext_0001";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
        DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 99999;
    bool isEncrypted = true;
    uint32_t ret = fileSystemCrypto_->GetFileEncryptStatus(userId, isEncrypted);
    EXPECT_EQ(ret, E_USERID_RANGE);

    userId = 800;
    ret = fileSystemCrypto_->GetFileEncryptStatus(userId, isEncrypted);
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_GetFileEncryptStatus_0000";
}
/**
 * @tc.number: SUB_STORAGE_Storage_manager_crypto_GenerateAppkey_0000
 * @tc.name: Storage_manager_crypto_GenerateAppkey_0000
 * @tc.desc: Test function of GenerateAppkey interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_GenerateAppkey_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_GenerateAppkey_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
            DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = 108;
    uint32_t hashId = -1;
    std::string keyId = "keys"; // UserKeys type
    ASSERT_TRUE(fileSystemCrypto_ != nullptr);
    uint32_t result = fileSystemCrypto_->GenerateAppkey(hashId, userId, keyId);
    EXPECT_EQ(result, E_OK);

    fileSystemCrypto_->DeleteAppkey(keyId);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_GenerateAppkey_0000";
}

/**
 * @tc.number: Storage_manager_crypto_UpdateUseAuthWithRecoveryKey_0000
 * @tc.name: Storage_manager_crypto_UpdateUseAuthWithRecoveryKey_0000
 * @tc.desc: Test function of UpdateUseAuthWithRecoveryKey interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(FileSystemCryptoTest, Storage_manager_crypto_UpdateUseAuthWithRecoveryKey_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-start Storage_manager_crypto_UpdateUseAuthWithRecoveryKey_0000";
    std::shared_ptr<FileSystemCrypto> fileSystemCrypto_ =
            DelayedSingleton<FileSystemCrypto>::GetInstance();
    uint32_t userId = -1;
    std::vector<uint8_t> authToken;
    std::vector<uint8_t> newSecret;
    uint64_t secureUid = 0;
    std::vector<std::vector<uint8_t>> plainText;
    ASSERT_TRUE(fileSystemCrypto_ != nullptr);
    uint32_t result = fileSystemCrypto_->UpdateUseAuthWithRecoveryKey(authToken, newSecret, secureUid,
        userId, plainText);
    EXPECT_EQ(result, E_USERID_RANGE);

    userId = 800;
    result = fileSystemCrypto_->UpdateUseAuthWithRecoveryKey(authToken, newSecret, secureUid,
        userId, plainText);
    EXPECT_EQ(result, E_OK);
    GTEST_LOG_(INFO) << "FileSystemCryptoTest-end Storage_manager_crypto_UpdateUseAuthWithRecoveryKey_0000";
}
}
}