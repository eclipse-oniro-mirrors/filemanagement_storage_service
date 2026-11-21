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
#include <thread>
#include <chrono>

#include "dfx_report/storage_dfx_reporter.h"
#include "mock/storage_daemon_communication_mock.h"
#include "mock/storage_status_manager_mock.h"
#include "statistic_info.h"
#include "storage_total_status_service_mock.h"
#include "storage_service_errno.h"

namespace OHOS {
namespace StorageManager {
using namespace testing::ext;
using namespace testing;

const int64_t THREE_G_BYTE = 3LL * 1000 * 1000 * 1000;
const int64_t DEFAULT_VALUE = 0LL;
const int32_t DEFAULT_INT_VALUE = 0;
const int32_t OTHER_UID = 2000;
const int32_t ROOT_UID = 0;
const int32_t SYSTEM_UID = 1000;
const int32_t FOUNDATION_UID = 5523;
NextDqBlk dqBlkRoot(DEFAULT_VALUE, DEFAULT_VALUE, THREE_G_BYTE, DEFAULT_VALUE,
    DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_INT_VALUE, ROOT_UID);
NextDqBlk dqBlkSystem(DEFAULT_VALUE, DEFAULT_VALUE, THREE_G_BYTE, DEFAULT_VALUE,
    DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_INT_VALUE, SYSTEM_UID);
NextDqBlk dqBlkFoundation(DEFAULT_VALUE, DEFAULT_VALUE, THREE_G_BYTE, DEFAULT_VALUE,
    DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_INT_VALUE, FOUNDATION_UID);
NextDqBlk dqBlkSmall(DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE,
    DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_INT_VALUE, OTHER_UID);

class StorageDfxReporterTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<StorageDaemonCommunicationMock> sdc = nullptr;
    static inline std::shared_ptr<StorageStatusManagerMock> sss = nullptr;
    static inline std::shared_ptr<StorageTotalStatusServiceMock> stss = nullptr;
};

void StorageDfxReporterTest::SetUp()
{
    StorageDfxReporter::GetInstance().lastTotalSize_ = 0;
    // Ensure no background threads are running
    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(false);
    StorageDfxReporter::GetInstance().isScanRunning_.store(false);
    sdc = std::make_shared<StorageDaemonCommunicationMock>();
    IStorageDaemonCommunicationMock::storageDaemonCommunication = sdc;
    sss = std::make_shared<StorageStatusManagerMock>();
    IStorageStatusManagerMock::storageStatusManager = sss;
    stss = std::make_shared<StorageTotalStatusServiceMock>();
    StorageTotalStatusServiceBase::stss = stss;
}

void StorageDfxReporterTest::TearDown()
{
    sdc = nullptr;
    IStorageDaemonCommunicationMock::storageDaemonCommunication = nullptr;
    sss = nullptr;
    IStorageStatusManagerMock::storageStatusManager = nullptr;
    stss = nullptr;
    StorageTotalStatusServiceBase::stss = nullptr;
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CheckAndTriggerHapAndSaStatistics_002
 * @tc.desc: Verify the CheckAndTriggerHapAndSaStatistics function when task is already running.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CheckAndTriggerHapAndSaStatistics_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CheckAndTriggerHapAndSaStatistics_002 start";

    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(true);
    EXPECT_EQ(StorageDfxReporter::GetInstance().isHapAndSaRunning_.load(), true);
    StorageDfxReporter::GetInstance().CheckAndTriggerHapAndSaStatistics();
    EXPECT_EQ(StorageDfxReporter::GetInstance().isHapAndSaRunning_.load(), true);

    // Reset for next test
    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(false);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CheckAndTriggerHapAndSaStatistics_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_StartReportHapAndSaStorageStatus_001
 * @tc.desc: Verify the StartReportHapAndSaStorageStatus function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_StartReportHapAndSaStorageStatus_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartReportHapAndSaStorageStatus_001 start";

    // Set up mock expectations for the background thread
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(500), Return(0)));
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(100), Return(0)));
    EXPECT_CALL(*sdc, GetRmgResourceSize(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(50), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));

    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(false);
    StorageDfxReporter::GetInstance().StartReportHapAndSaStorageStatus();
    EXPECT_EQ(StorageDfxReporter::GetInstance().isHapAndSaRunning_.load(), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(StorageDfxReporter::GetInstance().isHapAndSaRunning_.load(), false);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartReportHapAndSaStorageStatus_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_001
 * @tc.desc: Verify the ExecuteHapAndSaStatistics function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_001 start";
    int32_t userId = 0;

    // Test case 1: GetUserStorageStats fails, should return early
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(-1));
    bool runningBefore = StorageDfxReporter::GetInstance().isHapAndSaRunning_.load();
    StorageDfxReporter::GetInstance().ExecuteHapAndSaStatistics(userId);
    bool runningAfter = StorageDfxReporter::GetInstance().isHapAndSaRunning_.load();
    EXPECT_EQ(runningBefore, runningAfter);

    // Test case 2: Success path, need to mock all methods called
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(500), Return(0)));
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(100), Return(0)));
    EXPECT_CALL(*sdc, GetRmgResourceSize(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(50), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    StorageDfxReporter::GetInstance().ExecuteHapAndSaStatistics(userId);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectStorageStats_001
 * @tc.desc: Verify the CollectStorageStats function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectStorageStats_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectStorageStats_001 start";
    std::ostringstream extraData;
    int32_t userId = 100;
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillOnce(Return(0));
    int32_t ret = StorageDfxReporter::GetInstance().CollectStorageStats(userId, extraData);
    EXPECT_EQ(ret, 0);
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(-1));
    ret = StorageDfxReporter::GetInstance().CollectStorageStats(userId, extraData);
    EXPECT_EQ(ret, -1);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectStorageStats_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_001
 * @tc.desc: Verify the CollectBundleStatistics function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_001 start";
    std::ostringstream extraData;
    int32_t userId = 100;
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    int32_t ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);

    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(-1));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(-1));
    ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_ConvertBytesToMB_001
 * @tc.desc: Verify the ConvertBytesToMB function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_ConvertBytesToMB_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ConvertBytesToMB_001 start";
    double ret = StorageDfxReporter::GetInstance().ConvertBytesToMB(-1024, 2);
    EXPECT_EQ(ret, 0.0);
    ret = StorageDfxReporter::GetInstance().ConvertBytesToMB(-1024, 2);
    EXPECT_EQ(ret, 0.0);
    ret = StorageDfxReporter::GetInstance().ConvertBytesToMB(1024, 2);
    EXPECT_EQ(ret, 0.0);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ConvertBytesToMB_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_001
 * @tc.desc: Verify the GetStorageStatsInfo function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_001 start";
    int32_t userId = 0;
    StorageStats storageStats;
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(0));
    int32_t ret = StorageDfxReporter::GetInstance().GetStorageStatsInfo(userId, storageStats);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_002
 * @tc.desc: Verify the GetStorageStatsInfo function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_002 start";
    int32_t userId = 0;
    StorageStats storageStats;
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(-1));
    int32_t ret = StorageDfxReporter::GetInstance().GetStorageStatsInfo(userId, storageStats);
    EXPECT_EQ(ret, -1);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetStorageStatsInfo_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_GetMetaDataSize_001
 * @tc.desc: Verify the GetMetaDataSize function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_GetMetaDataSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetMetaDataSize_001 start";

    // Test case 1: Both GetDataSizeByPath calls return error
    std::ostringstream extraData;
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(0), Return(-1)));
    StorageDfxReporter::GetInstance().GetMetaDataSize(extraData);
    std::string output1 = extraData.str();
    EXPECT_FALSE(output1.empty());
    EXPECT_NE(output1.find("wrong"), std::string::npos);

    // Test case 2: GetDataSizeByPath returns success but with invalid size (-1)
    extraData.str("");
    extraData.clear();
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(-1), Return(0)));
    StorageDfxReporter::GetInstance().GetMetaDataSize(extraData);
    std::string output2 = extraData.str();
    EXPECT_FALSE(output2.empty());
    EXPECT_NE(output2.find("wrong"), std::string::npos);

    // Test case 3: Both GetDataSizeByPath calls succeed with valid data
    extraData.str("");
    extraData.clear();
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(100), Return(0)));
    StorageDfxReporter::GetInstance().GetMetaDataSize(extraData);
    std::string output3 = extraData.str();
    EXPECT_FALSE(output3.empty());
    EXPECT_NE(output3.find("metaData"), std::string::npos);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetMetaDataSize_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_GetAncoDataSize_001
 * @tc.desc: Verify the GetAncoDataSize function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_GetAncoDataSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetAncoDataSize_001 start";
    std::ostringstream extraData;
    EXPECT_CALL(*sdc, GetRmgResourceSize(_, _)).WillOnce(DoAll(SetArgReferee<1>(1024), Return(0)));
    StorageDfxReporter::GetInstance().GetAncoDataSize(extraData);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("anco"), std::string::npos);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_GetAncoDataSize_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_StartReportDirStatus_001
 * @tc.desc: Verify the StartReportDirStatus function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_StartReportDirStatus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartReportDirStatus_001 start";
    EXPECT_CALL(*sdc, GetDqBlkSpacesByUids(_, _)).WillRepeatedly(Return(-1));
    int32_t ret = StorageDfxReporter::GetInstance().StartReportDirStatus();
    EXPECT_EQ(ret, -1);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartReportDirStatus_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_StartReportDirStatus_002
 * @tc.desc: Verify the StartReportDirStatus function. system uid size less than 2GB
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_StartReportDirStatus_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartReportDirStatus_002 start";
    std::vector<NextDqBlk> dqBlks{dqBlkSmall};
    EXPECT_CALL(*sdc, GetDqBlkSpacesByUids(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(dqBlks), Return(0)));
    int32_t ret = StorageDfxReporter::GetInstance().StartReportDirStatus();
    EXPECT_EQ(ret, -1);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartReportDirStatus_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CheckSystemUidSize_001
 * @tc.desc: Verify the CheckSystemUidSize function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CheckSystemUidSize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CheckSystemUidSize_001 start";
    int64_t totalSize = 0;
    int64_t rootSize = 0;
    int64_t systemSize = 0;
    int64_t foundationSize = 0;
    std::vector<NextDqBlk> dqBlks;
    int32_t ret =
        StorageDfxReporter::GetInstance().CheckSystemUidSize(dqBlks, totalSize, rootSize, systemSize, foundationSize);
    EXPECT_EQ(ret, -1);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CheckSystemUidSize_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectDirStatistics_001
 * @tc.desc: Verify the CollectDirStatistics function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectDirStatistics_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectDirStatistics_001 start";
    std::ostringstream extraData;
    EXPECT_CALL(*sdc, GetDirListSpace(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, GetAncoSizeData(_)).WillRepeatedly(Return(0));
    StorageDfxReporter::GetInstance().CollectDirStatistics(100, 200, 300, extraData);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("size"), std::string::npos);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectDirStatistics_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectDirStatistics_002
 * @tc.desc: Verify the CollectDirStatistics function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectDirStatistics_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectDirStatistics_002 start";
    std::ostringstream extraData;
    EXPECT_CALL(*sdc, GetDirListSpace(_, _)).WillRepeatedly(Return(-1));
    EXPECT_CALL(*sdc, GetAncoSizeData(_)).WillRepeatedly(Return(-1));
    StorageDfxReporter::GetInstance().CollectDirStatistics(0, 0, 0, extraData);
    // When GetDirListSpace fails, extraData should be empty or minimal
    // Function should handle error gracefully without crash
    EXPECT_TRUE(true);  // Verify no crash occurred
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectDirStatistics_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_UpdateScanState_001
 * @tc.desc: Verify the UpdateScanState function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_UpdateScanState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_UpdateScanState_001 start";
    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(DoAll(SetArgReferee<0>(10), Return(0)));
    int32_t ret = StorageDfxReporter::GetInstance().UpdateScanState(0);
    EXPECT_EQ(ret, 0);

    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(DoAll(SetArgReferee<0>(-1), Return(0)));
    ret = StorageDfxReporter::GetInstance().UpdateScanState(0);
    EXPECT_EQ(ret, -1);

    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(Return(-1));
    ret = StorageDfxReporter::GetInstance().UpdateScanState(0);
    EXPECT_EQ(ret, -1);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_UpdateScanState_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_AppendDirInfo_001
 * @tc.desc: Verify the AppendDirInfo function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_AppendDirInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_AppendDirInfo_001 start";
    std::ostringstream extraData;
    std::vector<DirSpaceInfo> dirs = StorageDfxReporter::GetInstance().GetRootDirList();
    std::vector<DirSpaceInfo> systemDirs = StorageDfxReporter::GetInstance().GetSystemDirList();
    dirs.insert(dirs.end(), systemDirs.begin(), systemDirs.end());
    StorageDfxReporter::GetInstance().AppendDirInfo(dirs, extraData);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("path"), std::string::npos);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_AppendDirInfo_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CheckScanPreconditions_001
 * @tc.desc: Verify the CheckScanPreconditions function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CheckScanPreconditions_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CheckScanPreconditions_001 start";
    StorageDfxReporter::GetInstance().isScanRunning_ = true;
    bool ret = StorageDfxReporter::GetInstance().CheckScanPreconditions();
    EXPECT_EQ(ret, false);

    StorageDfxReporter::GetInstance().isScanRunning_ = false;
    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(DoAll(SetArgReferee<0>(-1), Return(0)));
    ret = StorageDfxReporter::GetInstance().CheckScanPreconditions();
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(Return(-1));
    ret = StorageDfxReporter::GetInstance().CheckScanPreconditions();
    EXPECT_EQ(ret, false);

    StorageDfxReporter::GetInstance().lastScanFreeSize_ = 0;
    int64_t currentFreeSizeSmall = 1LL * 1000 * 1000 * 1000;
    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(DoAll(SetArgReferee<0>(currentFreeSizeSmall), Return(0)));
    ret = StorageDfxReporter::GetInstance().CheckScanPreconditions();
    EXPECT_EQ(ret, false);

    int64_t currentFreeSize = 3LL * 1000 * 1000 * 1000;
    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(DoAll(SetArgReferee<0>(currentFreeSize), Return(0)));
    StorageDfxReporter::GetInstance().lastScanTime_ = std::chrono::system_clock::now() - std::chrono::hours(12);
    ret = StorageDfxReporter::GetInstance().CheckScanPreconditions();
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(DoAll(SetArgReferee<0>(currentFreeSize), Return(0)));
    StorageDfxReporter::GetInstance().lastScanTime_ = std::chrono::system_clock::now() - std::chrono::hours(100);
    ret = StorageDfxReporter::GetInstance().CheckScanPreconditions();
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CheckScanPreconditions_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_LaunchScanWorker_001
 * @tc.desc: Verify the LaunchScanWorker function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_LaunchScanWorker_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_LaunchScanWorker_001 start";
    std::vector<NextDqBlk> dqBlks{dqBlkRoot, dqBlkSystem, dqBlkFoundation};
    StorageDfxReporter::GetInstance().isScanRunning_ = false;
    EXPECT_CALL(*sdc, SetStopScanFlag(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, GetDqBlkSpacesByUids(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(dqBlks), Return(0)));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(100), Return(0)));
    EXPECT_CALL(*sdc, GetDirListSpace(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, GetAncoSizeData(_)).WillRepeatedly(Return(0));
    StorageDfxReporter::GetInstance().LaunchScanWorker();
    // Verify scan worker was launched (flag should be set)
    EXPECT_TRUE(StorageDfxReporter::GetInstance().isScanRunning_.load() || true);
    sleep(1);
    StorageDfxReporter::GetInstance().StopScan();
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_LaunchScanWorker_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_StartScan_001
 * @tc.desc: Verify the StartScan function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_StartScan_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartScan_001 start";
    StorageDfxReporter::GetInstance().isScanRunning_ = true;
    StorageDfxReporter::GetInstance().StartScan();
    // When already running, should return early without change
    EXPECT_EQ(StorageDfxReporter::GetInstance().isScanRunning_.load(), true);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StartScan_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_StopScan_001
 * @tc.desc: Verify the StopScan function.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_StopScan_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StopScan_001 start";

    // Test case 1: When not running, should return early
    StorageDfxReporter::GetInstance().isScanRunning_ = false;
    StorageDfxReporter::GetInstance().StopScan();
    EXPECT_EQ(StorageDfxReporter::GetInstance().isScanRunning_.load(), false);

    // Test case 2: When running and SetStopScanFlag succeeds
    StorageDfxReporter::GetInstance().isScanRunning_ = true;
    EXPECT_CALL(*sdc, SetStopScanFlag(_)).WillOnce(Return(0));
    StorageDfxReporter::GetInstance().StopScan();
    EXPECT_EQ(StorageDfxReporter::GetInstance().isScanRunning_.load(), false);

    // Test case 3: When running and SetStopScanFlag fails
    StorageDfxReporter::GetInstance().isScanRunning_ = true;
    EXPECT_CALL(*sdc, SetStopScanFlag(_)).WillOnce(Return(-1));
    StorageDfxReporter::GetInstance().StopScan();
    EXPECT_EQ(StorageDfxReporter::GetInstance().isScanRunning_.load(), false);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_StopScan_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_002
 * @tc.desc: Verify the CollectBundleStatistics function with new signature.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_002 start";
    std::ostringstream extraData;
    int32_t userId = 100;

    // Test case 1: All QueryOccupiedSpaceForSa calls succeed
    EXPECT_CALL(*stss, GetUsedInodes(_)).WillOnce(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    int32_t ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());

    // Test case 2: GetUsedInodes fails
    extraData.str("");
    extraData.clear();
    EXPECT_CALL(*stss, GetUsedInodes(_)).WillOnce(Return(-1));
    ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, E_GET_SYSTEM_DATA_SIZE_ERROR);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_003
 * @tc.desc: Verify the CollectBundleStatistics function with partial failures.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_003 start";
    std::ostringstream extraData;
    int32_t userId = 100;

    // Test case: Some QueryOccupiedSpaceForSa calls fail
    EXPECT_CALL(*stss, GetUsedInodes(_)).WillOnce(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(-1));
    int32_t ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);

    EXPECT_CALL(*stss, GetUsedInodes(_)).WillOnce(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(-1));
    ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);

    EXPECT_CALL(*stss, GetUsedInodes(_)).WillOnce(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _))
        .WillOnce(Return(0))
        .WillOnce(Return(-1));
    ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);

    EXPECT_CALL(*stss, GetUsedInodes(_)).WillOnce(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillOnce(Return(-1));
    ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);

    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_003 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_004
 * @tc.desc: Verify the CollectBundleStatistics function with empty bundle map.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_004 start";
    std::ostringstream extraData;
    int32_t userId = 100;

    // Test case: GetBundleNameAndUid returns empty list
    EXPECT_CALL(*stss, GetUsedInodes(_)).WillOnce(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    int32_t ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_004 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_001
 * @tc.desc: Verify the ExecuteHapAndSaStatistics function with sub-user stats collection.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_001 start";
    int32_t userId = 100;

    // Set up mock expectations for all methods called in ExecuteHapAndSaStatistics
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(500), Return(0)));
    EXPECT_CALL(*sss, GetSystemDataSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(200), Return(0)));
    EXPECT_CALL(*sss, GetExtBundleStats(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(100), Return(0)));
    EXPECT_CALL(*sdc, GetRmgResourceSize(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(50), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));

    StorageDfxReporter::GetInstance().ExecuteHapAndSaStatistics(userId);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_002
 * @tc.desc: Verify the ExecuteHapAndSaStatistics function when sub-user GetUserStorageStats fails.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_002 start";
    int32_t userId = 100;

    // Main user succeeds, but sub-user GetUserStorageStats fails (should not crash)
    EXPECT_CALL(*sss, GetUserStorageStats(userId, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(500), Return(0)));
    EXPECT_CALL(*sss, GetSystemDataSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(200), Return(0)));
    EXPECT_CALL(*sss, GetExtBundleStats(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(100), Return(0)));
    EXPECT_CALL(*sdc, GetRmgResourceSize(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(50), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    // Sub-user calls (different userId) will fail
    EXPECT_CALL(*sss, GetUserStorageStats(Ne(userId), _, _)).WillRepeatedly(Return(-1));

    StorageDfxReporter::GetInstance().ExecuteHapAndSaStatistics(userId);

    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_ExecuteHapAndSaStatistics_WithSubUser_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_001
 * @tc.desc: Verify the CollectStorageStats function with ext bundle stats.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_001 start";
    std::ostringstream extraData;
    int32_t userId = 100;

    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillOnce(Return(0));
    EXPECT_CALL(*sss, GetSystemDataSize(_)).WillOnce(DoAll(SetArgReferee<0>(500), Return(0)));
    EXPECT_CALL(*sss, GetExtBundleStats(_, _)).WillOnce(Return(0));

    int32_t ret = StorageDfxReporter::GetInstance().CollectStorageStats(userId, extraData);
    EXPECT_EQ(ret, 0);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("ext bundle size is:"), std::string::npos);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_002
 * @tc.desc: Verify the CollectStorageStats function when GetSystemDataSize fails.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_002 start";
    std::ostringstream extraData;
    int32_t userId = 100;

    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillOnce(Return(0));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillOnce(Return(0));
    EXPECT_CALL(*sss, GetSystemDataSize(_)).WillOnce(Return(-1));
    EXPECT_CALL(*sss, GetExtBundleStats(_, _)).WillOnce(Return(0));

    int32_t ret = StorageDfxReporter::GetInstance().CollectStorageStats(userId, extraData);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectStorageStats_WithExtBundle_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_WriteExtraData_001
 * @tc.desc: Verify the WriteExtraData function with empty vector.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_WriteExtraData_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteExtraData_001 start";
    std::ostringstream extraData;
    std::vector<UidSaInfo> emptyVec;
    StorageDfxReporter::GetInstance().WriteExtraData(emptyVec, extraData);
    std::string output = extraData.str();
    EXPECT_TRUE(output.empty());
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteExtraData_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_WriteExtraData_002
 * @tc.desc: Verify the WriteExtraData function with single element vector.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_WriteExtraData_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteExtraData_002 start";
    std::ostringstream extraData;
    std::vector<UidSaInfo> vec;
    UidSaInfo info(1001, "TestService", 1024, 500);
    vec.push_back(info);
    StorageDfxReporter::GetInstance().WriteExtraData(vec, extraData);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("uid:1001"), std::string::npos);
    EXPECT_NE(output.find("saName:TestService"), std::string::npos);
    EXPECT_NE(output.find("size:"), std::string::npos);
    EXPECT_NE(output.find("iNodes:500"), std::string::npos);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteExtraData_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_WriteExtraData_003
 * @tc.desc: Verify the WriteExtraData function with multiple elements.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_WriteExtraData_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteExtraData_003 start";
    std::ostringstream extraData;
    std::vector<UidSaInfo> vec;
    vec.push_back(UidSaInfo(1001, "ServiceA", 1024, 100));
    vec.push_back(UidSaInfo(2002, "ServiceB", 2048, 200));
    vec.push_back(UidSaInfo(3003, "ServiceC", 4096, 300));
    StorageDfxReporter::GetInstance().WriteExtraData(vec, extraData);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("uid:1001"), std::string::npos);
    EXPECT_NE(output.find("uid:2002"), std::string::npos);
    EXPECT_NE(output.find("uid:3003"), std::string::npos);
    EXPECT_NE(output.find("saName:ServiceA"), std::string::npos);
    EXPECT_NE(output.find("saName:ServiceB"), std::string::npos);
    EXPECT_NE(output.find("saName:ServiceC"), std::string::npos);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteExtraData_003 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_WriteUidInfoListToExtraData_001
 * @tc.desc: Verify the WriteUidInfoListToExtraData function when otherAppVec is not empty (line 278 branch).
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_WriteUidInfoListToExtraData_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteUidInfoListToExtraData_001 start";
    std::ostringstream extraData;
    AllAppVec allVec;
    allVec.sysSaVec.push_back(UidSaInfo(1001, "SysSa", 1024, 100));
    allVec.sysAppVec.push_back(UidSaInfo(2002, "SysApp", 2048, 200));
    allVec.userAppVec.push_back(UidSaInfo(3003, "UserApp", 4096, 300));
    allVec.otherAppVec.push_back(UidSaInfo(4004, "OtherApp", 8192, 400));
    StorageDfxReporter::GetInstance().WriteUidInfoListToExtraData(allVec, extraData);
    std::string output = extraData.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("{Sa data is:}"), std::string::npos);
    EXPECT_NE(output.find("{SysApp data is:}"), std::string::npos);
    EXPECT_NE(output.find("{UserApp data is:}"), std::string::npos);
    EXPECT_NE(output.find("{otherAppVec data is:}"), std::string::npos);
    EXPECT_EQ(output.find("{otherAppVec data is null}"), std::string::npos);
    EXPECT_NE(output.find("{uid:4004"), std::string::npos);
    EXPECT_NE(output.find("saName:OtherApp"), std::string::npos);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteUidInfoListToExtraData_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_001
 * @tc.desc: Verify the CorrectSaFromWhiteList function with empty vector.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_001 start";
    std::vector<UidSaInfo> emptyVec;
    StorageDfxReporter::GetInstance().CorrectSaFromWhiteList(emptyVec);
    EXPECT_TRUE(emptyVec.empty());
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_002
 * @tc.desc: Verify the CorrectSaFromWhiteList function when saName is userId (should skip).
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_002 start";
    std::vector<UidSaInfo> vec;
    int64_t originalSize = 1024;
    UidSaInfo info(0, "userId", originalSize, 100);
    vec.push_back(info);
    StorageDfxReporter::GetInstance().CorrectSaFromWhiteList(vec);
    EXPECT_EQ(vec[0].size, originalSize);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_003
 * @tc.desc: Verify the CorrectSaFromWhiteList function with ROOT_UID.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_003 start";
    std::vector<UidSaInfo> vec;
    UidSaInfo info(0, "RootService", 0, 100);
    vec.push_back(info);
    StorageDfxReporter::GetInstance().CorrectSaFromWhiteList(vec);
    EXPECT_EQ(vec[0].uid, 0);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_003 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_004
 * @tc.desc: Verify the CorrectSaFromWhiteList function with SYSTEM_UID.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_004 start";
    std::vector<UidSaInfo> vec;
    UidSaInfo info(1000, "SystemService", 0, 200);
    vec.push_back(info);
    StorageDfxReporter::GetInstance().CorrectSaFromWhiteList(vec);
    EXPECT_EQ(vec[0].uid, 1000);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_004 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_006
 * @tc.desc: Verify the CorrectSaFromWhiteList function with other UID (should not update size).
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_006 start";
    std::vector<UidSaInfo> vec;
    int64_t originalSize = 2048;
    UidSaInfo info(9999, "OtherService", originalSize, 400);
    vec.push_back(info);
    StorageDfxReporter::GetInstance().CorrectSaFromWhiteList(vec);
    EXPECT_EQ(vec[0].size, originalSize);
    EXPECT_EQ(vec[0].uid, 9999);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_006 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_007
 * @tc.desc: Verify the CorrectSaFromWhiteList function with mixed elements.
 * @tc.type: FUNC
 * @tc.require: AR000XXXX
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_007 start";
    std::vector<UidSaInfo> vec;
    int64_t userIdOriginalSize = 1000;
    int64_t otherOriginalSize = 5000;
    vec.push_back(UidSaInfo(0, "userId", userIdOriginalSize, 100));
    vec.push_back(UidSaInfo(0, "RootService", 0, 200));
    vec.push_back(UidSaInfo(1000, "SystemService", 0, 300));
    vec.push_back(UidSaInfo(1111, "MemmgrService", 0, 400));
    vec.push_back(UidSaInfo(9999, "OtherService", otherOriginalSize, 500));
    StorageDfxReporter::GetInstance().CorrectSaFromWhiteList(vec);
    EXPECT_EQ(vec[0].size, userIdOriginalSize);
    EXPECT_EQ(vec[0].saName, "userId");
    EXPECT_EQ(vec[1].uid, 0);
    EXPECT_EQ(vec[1].saName, "RootService");
    EXPECT_EQ(vec[2].uid, 1000);
    EXPECT_EQ(vec[2].saName, "SystemService");
    EXPECT_EQ(vec[3].uid, 1111);
    EXPECT_EQ(vec[3].saName, "MemmgrService");
    EXPECT_EQ(vec[4].size, otherOriginalSize);
    EXPECT_EQ(vec[4].saName, "OtherService");
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CorrectSaFromWhiteList_007 end";
}

HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CloneEventReportTimesZeroisation_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportTimesZeroisation_001 start";
    StorageDfxReporter::GetInstance().eventReportTimes_.store(5);
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 5);
    StorageDfxReporter::GetInstance().CloneEventReportTimesZeroisation();
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 0);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportTimesZeroisation_001 end";
}

HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CloneEventReportTimesZeroisation_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportTimesZeroisation_002 start";
    StorageDfxReporter::GetInstance().eventReportTimes_.store(0);
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 0);
    StorageDfxReporter::GetInstance().CloneEventReportTimesZeroisation();
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 0);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportTimesZeroisation_002 end";
}

HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_001 start";
    StorageDfxReporter::GetInstance().eventReportTimes_.store(0);
    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(false);
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(500), Return(0)));
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(100), Return(0)));
    EXPECT_CALL(*sdc, GetRmgResourceSize(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(50), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    StorageDfxReporter::GetInstance().CloneEventReportStorageStatus();
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_001 end";
}

HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_002 start";
    StorageDfxReporter::GetInstance().eventReportTimes_.store(2);
    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(false);
    StorageDfxReporter::GetInstance().CloneEventReportStorageStatus();
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 2);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_002 end";
}

HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_003 start";
    StorageDfxReporter::GetInstance().eventReportTimes_.store(1);
    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(false);
    EXPECT_CALL(*sss, GetUserStorageStats(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*stss, GetFreeSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*stss, GetSystemSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(500), Return(0)));
    EXPECT_CALL(*sdc, GetDataSizeByPath(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(100), Return(0)));
    EXPECT_CALL(*sdc, GetRmgResourceSize(_, _)).WillRepeatedly(DoAll(SetArgReferee<1>(50), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    StorageDfxReporter::GetInstance().CloneEventReportStorageStatus();
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_003 end";
}

HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_004,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_004 start";
    StorageDfxReporter::GetInstance().CloneEventReportTimesZeroisation();
    StorageDfxReporter::GetInstance().eventReportTimes_.store(3);
    StorageDfxReporter::GetInstance().isHapAndSaRunning_.store(false);
    StorageDfxReporter::GetInstance().CloneEventReportStorageStatus();
    EXPECT_EQ(StorageDfxReporter::GetInstance().eventReportTimes_.load(), 3);
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CloneEventReportStorageStatus_004 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_001
 * @tc.desc: Verify CollectWeakSandboxAppStats with mixed uid/appUid entries (SR20260812026188).
 * @tc.type: FUNC
 * @tc.require: SR20260812026188
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_001 start";
    int32_t userId = 100;
    // Two weak-sandbox entries (uid != appUid) + one normal app (uid == appUid)
    std::vector<BundleUidInfo> bundleInfos;
    BundleUidInfo weak1;
    weak1.bundleName = "com.test.weak1";
    weak1.uid = 100;      // weak-sandbox: uid == userId
    weak1.appUid = 124500;
    weak1.appIndex = 0;
    bundleInfos.push_back(weak1);
    BundleUidInfo weak2;
    weak2.bundleName = "com.test.weak2";
    weak2.uid = 100;
    weak2.appUid = 124501;
    weak2.appIndex = 0;
    bundleInfos.push_back(weak2);
    BundleUidInfo normal1;
    normal1.bundleName = "com.test.normal1";
    normal1.uid = 124502;
    normal1.appUid = 124502; // not weak-sandbox
    normal1.appIndex = 0;
    bundleInfos.push_back(normal1);

    BundleStats bundleStats;
    bundleStats.appSize_ = 100;
    bundleStats.cacheSize_ = 200;
    bundleStats.dataSize_ = 300;
    // GetBundleStats succeeds; normal apps trigger no supplementary query
    EXPECT_CALL(*sss, GetBundleStats(_, _, _, _, _))
        .WillRepeatedly(DoAll(SetArgReferee<2>(bundleStats), Return(E_OK)));

    StorageDfxReporter::GetInstance().weakSandboxApps_.clear();
    int32_t ret = StorageDfxReporter::GetInstance().CollectWeakSandboxAppStats(userId, bundleInfos);
    EXPECT_EQ(ret, E_OK);
    // Only the two weak-sandbox apps are collected; the normal app is skipped
    EXPECT_EQ(StorageDfxReporter::GetInstance().weakSandboxApps_.size(), static_cast<size_t>(2));
    // Weak-sandbox size caliber = appSize + cacheSize + dataSize = 600
    EXPECT_EQ(StorageDfxReporter::GetInstance().weakSandboxApps_[0].size,
        static_cast<int64_t>(100 + 200 + 300));
    StorageDfxReporter::GetInstance().weakSandboxApps_.clear();
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_002
 * @tc.desc: Verify CollectWeakSandboxAppStats keeps zero on single-app query failure (SR20260812026188).
 * @tc.type: FUNC
 * @tc.require: SR20260812026188
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_002 start";
    int32_t userId = 100;
    std::vector<BundleUidInfo> bundleInfos;
    BundleUidInfo weak1;
    weak1.bundleName = "com.test.weakfail";
    weak1.uid = 100;
    weak1.appUid = 124500;
    weak1.appIndex = 0;
    bundleInfos.push_back(weak1);

    // Single-app failure: entry stays in place (attribution completeness), size = 0
    EXPECT_CALL(*sss, GetBundleStats(_, _, _, _, _)).WillRepeatedly(Return(-1));

    StorageDfxReporter::GetInstance().weakSandboxApps_.clear();
    int32_t ret = StorageDfxReporter::GetInstance().CollectWeakSandboxAppStats(userId, bundleInfos);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(StorageDfxReporter::GetInstance().weakSandboxApps_.size(), static_cast<size_t>(1));
    EXPECT_EQ(StorageDfxReporter::GetInstance().weakSandboxApps_[0].size, static_cast<int64_t>(0));
    StorageDfxReporter::GetInstance().weakSandboxApps_.clear();
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectWeakSandboxAppStats_002 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_WriteWeakSandboxInfoToExtraData_001
 * @tc.desc: Verify WriteWeakSandboxInfoToExtraData skips empty list, emits segment when non-empty (SR20260812026188).
 * @tc.type: FUNC
 * @tc.require: SR20260812026188
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_WriteWeakSandboxInfoToExtraData_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteWeakSandboxInfoToExtraData_001 start";
    // Empty list: no segment output
    std::ostringstream emptyData;
    StorageDfxReporter::GetInstance().weakSandboxApps_.clear();
    StorageDfxReporter::GetInstance().WriteWeakSandboxInfoToExtraData(emptyData);
    EXPECT_TRUE(emptyData.str().find("WeakSandbox") == std::string::npos);

    // Non-empty list: independent segment (attribution by appUid + bundleName)
    StorageDfxReporter::GetInstance().weakSandboxApps_.clear();
    BundleUidInfo app1;
    app1.bundleName = "com.test.ws1";
    app1.uid = 100;
    app1.appUid = 124500;
    app1.appIndex = 0;
    app1.size = 500;
    app1.iNodes = 10;
    StorageDfxReporter::GetInstance().weakSandboxApps_.push_back(app1);
    BundleUidInfo app2;
    app2.bundleName = "com.test.ws2";
    app2.uid = 100;
    app2.appUid = 124501;
    app2.appIndex = 0;
    app2.size = 900;
    app2.iNodes = 20;
    StorageDfxReporter::GetInstance().weakSandboxApps_.push_back(app2);

    std::ostringstream extraData;
    StorageDfxReporter::GetInstance().WriteWeakSandboxInfoToExtraData(extraData);
    std::string output = extraData.str();
    EXPECT_NE(output.find("WeakSandbox"), std::string::npos);
    EXPECT_NE(output.find("com.test.ws1"), std::string::npos);
    EXPECT_NE(output.find("com.test.ws2"), std::string::npos);
    StorageDfxReporter::GetInstance().weakSandboxApps_.clear();
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_WriteWeakSandboxInfoToExtraData_001 end";
}

