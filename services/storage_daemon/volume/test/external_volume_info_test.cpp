/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <linux/kdev_t.h>

#include "external_volume_info_mock.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "mock/file_utils_mock.h"

namespace OHOS {
namespace StorageDaemon {
using namespace testing::ext;

class ExternalVolumeInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    ExternalVolumeInfo* externalVolumeInfo_;
    static inline std::shared_ptr<FileUtilMoc> fileUtilMoc_ = nullptr;
};

void ExternalVolumeInfoTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "ExternalVolumeInfoTest SetUpTestCase";
    fileUtilMoc_ = std::make_shared<FileUtilMoc>();
    FileUtilMoc::fileUtilMoc = fileUtilMoc_;
}

void ExternalVolumeInfoTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "ExternalVolumeInfoTest TearDownTestCase";
    FileUtilMoc::fileUtilMoc = nullptr;
    fileUtilMoc_ = nullptr;
}

void ExternalVolumeInfoTest::SetUp()
{
    externalVolumeInfo_ = new ExternalVolumeInfo();
}

void ExternalVolumeInfoTest::TearDown(void)
{
    if (externalVolumeInfo_ != nullptr) {
        delete externalVolumeInfo_;
        externalVolumeInfo_ = nullptr;
    }
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoCreate_001
 * @tc.desc: Verify the DoCreate function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoCreate_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoCreate_001 start";

    ExternalVolumeInfo vol;
    dev_t device = MKDEV(156, 300);
    std::string diskId = "disk-156-300";
    std::string volId = "vol-156-301";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);
    EXPECT_EQ(ret, E_OK);
    ret = vol.Destroy();

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoCreate_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoDestroy_001
 * @tc.desc: Verify the DoDestroy function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoDestroy_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoDestroy_001 start";

    ExternalVolumeInfo vol;
    dev_t device = MKDEV(156, 400);
    std::string diskId = "disk-156-400";
    std::string volId = "vol-156-401";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);
    EXPECT_EQ(ret, E_OK);
    ret = vol.Destroy();
    EXPECT_EQ(ret, E_OK);

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoDestroy_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoMount_001
 * @tc.desc: Verify the DoMount function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoMount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount_001 start";

    ExternalVolumeInfoMock mock;
    uint32_t mountFlags = 0;

    EXPECT_CALL(mock, DoMount(testing::_)).Times(1).WillOnce(testing::Return(E_VOL_MOUNT_ERR));
    auto ret = mock.DoMount(mountFlags);
    EXPECT_TRUE(ret == E_VOL_MOUNT_ERR);

    EXPECT_CALL(mock, DoMount(testing::_)).Times(1).WillOnce(testing::Return(E_OK));
    ret = mock.DoMount(mountFlags);
    EXPECT_TRUE(ret == E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoUMount_001
 * @tc.desc: Verify the DoUMount function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoUMount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoUMount_001 start";

    ExternalVolumeInfoMock mock;
    bool force = true;

    EXPECT_CALL(mock, DoUMount(testing::_)).Times(1).WillOnce(testing::Return(E_VOL_UMOUNT_ERR));
    auto ret = mock.DoUMount(force);
    EXPECT_TRUE(ret == E_VOL_UMOUNT_ERR);

    EXPECT_CALL(mock, DoUMount(testing::_)).Times(1).WillOnce(testing::Return(E_SYS_KERNEL_ERR));
    ret = mock.DoUMount(force);
    EXPECT_TRUE(ret == E_SYS_KERNEL_ERR);

    EXPECT_CALL(mock, DoUMount(testing::_)).Times(1).WillOnce(testing::Return(E_OK));
    ret = mock.DoUMount(force);
    EXPECT_TRUE(ret == E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoUMount_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoTryToFix_001
 * @tc.desc: Verify the DoTryToFix function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoTryToFix_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoTryToFix_001 start";

    ExternalVolumeInfoMock mock;
    ExternalVolumeInfo vol;

    auto ret = vol.DoTryToFix();
    EXPECT_EQ(ret, E_DOCHECK_MOUNT);

    mock.fsType_ = "ntfs";
    EXPECT_CALL(mock, DoFix4Ntfs()).Times(1).WillOnce(testing::Return(E_VOL_FIX_FAILED));
    ret = mock.DoFix4Ntfs();
    EXPECT_EQ(ret, E_VOL_FIX_FAILED);
    ret = mock.DoTryToFix();
    EXPECT_EQ(ret, E_OK);

    mock.fsType_ = "exfat";
    EXPECT_CALL(mock, DoFix4Exfat()).Times(1).WillOnce(testing::Return(E_VOL_FIX_FAILED));
    ret = mock.DoFix4Exfat();
    EXPECT_EQ(ret, E_VOL_FIX_FAILED);
    EXPECT_CALL(mock, DoFix4Exfat()).Times(1).WillOnce(testing::Return((E_OK)));
    ret = mock.DoFix4Exfat();
    EXPECT_TRUE(ret == (E_OK));

    
    mock.fsType_ = "vfat";
    EXPECT_CALL(mock, DoTryToFix()).Times(1).WillOnce(testing::Return(E_VOL_FIX_NOT_SUPPORT));
    ret = mock.DoTryToFix();
    EXPECT_EQ(ret, E_VOL_FIX_NOT_SUPPORT);

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoTryToFix_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoTryToFix_002
 * @tc.desc: Verify the DoTryToFix function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoTryToFix_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoTryToFix_002 start";
    ExternalVolumeInfo vol;
    vol.fsType_ = "ntfs";
    dev_t device = MKDEV(156, 600);
    std::string diskId = "disk-156-600";
    std::string volId = "vol-156-601";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);

    EXPECT_EQ(ret, E_OK);
    ret = vol.Check();
    EXPECT_EQ(ret, E_CHECK);
    ret = vol.Destroy();
    EXPECT_EQ(ret, E_OK);

    ExternalVolumeInfoMock mock;

    EXPECT_CALL(mock, DoTryToFix()).Times(1).WillOnce(testing::Return(E_OK));
    ret = mock.DoTryToFix();
    EXPECT_TRUE(ret == E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoTryToFix_002 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoTryToFix_003
 * @tc.desc: Verify the DoTryToFix function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoTryToFix_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoTryToFix_003 start";
    ExternalVolumeInfo vol;
    vol.fsType_ = "fat32";

    int32_t ret = vol.DoTryToFix();
    EXPECT_EQ(ret, E_DOCHECK_MOUNT);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoTryToFix_003 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoCheck_001
 * @tc.desc: Verify the DoCheck function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoCheck_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoCheck_001 start";

    ExternalVolumeInfo vol;
    dev_t device = MKDEV(156, 600);
    std::string diskId = "disk-156-600";
    std::string volId = "vol-156-601";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);
    EXPECT_EQ(ret, E_OK);
    ret = vol.Check();
    EXPECT_EQ(ret, E_CHECK);
    ret = vol.Destroy();
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoCheck_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoFormat_001
 * @tc.desc: Verify the DoFormat function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoFormat_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_001 start";

    ExternalVolumeInfo vol;
    dev_t device = MKDEV(156, 700);
    std::string diskId = "disk-156-700";
    std::string volId = "vol-156-701";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);
    EXPECT_EQ(ret, E_OK);
    std::string flag = "exfat";
    ret = vol.Format(flag);
    EXPECT_EQ(ret, E_OK);
    ret = vol.Destroy();
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoFormat_002
 * @tc.desc: Verify the DoFormat function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoFormat_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_002 start";

    ExternalVolumeInfo vol;
    dev_t device = MKDEV(156, 701);
    std::string diskId = "disk-156-701";
    std::string volId = "vol-156-702";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);
    EXPECT_EQ(ret, E_OK);
    std::string flag = "vfat";
    ret = vol.Format(flag);
    EXPECT_EQ(ret, E_OK);
    ret = vol.Destroy();
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_002 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoFormat_003
 * @tc.desc: Verify the DoFormat function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoFormat_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_003 start";

    ExternalVolumeInfo vol;
    dev_t device = MKDEV(156, 702);
    std::string diskId = "disk-156-702";
    std::string volId = "vol-156-703";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);
    EXPECT_EQ(ret, E_OK);
    std::string flag = "ntfs";
    ret = vol.Format(flag);
    EXPECT_EQ(ret, E_NOT_SUPPORT);
    ret = vol.Destroy();
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_003 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoFormat_004
 * @tc.desc: Verify the DoFormat function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoFormat_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_004 start";

    ExternalVolumeInfo vol;
    EXPECT_CALL(*fileUtilMoc_, IsFuse()).WillOnce(testing::Return(true));
    EXPECT_CALL(*fileUtilMoc_, IsPathMounted(testing::_)).WillOnce(testing::Return(true));
    auto ret = vol.DoFormat("exfat");
    EXPECT_EQ(ret, E_RMDIR_MOUNT);
}

 /**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoFormat_005
 * @tc.desc: Verify the DoFormat function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoFormat_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_005 start";

    ExternalVolumeInfo vol;
    EXPECT_CALL(*fileUtilMoc_, IsFuse()).WillOnce(testing::Return(true));
    EXPECT_CALL(*fileUtilMoc_, IsPathMounted(testing::_)).WillOnce(testing::Return(false));
    auto ret = vol.DoFormat("exfat");
    EXPECT_EQ(ret, E_OK);
}

 /**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoFormat_006
 * @tc.desc: Verify the DoFormat function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoFormat_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_006 start";

    ExternalVolumeInfo vol;
    EXPECT_CALL(*fileUtilMoc_, IsFuse()).WillOnce(testing::Return(false));
    EXPECT_CALL(*fileUtilMoc_, IsPathMounted(testing::_)).WillOnce(testing::Return(true));
    auto ret = vol.DoFormat("exfat");
    EXPECT_EQ(ret, E_OK);
}

  /**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoFormat_007
 * @tc.desc: Verify the DoFormat function.
 * @tc.type: FUNC
 * @tc.require: SR000GGUOT
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoFormat_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoFormat_007 start";

    ExternalVolumeInfo vol;
    EXPECT_CALL(*fileUtilMoc_, IsFuse()).WillOnce(testing::Return(false));
    EXPECT_CALL(*fileUtilMoc_, IsPathMounted(testing::_)).WillOnce(testing::Return(false));
    auto ret = vol.DoFormat("exfat");
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoSetVolDesc_001
 * @tc.desc: Verify the DoSetVolDesc function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoSetVolDesc_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoSetVolDesc_001 start";

    ExternalVolumeInfo vol;
    dev_t device = MKDEV(156, 800);
    std::string diskId = "disk-156-800";
    std::string volId = "vol-156-801";
    bool isUserdata = false;
    int32_t ret = vol.Create(volId, diskId, device, isUserdata);
    EXPECT_EQ(ret, E_OK);
    std::string des = "label1";
    ret = vol.SetVolumeDescription(des);
    EXPECT_EQ(ret, E_NOT_SUPPORT);
    ret = vol.Destroy();
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoSetVolDesc_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_GetFsType_001
 * @tc.desc: Verify the GetFsType function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_GetFsType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetFsType_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    std::string ret = externalVolumeInfo_->GetFsType();
    GTEST_LOG_(INFO) << ret;
    EXPECT_TRUE(ret.empty());

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetFsType_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_GetFsUuid_001
 * @tc.desc: Verify the GetFsUuid function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_GetFsUuid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetFsUuid_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    std::string ret = externalVolumeInfo_->GetFsUuid();
    GTEST_LOG_(INFO) << ret;
    EXPECT_TRUE(ret.empty());

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetFsUuid_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_GetFsLabel_001
 * @tc.desc: Verify the GetFsLabel function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_GetFsLabel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetFsLabel_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    std::string ret = externalVolumeInfo_->GetFsLabel();
    GTEST_LOG_(INFO) << ret;
    EXPECT_TRUE(ret.empty());

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetFsLabel_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_GetMountPath_001
 * @tc.desc: Verify the GetMountPath function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_GetMountPath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetMountPath_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    std::string ret = externalVolumeInfo_->GetMountPath();
    GTEST_LOG_(INFO) << ret;
    EXPECT_TRUE(ret.empty());

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_GetMountPath_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoMount4Ext_001
 * @tc.desc: Verify the DoMount4Ext function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoMount4Ext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4Ext_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    uint32_t mountFlags = 0;
    int32_t ret = externalVolumeInfo_->DoMount4Ext(mountFlags);
    GTEST_LOG_(INFO) << ret;
    EXPECT_EQ(ret, E_EXT_MOUNT);

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4Ext_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoMount4Ntfs_001
 * @tc.desc: Verify the DoMount4Ntfs function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoMount4Ntfs_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4Ntfs_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    uint32_t mountFlags = 0;
    int32_t ret = externalVolumeInfo_->DoMount4Ntfs(mountFlags);
    GTEST_LOG_(INFO) << ret;
    EXPECT_EQ(ret, E_OK);

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4Ntfs_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoMount4Exfat_001
 * @tc.desc: Verify the DoMount4Exfat function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoMount4Exfat_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4Exfat_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    uint32_t mountFlags = 0;
    int32_t ret = externalVolumeInfo_->DoMount4Exfat(mountFlags);
    GTEST_LOG_(INFO) << ret;
    EXPECT_EQ(ret, E_OK);

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4Exfat_001 end";
}

/**
 * @tc.name: Storage_Service_ExternalVolumeInfoTest_DoMount4OtherType_001
 * @tc.desc: Verify the DoMount4OtherType function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(ExternalVolumeInfoTest, Storage_Service_ExternalVolumeInfoTest_DoMount4OtherType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4OtherType_001 start";

    ASSERT_TRUE(externalVolumeInfo_ != nullptr);
    uint32_t mountFlags = 0;
    int32_t ret = externalVolumeInfo_->DoMount4OtherType(mountFlags);
    GTEST_LOG_(INFO) << ret;
    EXPECT_EQ(ret, E_OTHER_MOUNT);

    GTEST_LOG_(INFO) << "Storage_Service_ExternalVolumeInfoTest_DoMount4OtherType_001 end";
}
} // STORAGE_DAEMON
} // OHOS
