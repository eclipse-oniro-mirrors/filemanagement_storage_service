/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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
#include "key_manager.h"

#include <fcntl.h>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "directory_ex.h"

#include "base_key_mock.h"
#include "file_utils_mock.h"
#include "key_control_mock.h"
#include "fscrypt_control_mock.h"
#include "fscrypt_key_v1.h"
#include "recover_manager_mock.h"
#include "storage_service_errno.h"
#include "utils/file_utils.h"

using namespace std;
using namespace testing::ext;
using namespace testing;

namespace OHOS::StorageDaemon {
class KeyMgrWithFileMockAnotherTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static inline shared_ptr<RecoveryMgrMock> recoveryMgrMock_ = nullptr;
    static inline shared_ptr<BaseKeyMoc> baseKeyMock_ = nullptr;
    static inline shared_ptr<KeyControlMoc> keyControlMock_ = nullptr;
    static inline shared_ptr<FscryptControlMoc> fscryptControlMock_ = nullptr;
    static inline shared_ptr<FileUtilMoc> fileUtilMoc_ = nullptr;
};

void KeyMgrWithFileMockAnotherTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase Start";
}

void KeyMgrWithFileMockAnotherTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase Start";
}

void KeyMgrWithFileMockAnotherTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp Start";
    recoveryMgrMock_ = make_shared<RecoveryMgrMock>();
    RecoveryMgrMock::recoveryMgrMock = recoveryMgrMock_;
    baseKeyMock_ = make_shared<BaseKeyMoc>();
    BaseKeyMoc::baseKeyMoc = baseKeyMock_;
    fscryptControlMock_ = make_shared<FscryptControlMoc>();
    FscryptControlMoc::fscryptControlMoc = fscryptControlMock_;
    keyControlMock_ = make_shared<KeyControlMoc>();
    KeyControlMoc::keyControlMoc = keyControlMock_;
    fileUtilMoc_ = make_shared<FileUtilMoc>();
    FileUtilMoc::fileUtilMoc = fileUtilMoc_;
}

void KeyMgrWithFileMockAnotherTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown Start";
    RecoveryMgrMock::recoveryMgrMock = nullptr;
    recoveryMgrMock_ = nullptr;
    BaseKeyMoc::baseKeyMoc = nullptr;
    baseKeyMock_ = nullptr;
    FscryptControlMoc::fscryptControlMoc = nullptr;
    fscryptControlMock_ = nullptr;
    KeyControlMoc::keyControlMoc = nullptr;
    keyControlMock_ = nullptr;
    FileUtilMoc::fileUtilMoc = nullptr;
    fileUtilMoc_ = nullptr;
}