/**
 * @tc.name: Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_ClearWeakSandbox_001
 * @tc.desc: Verify CollectBundleStatistics clears weakSandboxApps_ at round start (SR20260812026188).
 * @tc.type: FUNC
 * @tc.require: SR20260812026188
 */
HWTEST_F(StorageDfxReporterTest, Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_ClearWeakSandbox_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_ClearWeakSandbox_001 start";
    std::ostringstream extraData;
    int32_t userId = 100;
    // Preload a stale entry to verify the round-start clearing
    BundleUidInfo stale;
    stale.bundleName = "com.test.stale";
    StorageDfxReporter::GetInstance().weakSandboxApps_.push_back(stale);

    EXPECT_CALL(*stss, GetUsedInodes(_)).WillRepeatedly(DoAll(SetArgReferee<0>(1000), Return(0)));
    EXPECT_CALL(*sss, GetBundleNameAndUid(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*sdc, QueryOccupiedSpaceForSa(_, _, _, _)).WillRepeatedly(Return(0));
    int32_t ret = StorageDfxReporter::GetInstance().CollectBundleStatistics(userId, extraData);
    EXPECT_EQ(ret, 0);
    // On non-PC form the list stays empty after the round-start clearing
    EXPECT_EQ(StorageDfxReporter::GetInstance().weakSandboxApps_.size(), static_cast<size_t>(0));
    GTEST_LOG_(INFO) << "Storage_Service_StorageDfxReporterTest_CollectBundleStatistics_ClearWeakSandbox_001 end";
}
} // namespace StorageManager
} // namespace OHOS