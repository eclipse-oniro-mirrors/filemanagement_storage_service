/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "directory_ex.h"
#include "fbex.h"
#include "file_ex.h"
#include "fscrypt_key_v2.h"
#include "key_blob.h"
#include "libfscrypt/fscrypt_control.h"
#include "storage_service_errno.h"

using namespace testing::ext;

namespace {
const std::string TEST_MNT = "/data";
const std::string TEST_DIR_LEGACY = "/data/test/crypto_dir_legacy";
const std::string TEST_DIR_V2 = "/data/test/crypto_dir";
const std::string TEST_KEYPATH = "/data/test/key/el2/80";

OHOS::StorageDaemon::FscryptKeyV2 g_testKeyV2 {TEST_KEYPATH};
}

namespace OHOS::StorageDaemon {
class FscryptKeyV2Test : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    UserAuth emptyUserAuth {};
};

void FscryptKeyV2Test::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void FscryptKeyV2Test::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void FscryptKeyV2Test::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void FscryptKeyV2Test::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: fscrypt_key_v2_active_support
 * @tc.desc: Verify the fscrypt V2 active function.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_active_support, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_active_support start";
    uint32_t flag = 1;
    g_testKeyV2.ClearKey();
    EXPECT_FALSE(g_testKeyV2.ActiveKey(flag, TEST_MNT));

    g_testKeyV2.keyInfo_.key.Alloc(FSCRYPT_MAX_KEY_SIZE + 1);
    EXPECT_FALSE(g_testKeyV2.ActiveKey(flag, TEST_MNT));

    g_testKeyV2.ClearKey();
    std::string emptyStr;
    g_testKeyV2.keyInfo_.key.Alloc(FSCRYPT_MAX_KEY_SIZE);
    EXPECT_FALSE(g_testKeyV2.ActiveKey(flag, emptyStr));

    g_testKeyV2.ClearKey();
    g_testKeyV2.keyInfo_.keyId.Clear();
    g_testKeyV2.keyInfo_.key.Alloc(FSCRYPT_MAX_KEY_SIZE);
    g_testKeyV2.keyInfo_.keyId.Alloc(FSCRYPT_KEY_IDENTIFIER_SIZE + 1);
    EXPECT_FALSE(g_testKeyV2.ActiveKey(flag, TEST_MNT));

    g_testKeyV2.ClearKey();
    g_testKeyV2.keyInfo_.keyId.Clear();
    g_testKeyV2.keyInfo_.key.Alloc(FSCRYPT_MAX_KEY_SIZE);
    g_testKeyV2.keyInfo_.keyId.Alloc(FSCRYPT_KEY_IDENTIFIER_SIZE);
    EXPECT_FALSE(g_testKeyV2.ActiveKey(flag, TEST_MNT));
    GTEST_LOG_(INFO) << "fscrypt_key_v2_active_support end";
}