#ifdef RECOVER_KEY_TEE_ENVIRONMENT
/**
 * @tc.name: KeyManager_ResetSecretWithRecoveryKey_000
 * @tc.desc: Verify the ResetSecretWithRecoveryKey function.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_ResetSecretWithRecoveryKey_000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_ResetSecretWithRecoveryKey_000 Start";
    uint32_t userId = 1112;
    uint32_t rkType = 0;
    std::vector<uint8_t> key;
    rkType = 6;
    EXPECT_CALL(*recoveryMgrMock_, IsEncryptionEnabled()).WillOnce(Return(true));
    EXPECT_CALL(*fscryptControlMock_, GetFscryptVersionFromPolicy()).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2));
    EXPECT_CALL(*keyControlMock_, KeyCtrlGetFscryptVersion(_)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2));
    EXPECT_CALL(*recoveryMgrMock_, ResetSecretWithRecoveryKey()).WillOnce(Return(E_OK));
    EXPECT_CALL(*baseKeyMock_, StoreKey(_, _)).Times(6).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, IsDir(_)).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillOnce(Return(1));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().ResetSecretWithRecoveryKey(userId, rkType, key), E_UMOUNT_FBE);

    EXPECT_CALL(*fscryptControlMock_, GetFscryptVersionFromPolicy()).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2));
    EXPECT_CALL(*keyControlMock_, KeyCtrlGetFscryptVersion(_)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2));
    EXPECT_CALL(*recoveryMgrMock_, ResetSecretWithRecoveryKey()).WillOnce(Return(E_OK));
    EXPECT_CALL(*baseKeyMock_, StoreKey(_, _)).Times(6).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, IsDir(_)).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillOnce(Return(0));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _)).WillOnce(Return(1));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().ResetSecretWithRecoveryKey(userId, rkType, key), E_MOUNT_FBE);

    EXPECT_CALL(*fscryptControlMock_, GetFscryptVersionFromPolicy()).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2));
    EXPECT_CALL(*keyControlMock_, KeyCtrlGetFscryptVersion(_)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2))
        .WillOnce(Return(FSCRYPT_V2)).WillOnce(Return(FSCRYPT_V2));
    EXPECT_CALL(*recoveryMgrMock_, ResetSecretWithRecoveryKey()).WillOnce(Return(E_OK));
    EXPECT_CALL(*baseKeyMock_, StoreKey(_, _)).Times(6).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, IsDir(_)).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillOnce(Return(0));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _)).WillOnce(Return(0));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().ResetSecretWithRecoveryKey(userId, rkType, key), E_OK);

    GTEST_LOG_(INFO) << "KeyManager_ResetSecretWithRecoveryKey_000 end";
}
#endif

#ifdef PC_ENCRYPTION_EXTENSION
/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_000
 * @tc.desc: Verify SetRecoverKey with needMountPath=false returns E_OK via guard clause.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_000 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, false), E_OK);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_000 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_001
 * @tc.desc: Verify SetRecoverKey with needMountPath=true returns error when RecoveryManager fails.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_001 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(-EFAULT));
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_SET_RECOVERY_KEY_ERR);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_001 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_002
 * @tc.desc: Verify SetRecoverKey returns E_UMOUNT_RECOVER_KEY when UMount fails at first pair.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_002 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillOnce(Return(1));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_UMOUNT_RECOVER_KEY);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_002 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_003
 * @tc.desc: Verify SetRecoverKey returns E_MOUNT_RECOVER_KEY when Mount fails at first pair.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_003 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillOnce(Return(0));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _)).WillOnce(Return(1));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_MOUNT_RECOVER_KEY);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_003 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_004
 * @tc.desc: Verify SetRecoverKey returns E_OK when all 5 mount pairs succeed.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_004 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_OK);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_004 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_005
 * @tc.desc: Verify SetRecoverKey returns E_UMOUNT_RECOVER_KEY when UMount fails at last pair.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_005 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_))
        .WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(1));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _)).WillRepeatedly(Return(0));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_UMOUNT_RECOVER_KEY);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_005 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_006
 * @tc.desc: Verify SetRecoverKey returns E_MOUNT_RECOVER_KEY when Mount fails at last pair.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_006 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _))
        .WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(1));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_MOUNT_RECOVER_KEY);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_006 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_007
 * @tc.desc: Verify SetRecoverKey returns E_OK when UMount fails with tolerated errno (ENOENT).
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_007 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_))
        .WillRepeatedly(Invoke([](const std::string &) -> int32_t { errno = ENOENT; return 1; }));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_OK);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_007 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_008
 * @tc.desc: Verify SetRecoverKey returns E_OK when Mount fails with tolerated errno (EEXIST).
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_008 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _))
        .WillRepeatedly(Invoke([](const std::string &, const std::string &, const char *,
            unsigned long, const void *) -> int32_t { errno = EEXIST; return 1; }));
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_OK);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_008 end";
}

/**
 * @tc.name: KeyManager_SetRecoverKey_NeedMountPath_009
 * @tc.desc: Verify SetRecoverKey returns E_MOUNT_RECOVER_KEY when Mount fails with EBUSY after UMount.
 * @tc.type: FUNC
 * @tc.require: IAHHWW
 */
HWTEST_F(KeyMgrWithFileMockAnotherTest, KeyManager_SetRecoverKey_NeedMountPath_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_009 Start";
    std::vector<uint8_t> key;
    EXPECT_CALL(*recoveryMgrMock_, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(*fileUtilMoc_, UMount(_)).WillOnce(Return(0));
    EXPECT_CALL(*fileUtilMoc_, Mount(_, _, _, _, _))
        .WillOnce(Invoke([](const std::string &, const std::string &, const char *,
            unsigned long, const void *) -> int32_t { errno = EBUSY; return 1; }));
    errno = 0;
    EXPECT_EQ(KeyManager::GetInstance().SetRecoverKey(key, true), E_MOUNT_RECOVER_KEY);
    GTEST_LOG_(INFO) << "KeyManager_SetRecoverKey_NeedMountPath_009 end";
}
#endif
}
