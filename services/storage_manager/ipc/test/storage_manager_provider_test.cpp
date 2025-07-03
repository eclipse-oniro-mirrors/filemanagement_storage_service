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

#include <gtest/gtest.h>
#include <system_ability_definition.h>

#include "disk.h"
#include "message_parcel.h"
#include "storage_manager.h"
#include "storage_manager_provider.h"
#include "storage_service_errno.h"
#include "test/common/help_utils.h"
#include "user/multi_user_manager_service.h"
#include "volume_core.h"
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
namespace OHOS {
namespace StorageManager {
using namespace testing;
using namespace testing::ext;
class StorageManagerProviderTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp();
    void TearDown();

    StorageManagerProvider *storageManagerProviderTest_;
};

void StorageManagerProviderTest::SetUp(void)
{
    storageManagerProviderTest_ = new StorageManagerProvider(STORAGE_MANAGER_MANAGER_ID);
}

void StorageManagerProviderTest::TearDown(void)
{
    if (storageManagerProviderTest_ != nullptr) {
        delete storageManagerProviderTest_;
        storageManagerProviderTest_ = nullptr;
    }
}
/**
 * @tc.name: StorageManagerProviderTest_PrepareAddUser_001
 * @tc.desc: Verify the PrepareAddUser function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_PrepareAddUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_PrepareAddUser_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 121;
    uint32_t flags = 3;
    auto ret = storageManagerProviderTest_->PrepareAddUser(userId, flags);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    storageManagerProviderTest_->RemoveUser(userId, flags);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_PrepareAddUser_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_RemoveUser_001
 * @tc.desc: Verify the RemoveUser function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_RemoveUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_RemoveUser_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    const int32_t testUserId = 1001;
    const uint32_t testFlags = 0;
    auto ret = storageManagerProviderTest_->RemoveUser(testUserId, testFlags);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_RemoveUser_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_PrepareStartUser_001
 * @tc.desc: Verify the PrepareStartUser function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_PrepareStartUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_PrepareStartUser_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    auto ret = storageManagerProviderTest_->PrepareStartUser(OHOS::StorageDaemon::StorageTest::USER_ID1);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_PrepareStartUser_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_StopUser_001
 * @tc.desc: Verify the StopUser function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_StopUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_StopUser_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    auto ret = storageManagerProviderTest_->StopUser(OHOS::StorageDaemon::StorageTest::USER_ID2);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_StopUser_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_CompleteAddUser_001
 * @tc.desc: Verify the CompleteAddUser function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_CompleteAddUser_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "storageManagerProviderTest_CompleteAddUser_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    auto ret = storageManagerProviderTest_->CompleteAddUser(OHOS::StorageDaemon::StorageTest::USER_ID3);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_CompleteAddUser_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetFreeSizeOfVolume_001
 * @tc.desc: Verify the GetFreeSizeOfVolume function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetFreeSizeOfVolume_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "storageManagerProviderTest_GetFreeSizeOfVolume_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volumeUuid = "test-volume-uuid";
    int64_t freeSize = 0;
    auto ret = storageManagerProviderTest_->GetFreeSizeOfVolume(volumeUuid, freeSize);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetFreeSizeOfVolume_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetTotalSizeOfVolume_001
 * @tc.desc: Verify the GetTotalSizeOfVolume function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetTotalSizeOfVolume_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetTotalSizeOfVolume_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    const std::string volumeUuid = "test_volume_uuid";
    int64_t totalSize = 0;
    auto ret = storageManagerProviderTest_->GetTotalSizeOfVolume(volumeUuid, totalSize);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetTotalSizeOfVolume_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetBundleStats_001
 * @tc.desc: Verify the GetBundleStats function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetBundleStats_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetBundleStats_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string pkgName = "com.example.test";
    BundleStats bundleStats;
    int32_t appIndex = 0;
    uint32_t statFlag = 0x01;
    auto ret = storageManagerProviderTest_->GetBundleStats(pkgName, bundleStats, appIndex, statFlag);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetBundleStats_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetSystemSize_001
 * @tc.desc: Verify the GetSystemSize function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetSystemSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetSystemSize_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int64_t systemSize = 0;
    auto ret = storageManagerProviderTest_->GetSystemSize(systemSize);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetSystemSize_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetTotalSize_001
 * @tc.desc: Verify the GetTotalSize function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetTotalSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetTotalSize_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int64_t totalSize = 0;
    auto ret = storageManagerProviderTest_->GetTotalSize(totalSize);
    EXPECT_NE(ret, E_OK);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetTotalSize_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetFreeSize_001
 * @tc.desc: Verify the GetFreeSize function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetFreeSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetFreeSize_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int64_t freeSize = 0;
    auto ret = storageManagerProviderTest_->GetFreeSize(freeSize);
    EXPECT_NE(ret, E_OK);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetFreeSize_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetUserStorageStats_001
 * @tc.desc: Verify the GetUserStorageStats function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetUserStorageStats_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserStorageStats_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    StorageStats storageStats;
    auto ret = storageManagerProviderTest_->GetUserStorageStats(storageStats);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserStorageStats_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetUserStorageStatsIpc_001
 * @tc.desc: Verify the GetUserStorageStatsIpc function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetUserStorageStatsIpc_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserStorageStatsIpc_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    StorageStats storageStats;
    auto ret = storageManagerProviderTest_->GetUserStorageStats(userId, storageStats);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserStorageStatsIpc_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetCurrentBundleStats_001
 * @tc.desc: Verify the GetCurrentBundleStats function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetCurrentBundleStats_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetCurrentBundleStats_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    BundleStats bundleStats;
    uint32_t statFlag = 0x01;
    auto ret = storageManagerProviderTest_->GetCurrentBundleStats(bundleStats, statFlag);
    EXPECT_NE(ret, E_OK);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetCurrentBundleStats_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_NotifyVolumeCreated_001
 * @tc.desc: Verify the NotifyVolumeCreated function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyVolumeCreated_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeCreated_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    VolumeCore volumeCore;
    auto ret = storageManagerProviderTest_->NotifyVolumeCreated(volumeCore);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeCreated_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_NotifyVolumeMounted_001
 * @tc.desc: Verify the NotifyVolumeMounted function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyVolumeMounted_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeMounted_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volumeId = "testVolumeId";
    std::string fsTypeStr = "testfsTypeStr";
    std::string fsUuid = "testFsUuid";
    std::string path = "/mnt/testVolume";
    std::string description = "Test Volume";
    auto ret = storageManagerProviderTest_->NotifyVolumeMounted(volumeId, fsTypeStr, fsUuid, path, description);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeMounted_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_NotifyVolumeStateChanged_001
 * @tc.desc: Verify the NotifyVolumeStateChanged function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyVolumeStateChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeStateChanged_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volumeId = "testVolumeId";
    uint32_t state = MOUNTED;
    auto ret = storageManagerProviderTest_->NotifyVolumeStateChanged(volumeId, state);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeStateChanged_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_Mount_001
 * @tc.desc: Verify the Mount function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_Mount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Mount_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volumeId = "testVolumeId";
    auto ret = storageManagerProviderTest_->Mount(volumeId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Mount_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_Unmount_001
 * @tc.desc: Verify the Unmount function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_Unmount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Unmount_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volumeId = "testVolumeId";
    auto ret = storageManagerProviderTest_->Unmount(volumeId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Unmount_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetAllVolumes_001
 * @tc.desc: Verify the GetAllVolumes function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetAllVolumes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetAllVolumes_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::vector<VolumeExternal> volumes;
    auto ret = storageManagerProviderTest_->GetAllVolumes(volumes);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetAllVolumes_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_NotifyDiskCreated_001
 * @tc.desc: Verify the NotifyDiskCreated function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyDiskCreated_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyDiskCreated_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    Disk disk;
    auto ret = storageManagerProviderTest_->NotifyDiskCreated(disk);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyDiskCreated_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_NotifyDiskDestroyed_001
 * @tc.desc: Verify the NotifyDiskDestroyed function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyDiskDestroyed_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyDiskDestroyed_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string diskId = "testDiskId";
    auto ret = storageManagerProviderTest_->NotifyDiskDestroyed(diskId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyDiskDestroyed_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_Partition_001
 * @tc.desc: Verify the Partition function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_Partition_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Partition_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string diskId = "testDiskId";
    int32_t type = 1;
    auto ret = storageManagerProviderTest_->Partition(diskId, type);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Partition_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetAllDisks_001
 * @tc.desc: Verify the GetAllDisks function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetAllDisks_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetAllDisks_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::vector<Disk> disks;
    auto ret = storageManagerProviderTest_->GetAllDisks(disks);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetAllDisks_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetVolumeByUuid_001
 * @tc.desc: Verify the GetVolumeByUuid function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetVolumeByUuid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetVolumeByUuid_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string fsUuid = "testUuid";
    VolumeExternal volume;
    auto ret = storageManagerProviderTest_->GetVolumeByUuid(fsUuid, volume);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetVolumeByUuid_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetVolumeById_001
 * @tc.desc: Verify the GetVolumeById function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetVolumeById_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetVolumeById_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volumeId = "testVolumeId";
    VolumeExternal volume;
    auto ret = storageManagerProviderTest_->GetVolumeById(volumeId, volume);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetVolumeById_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_SetVolumeDescription_001
 * @tc.desc: Verify the SetVolumeDescription function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_SetVolumeDescription_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_SetVolumeDescription_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string fsUuid = "testUuid";
    std::string description = "Test Volume Description";
    auto ret = storageManagerProviderTest_->SetVolumeDescription(fsUuid, description);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_SetVolumeDescription_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_Format_001
 * @tc.desc: Verify the SetVolumeDescription function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_Format_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Format_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volumeId = "testVolumeId";
    std::string fsType = "ext4";
    auto ret = storageManagerProviderTest_->Format(volumeId, fsType);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_Format_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetDiskById_001
 * @tc.desc: Verify the GetDiskById function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetDiskById_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetDiskById_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string diskId = "testDiskId";
    Disk disk;
    auto ret = storageManagerProviderTest_->GetDiskById(diskId, disk);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetDiskById_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GenerateUserKeys_001
 * @tc.desc: Verify the GenerateUserKeys function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GenerateUserKeys_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GenerateUserKeys_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    uint32_t flags = 0x01;
    auto ret = storageManagerProviderTest_->GenerateUserKeys(userId, flags);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GenerateUserKeys_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_QueryUsbIsInUse_001
 * @tc.desc: Verify the QueryUsbIsInUse function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_QueryUsbIsInUse_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_QueryUsbIsInUse_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string diskPath = "/dev/sda1";
    bool isInUse = false;
    auto ret = storageManagerProviderTest_->QueryUsbIsInUse(diskPath, isInUse);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_QueryUsbIsInUse_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_DeleteUserKeys_001
 * @tc.desc: Verify the DeleteUserKeys function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_DeleteUserKeys_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_DeleteUserKeys_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    auto ret = storageManagerProviderTest_->DeleteUserKeys(userId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_DeleteUserKeys_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UpdateUserAuth_001
 * @tc.desc: Verify the UpdateUserAuth function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UpdateUserAuth_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateUserAuth_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    uint64_t secureUid = 123456789;
    std::vector<uint8_t> token = {1, 2, 3, 4};
    std::vector<uint8_t> oldSecret = {5, 6, 7, 8};
    std::vector<uint8_t> newSecret = {9, 10, 11, 12};
    auto ret = storageManagerProviderTest_->UpdateUserAuth(userId, secureUid, token, oldSecret, newSecret);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateUserAuth_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UpdateUseAuthWithRecoveryKey_001
 * @tc.desc: Verify the UpdateUseAuthWithRecoveryKey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UpdateUseAuthWithRecoveryKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateUseAuthWithRecoveryKey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::vector<uint8_t> authToken = {1, 2, 3, 4};
    std::vector<uint8_t> newSecret = {5, 6, 7, 8};
    uint64_t secureUid = 123456789;
    uint32_t userId = 1001;
    std::vector<std::vector<uint8_t>> plainText = {{9, 10}, {11, 12}};
    auto ret =
        storageManagerProviderTest_->UpdateUseAuthWithRecoveryKey(authToken, newSecret, secureUid, userId, plainText);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateUseAuthWithRecoveryKey_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_ActiveUserKey_001
 * @tc.desc: Verify the ActiveUserKey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_ActiveUserKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_ActiveUserKey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    std::vector<uint8_t> token = {1, 2, 3, 4};
    std::vector<uint8_t> secret = {5, 6, 7, 8};
    auto ret = storageManagerProviderTest_->ActiveUserKey(userId, token, secret);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_ActiveUserKey_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_InactiveUserKey_001
 * @tc.desc: Verify the InactiveUserKey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_InactiveUserKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_InactiveUserKey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    auto ret = storageManagerProviderTest_->InactiveUserKey(userId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_InactiveUserKey_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_LockUserScreen_001
 * @tc.desc: Verify the LockUserScreen function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_LockUserScreen_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_LockUserScreen_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    auto ret = storageManagerProviderTest_->LockUserScreen(userId);
    EXPECT_NE(ret, E_OK);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_LockUserScreen_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetFileEncryptStatus_001
 * @tc.desc: Verify the GetFileEncryptStatus function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetFileEncryptStatus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetFileEncryptStatus_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    bool isEncrypted = false;
    bool needCheckDirMount = true;
    auto ret = storageManagerProviderTest_->GetFileEncryptStatus(userId, isEncrypted, needCheckDirMount);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetFileEncryptStatus_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetUserNeedActiveStatus_001
 * @tc.desc: Verify the GetUserNeedActiveStatus function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetUserNeedActiveStatus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserNeedActiveStatus_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    bool needActive = false;
    auto ret = storageManagerProviderTest_->GetUserNeedActiveStatus(userId, needActive);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserNeedActiveStatus_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UnlockUserScreen_001
 * @tc.desc: Verify the UnlockUserScreen function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UnlockUserScreen_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UnlockUserScreen_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    std::vector<uint8_t> token = {0x01, 0x02, 0x03};
    std::vector<uint8_t> secret = {0x04, 0x05, 0x06};
    auto ret = storageManagerProviderTest_->UnlockUserScreen(userId, token, secret);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UnlockUserScreen_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetLockScreenStatus_001
 * @tc.desc: Verify the GetLockScreenStatus function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetLockScreenStatus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetLockScreenStatus_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    bool needActive = false;
    auto ret = storageManagerProviderTest_->GetLockScreenStatus(userId, needActive);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetLockScreenStatus_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GenerateAppkey_001
 * @tc.desc: Verify the GenerateAppkey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GenerateAppkey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GenerateAppkey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t hashId = 12345;
    uint32_t userId = 1001;
    std::string keyId;
    bool needReSet = false;
    auto ret = storageManagerProviderTest_->GenerateAppkey(hashId, userId, keyId, needReSet);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GenerateAppkey_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_DeleteAppkey_001
 * @tc.desc: Verify the DeleteAppkey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_DeleteAppkey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_DeleteAppkey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string keyId = "testKeyId";
    auto ret = storageManagerProviderTest_->DeleteAppkey(keyId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_DeleteAppkey_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_CreateRecoverKey_001
 * @tc.desc: Verify the CreateRecoverKey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_CreateRecoverKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_CreateRecoverKey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    uint32_t userType = 1;
    std::vector<uint8_t> token = {0x01, 0x02, 0x03};
    std::vector<uint8_t> secret = {0xAA, 0xBB, 0xCC};
    auto ret = storageManagerProviderTest_->CreateRecoverKey(userId, userType, token, secret);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_CreateRecoverKey_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_SetRecoverKey_001
 * @tc.desc: Verify the SetRecoverKey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_SetRecoverKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_SetRecoverKey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::vector<uint8_t> key = {0x01, 0x02, 0x03};
    auto ret = storageManagerProviderTest_->SetRecoverKey(key);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_SetRecoverKey_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UpdateKeyContext_001
 * @tc.desc: Verify the UpdateKeyContext function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UpdateKeyContext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateKeyContext_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    bool needRemoveTmpKey = true;
    auto ret = storageManagerProviderTest_->UpdateKeyContext(userId, needRemoveTmpKey);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateKeyContext_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_CreateShareFile_001
 * @tc.desc: Verify the CreateShareFile function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_CreateShareFile_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_CreateShareFile_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::vector<std::string> uriList = {"file1", "file2"};
    StorageFileRawData fileRawData;
    fileRawData.ownedData = "file1";
    fileRawData.size = fileRawData.ownedData.size();
    fileRawData.data = fileRawData.ownedData.data();
    uint32_t tokenId = 12345;
    uint32_t flag = 1;
    std::vector<int32_t> funcResult;
    auto ret = storageManagerProviderTest_->CreateShareFile(fileRawData, tokenId, flag, funcResult);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_CreateShareFile_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_DeleteShareFile_001
 * @tc.desc: Verify the DeleteShareFile function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_DeleteShareFile_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_DeleteShareFile_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    StorageFileRawData fileRawData;
    fileRawData.ownedData = "file1";
    fileRawData.size = fileRawData.ownedData.size();
    fileRawData.data = fileRawData.ownedData.data();
    uint32_t tokenId = 12345;
    auto ret = storageManagerProviderTest_->DeleteShareFile(tokenId, fileRawData);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_DeleteShareFile_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_SetBundleQuota_001
 * @tc.desc: Verify the SetBundleQuota function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_SetBundleQuota_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_SetBundleQuota_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string bundleName = "com.example.app";
    int32_t uid = 1001;
    std::string bundleDataDirPath = "/data/app/example";
    int32_t limitSizeMb = 1024;
    auto ret = storageManagerProviderTest_->SetBundleQuota(bundleName, uid, bundleDataDirPath, limitSizeMb);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_SetBundleQuota_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_GetUserStorageStatsByType_001
 * @tc.desc: Verify the GetUserStorageStatsByType function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_GetUserStorageStatsByType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserStorageStatsByType_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    StorageStats storageStats;
    std::string type = "exampleType";
    auto ret = storageManagerProviderTest_->GetUserStorageStatsByType(userId, storageStats, type);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_GetUserStorageStatsByType_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UpdateMemoryPara_001
 * @tc.desc: Verify the UpdateMemoryPara function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UpdateMemoryPara_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateMemoryPara_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t size = 1024;
    int32_t oldSize = 0;
    auto ret = storageManagerProviderTest_->UpdateMemoryPara(size, oldSize);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    EXPECT_EQ(oldSize, 0);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UpdateMemoryPara_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_MountDfsDocs_001
 * @tc.desc: Verify the MountDfsDocs function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_MountDfsDocs_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountDfsDocs_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    std::string relativePath = "/relative/path";
    std::string networkId = "network123";
    std::string deviceId = "device123";
    auto ret = storageManagerProviderTest_->MountDfsDocs(userId, relativePath, networkId, deviceId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountDfsDocs_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UMountDfsDocs_001
 * @tc.desc: Verify the UMountDfsDocs function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UMountDfsDocs_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UMountDfsDocs_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    std::string relativePath = "/relative/path";
    std::string networkId = "network123";
    std::string deviceId = "device123";
    auto ret = storageManagerProviderTest_->UMountDfsDocs(userId, relativePath, networkId, deviceId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UMountDfsDocs_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_NotifyMtpMounted_001
 * @tc.desc: Verify the NotifyMtpMounted function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyMtpMounted_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyMtpMounted_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string id = "mtpId";
    std::string path = "/mnt/mtp";
    std::string desc = "MTP Device";
    std::string uuid = "1234-5678";
    auto ret = storageManagerProviderTest_->NotifyMtpMounted(id, path, desc, uuid);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyMtpMounted_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_NotifyMtpUnmounted_001
 * @tc.desc: Verify the NotifyMtpUnmounted function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyMtpUnmounted_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyMtpUnmounted_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string id = "mtpId";
    std::string path = "/mnt/mtp";
    bool isBadRemove = false;
    auto ret = storageManagerProviderTest_->NotifyMtpUnmounted(id, path, isBadRemove);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyMtpUnmounted_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_MountMediaFuse_001
 * @tc.desc: Verify the MountMediaFuse function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_MountMediaFuse_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountMediaFuse_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    int32_t devFd = -1;
    auto ret = storageManagerProviderTest_->MountMediaFuse(userId, devFd);
    EXPECT_NE(ret, E_OK);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountMediaFuse_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UMountMediaFuse_001
 * @tc.desc: Verify the UMountMediaFuse function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UMountMediaFuse_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountMediaFuse_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    auto ret = storageManagerProviderTest_->UMountMediaFuse(userId);
    EXPECT_NE(ret, E_OK);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UMountMediaFuse_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_MountFileMgrFuse_001
 * @tc.desc: Verify the MountFileMgrFuse function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_MountFileMgrFuse_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountFileMgrFuse_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    std::string path = "/mnt/fuse";
    int32_t fuseFd = -1;
    auto ret = storageManagerProviderTest_->MountFileMgrFuse(userId, path, fuseFd);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    EXPECT_EQ(fuseFd, -1);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountFileMgrFuse_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_UMountFileMgrFuse_001
 * @tc.desc: Verify the UMountFileMgrFuse function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UMountFileMgrFuse_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UMountFileMgrFuse_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 1001;
    std::string path = "/mnt/fuse";
    auto ret = storageManagerProviderTest_->UMountFileMgrFuse(userId, path);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UMountFileMgrFuse_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_IsFileOccupied_001
 * @tc.desc: Verify the IsFileOccupied function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_IsFileOccupied_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_IsFileOccupied_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string path = "/test/file";
    std::vector<std::string> inputList = {"file1", "file2"};
    std::vector<std::string> outputList;
    bool isOccupy = false;
    auto ret = storageManagerProviderTest_->IsFileOccupied(path, inputList, outputList, isOccupy);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_IsFileOccupied_001 end";
}
/**
 * @tc.name: StorageManagerProviderTest_ResetSecretWithRecoveryKey_001
 * @tc.desc: Verify the ResetSecretWithRecoveryKey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_ResetSecretWithRecoveryKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_ResetSecretWithRecoveryKey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 1001;
    uint32_t rkType = 1;
    std::vector<uint8_t> key = {0x01, 0x02, 0x03};
    auto ret = storageManagerProviderTest_->ResetSecretWithRecoveryKey(userId, rkType, key);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_ResetSecretWithRecoveryKey_001 end";
}

/**
 * @tc.name: StorageManagerProviderTest_TryToFix_001
 * @tc.desc: Verify the ResetSecretWithRecoveryKey function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_TryToFix_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_TryToFix_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volId = "vol-8-1";
    auto ret = storageManagerProviderTest_->TryToFix(volId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_TryToFix_001 end";
}
 
/**
 * @tc.name: StorageManagerProviderTest_NotifyVolumeDamaged_001
 * @tc.desc: Verify the NotifyVolumeDamaged function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_NotifyVolumeDamaged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeDamaged_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    std::string volId = "vol-8-1";
    std::string fsTypeStr = "ntfs";
    std::string uuid = "uuid-1";
    std::string path = "/";
    std::string description = "My Disk";
    auto ret = storageManagerProviderTest_->NotifyVolumeDamaged(volId, fsTypeStr, uuid, path, description);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_NotifyVolumeDamaged_001 end";
}

/**
 * @tc.name: StorageManagerProviderTest_MountDisShareFile_001
 * @tc.desc: Verify the MountDisShareFile function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_MountDisShareFile_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountDisShareFile_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 100;
    std::map<std::string, std::string> shareFiles = {{{"/data/sharefile1", "/data/sharefile2"}}};
    auto ret = storageManagerProviderTest_->MountDisShareFile(userId, shareFiles);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_MountDisShareFile_001 end";
}

/**
 * @tc.name: StorageManagerProviderTest_UMountDisShareFile_001
 * @tc.desc: Verify the UMountDisShareFile function.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_UMountDisShareFile_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UMountDisShareFile_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    int32_t userId = 100;
    std::string networkId = "sharefile1";
    auto ret = storageManagerProviderTest_->UMountDisShareFile(userId, networkId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_UMountDisShareFile_001 end";
}

/**
 * @tc.name: StorageManagerProviderTest_InactiveUserPublicDirKey_001
 * @tc.desc: Verify the InactiveUserPublicDirKey function.
 * @tc.type: FUNC
 * @tc.require: AR20250418146433
 */
HWTEST_F(StorageManagerProviderTest, StorageManagerProviderTest_InactiveUserPublicDirKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_InactiveUserPublicDirKey_001 start";
    ASSERT_TRUE(storageManagerProviderTest_ != nullptr);
    uint32_t userId = 100;
    auto ret = storageManagerProviderTest_->InactiveUserPublicDirKey(userId);
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
    GTEST_LOG_(INFO) << "StorageManagerProviderTest_InactiveUserPublicDirKey_001 end";
}
} // namespace StorageManager
} // namespace OHOS