/**
 * @tc.name: fscrypt_key_v2_policy_inactive_support
 * @tc.desc: Verify the fscryptV2 InactiveKey function.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_policy_inactive_support, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_policy_inactive_support start";
    uint32_t flag = 1;
    
    g_testKeyV2.keyInfo_.keyId.Clear();
    g_testKeyV2.keyInfo_.keyId.Alloc(FSCRYPT_KEY_IDENTIFIER_SIZE + 1);
    EXPECT_FALSE(g_testKeyV2.InactiveKey(flag, TEST_MNT));

    g_testKeyV2.keyInfo_.keyId.Clear();
    g_testKeyV2.keyInfo_.keyId.Alloc(FSCRYPT_KEY_IDENTIFIER_SIZE);
    EXPECT_FALSE(g_testKeyV2.InactiveKey(flag, TEST_MNT));
    g_testKeyV2.ClearKey();
    g_testKeyV2.keyInfo_.keyId.Clear();
    GTEST_LOG_(INFO) << "fscrypt_key_v2_policy_inactive_support end";
}

/**
 * @tc.name: fscrypt_key_v2_LockUserScreen_UnLockUserScreen
 * @tc.desc: Verify the fscrypt V2 LockUserScreen/UnLockUserScreen.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_LockUserScreen_UnLockUserScreen, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LockUserScreen_UnLockUserScreen start";
    uint32_t flag = 1;
    uint32_t sdpClass = 1;
    EXPECT_TRUE(g_testKeyV2.LockUserScreen(flag, sdpClass, TEST_MNT));
    EXPECT_TRUE(g_testKeyV2.UnlockUserScreen(flag, sdpClass, TEST_MNT));
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LockUserScreen_UnLockUserScreen end";
}

/**
 * @tc.name: fscrypt_key_v2_LockUece
 * @tc.desc: Verify the fscrypt V2 LockUece.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_LockUece, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LockUece start";
    bool isFbeSupport = true;
    EXPECT_TRUE(g_testKeyV2.LockUece(isFbeSupport));
    EXPECT_FALSE(isFbeSupport);
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LockUece end";
}

/**
 * @tc.name: fscrypt_key_v2_GenerateAppkey_DeleteAppkey
 * @tc.desc: Verify the fscrypt V2 GenerateAppkey/DeleteAppkey.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_GenerateAppkey_DeleteAppkey, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_GenerateAppkey_DeleteAppkey start";
    uint32_t userId = 1;
    uint32_t appUid = 1;
    std::string TEST_KEYID = "1234";
    EXPECT_FALSE(g_testKeyV2.GenerateAppkey(userId, appUid, TEST_KEYID));
    EXPECT_FALSE(g_testKeyV2.DeleteAppkey(TEST_KEYID));
    GTEST_LOG_(INFO) << "fscrypt_key_v2_GenerateAppkey_DeleteAppkey end";
}

/**
 * @tc.name: fscrypt_key_v2_AddClassE_ChangePinCodeClassE
 * @tc.desc: Verify the fscrypt V2 AddClassE/ChangePinCodeClassE.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_AddClassE_ChangePinCodeClassE, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_AddClassE_ChangePinCodeClassE start";
    bool isNeedEncryptClassE = true;
    bool isSupport = true;
    EXPECT_TRUE(g_testKeyV2.AddClassE(isNeedEncryptClassE, isSupport));
    bool isFbeSupport = true;
    EXPECT_TRUE(g_testKeyV2.ChangePinCodeClassE(isFbeSupport));
    EXPECT_FALSE(isFbeSupport);
    GTEST_LOG_(INFO) << "fscrypt_key_v2_AddClassE_ChangePinCodeClassE end";
}

/**
 * @tc.name: fscrypt_key_v2_DecryptClassE_EncryptClassE_DeleteClassEPinCode
 * @tc.desc: Verify the fscrypt V2 DecryptClassE/EncryptClassE/DeleteClassEPinCode.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_DecryptClassE_EncryptClassE_DeleteClassEPinCode, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_DecryptClassE_EncryptClassE_DeleteClassEPinCode start";
    bool isSupport = true;
    EXPECT_TRUE(g_testKeyV2.DecryptClassE(emptyUserAuth, isSupport));
    EXPECT_FALSE(isSupport);
    isSupport = true;
    EXPECT_TRUE(g_testKeyV2.EncryptClassE(emptyUserAuth, isSupport));
    EXPECT_FALSE(isSupport);
    uint32_t user = 1;
    EXPECT_TRUE(g_testKeyV2.DeleteClassEPinCode(user));
    GTEST_LOG_(INFO) << "fscrypt_key_v2_DecryptClassE_EncryptClassE_DeleteClassEPinCode end";
}

/**
 * @tc.name: fscrypt_key_v2_LoadAndSetPolicy
 * @tc.desc: Verify the fscrypt V2 LoadAndSetPolicy
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_LoadAndSetPolicy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LoadAndSetPolicy start";
    OHOS::ForceRemoveDirectory(TEST_DIR_V2);
    OHOS::ForceCreateDirectory(TEST_DIR_V2);
    OHOS::ForceRemoveDirectory(TEST_DIR_LEGACY);
    EXPECT_TRUE(OHOS::ForceCreateDirectory(TEST_DIR_LEGACY));
    std::string testVersionFile = TEST_DIR_LEGACY + "/fscrypt_version";
    EXPECT_TRUE(OHOS::SaveStringToFile(testVersionFile, "2\n"));
    EXPECT_NE(0, LoadAndSetPolicy(TEST_DIR_LEGACY.c_str(), TEST_DIR_V2.c_str()));
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LoadAndSetPolicy end";
}

/**
 * @tc.name: fscrypt_key_v2_LoadAndSetEceAndSecePolicy
 * @tc.desc: Verify the fscrypt V2 LoadAndSetEceAndSecePolicy
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(FscryptKeyV2Test, fscrypt_key_v2_LoadAndSetEceAndSecePolicy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LoadAndSetPolicy start";
    int type = 1;
    const char* dir = TEST_MNT.c_str();
    EXPECT_EQ(LoadAndSetEceAndSecePolicy(nullptr, nullptr, type), -EINVAL);
    EXPECT_EQ(LoadAndSetEceAndSecePolicy(TEST_DIR_LEGACY.c_str(), nullptr, type), -EINVAL);
    EXPECT_EQ(LoadAndSetEceAndSecePolicy(nullptr, dir, type), -EINVAL);
    EXPECT_EQ(LoadAndSetEceAndSecePolicy(TEST_DIR_LEGACY.c_str(), dir, type), 0);

    OHOS::ForceRemoveDirectory(TEST_DIR_LEGACY);
    EXPECT_TRUE(OHOS::ForceCreateDirectory(TEST_DIR_LEGACY));
    std::string testVersionFile = TEST_DIR_LEGACY + "/fscrypt_version";
    EXPECT_TRUE(OHOS::SaveStringToFile(testVersionFile, "1\n"));
    EXPECT_EQ(LoadAndSetEceAndSecePolicy(TEST_DIR_LEGACY.c_str(), dir, type), 0);
    EXPECT_EQ(type, 1);

    type = 3;
    EXPECT_NE(LoadAndSetEceAndSecePolicy(TEST_DIR_LEGACY.c_str(), dir, type), 0);

    type = 4;
    EXPECT_NE(LoadAndSetEceAndSecePolicy(TEST_DIR_LEGACY.c_str(), dir, type), 0);

    OHOS::ForceRemoveDirectory(TEST_DIR_LEGACY);
    EXPECT_TRUE(OHOS::ForceCreateDirectory(TEST_DIR_LEGACY));
    testVersionFile = TEST_DIR_LEGACY + "/fscrypt_version";
    EXPECT_TRUE(OHOS::SaveStringToFile(testVersionFile, "2\n"));
    EXPECT_EQ(LoadAndSetEceAndSecePolicy(TEST_DIR_LEGACY.c_str(), dir, type), 0);
    GTEST_LOG_(INFO) << "fscrypt_key_v2_LoadAndSetEceAndSecePolicy end";
}
} // OHOS::StorageDaemon