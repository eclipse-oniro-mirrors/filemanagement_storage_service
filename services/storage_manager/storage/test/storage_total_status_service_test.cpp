/*
 * Copyright (c) 2021-2026 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "bundle_mgr_interface.h"
#include "directory_ex.h"
#include "ipc_skeleton.h"
#include "nativetoken_kit.h"
#include "storage/bundle_manager_adapter_proxy.h"
#include "storage/bundle_manager_connector.h"
#include "storage/storage_monitor_service.h"
#include "storage/storage_status_manager.h"
#include "storage/storage_total_status_service.h"
#include "storage_service_errno.h"
#include "token_setproc.h"
#include "utils/string_utils.h"

static void SetNativeToken()
{
    uint64_t tokenId;
    const char *perms[] = {
        "ohos.permission.READ_MEDIA",
        "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
        "ohos.permission.MANAGE_LOCAL_ACCOUNTS",
        "ohos.permission.ACCESS_SCREEN_LOCK",
        "ohos.permission.ACCESS_SCREEN_LOCK_INNER",
        "ohos.permission.REMOVE_CACHE_FILES",
        "ohos.permission.PUBLISH_SYSTEM_COMMON_EVENT"
    };
    const char* acls[] = { "ohos.permission.ACCESS_SCREEN_LOCK_INNER" };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 7,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = acls,
        .aplStr = "system_basic",
    };

    infoInstance.processName = "storage_service_test";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

namespace {
using namespace std;
using namespace OHOS;
using namespace StorageManager;
using namespace StorageDaemon;
struct DirInfo {
    const std::string path;
    mode_t mode;
    uid_t uid;
    gid_t gid;
};
constexpr uid_t OID_ROOT = 0;
static constexpr int MODE_0711 = 0711;
static constexpr int MODE_02771 = 02771;
constexpr uid_t OID_FILE_MANAGER = 1006;
constexpr uid_t OID_USER_DATA_RW = 1008;
constexpr uid_t OID_DFS = 1009;
const std::vector<DirInfo> virtualDir_{{"/storage/media/%d", MODE_0711, OID_USER_DATA_RW, OID_USER_DATA_RW},
        {"/storage/media/%d/local", MODE_0711, OID_USER_DATA_RW, OID_USER_DATA_RW},
        {"/storage/cloud", MODE_0711, OID_ROOT, OID_ROOT},
        {"/storage/cloud/%d", MODE_0711, OID_USER_DATA_RW, OID_USER_DATA_RW},
        {"/mnt/share/", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/share/%d/", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/data/%d/", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/data/%d/cloud", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/data/%d/cloud_fuse", MODE_0711, OID_DFS, OID_DFS},
        {"/mnt/data/%d/userExternal", MODE_02771, OID_FILE_MANAGER, OID_FILE_MANAGER},
        {"/mnt/data/%d/hmdfs", MODE_0711, OID_FILE_MANAGER, OID_FILE_MANAGER},
        {"/mnt/hmdfs/", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/hmdfs/%d/", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/hmdfs/%d/cloud", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/hmdfs/%d/account", MODE_0711, OID_ROOT, OID_ROOT},
        {"/mnt/hmdfs/%d/non_account", MODE_0711, OID_ROOT, OID_ROOT}};
int32_t g_callingAppUid = 5523;
ErrCode g_getBundleInodeCountRet = ERR_OK;
std::string g_bundleName = "com.test.app";
ErrCode g_getNameIndexRet = ERR_OK;
int32_t g_appIndex = 0;
constexpr uint64_t MOCK_BUNDLE_INODE_COUNT = 100;
} // namespace

namespace OHOS {
pid_t IPCSkeleton::GetCallingAppUid()
{
    return g_callingAppUid;
}
} // namespace OHOS

namespace OHOS::StorageManager {
ErrCode BundleManagerAdapterProxy::GetNameAndIndexForUid(int32_t uid, std::string &bundleName, int32_t &appIndex)
{
    bundleName = g_bundleName;
    appIndex = g_appIndex;
    return g_getNameIndexRet;
}

// Define mock GetBundleInodeCount for testing GetBundleInodeCount failure branch
ErrCode BundleManagerAdapterProxy::GetBundleInodeCount(const std::string &bundleName,
    int32_t appIndex, int32_t userId, uint64_t &inodeCount)
{
    return g_getBundleInodeCountRet;
}

// Link-time stub (symbol takes precedence over libstorage_manager.z.so, same pattern as
// storage_monitor_service_test): on real devices the original implementation gets the
// real BMS proxy via samgr and bypasses the stub above, breaking the g_* mock values.
// The stub returns a controllable proxy so the queries go through this file's globals.
class TestBundleMgrStub : public AppExecFwk::IBundleMgr {
public:
    ErrCode GetNameAndIndexForUid(const int32_t uid, std::string &bundleName, int32_t &appIndex) override
    {
        bundleName = g_bundleName;
        appIndex = g_appIndex;
        return g_getNameIndexRet;
    }
    ErrCode GetBundleInodeCount(const std::string &bundleName, int32_t appIndex, int32_t userId,
        uint64_t &inodeCount) override
    {
        if (g_getBundleInodeCountRet != ERR_OK) {
            return g_getBundleInodeCountRet;
        }
        inodeCount = MOCK_BUNDLE_INODE_COUNT;
        return ERR_OK;
    }
    bool GetBundleNameForUid(const int uid, std::string &bundleName) override
    {
        bundleName = g_bundleName;
        return true;
    }
    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

BundleMgrConnector::BundleMgrConnector() {}
BundleMgrConnector::~BundleMgrConnector() {}

sptr<AppExecFwk::IBundleMgr> BundleMgrConnector::GetBundleMgrProxy()
{
    static sptr<AppExecFwk::IBundleMgr> stub = new TestBundleMgrStub();
    return stub;
}

int32_t BundleMgrConnector::ResetBundleMgrProxy()
{
    return E_OK;
}
}

class StorageTotalStatusServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void) { SetNativeToken(); };
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetSystemSize_0000
 * @tc.name: Storage_total_status_service_GetSystemSize_0000
 * @tc.desc: Test function of GetSystemSize interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetSystemSize_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetSystemSize_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t systemSize;
    int32_t result = service.GetSystemSize(systemSize);
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetSystemSize_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetTotalSize_0000
 * @tc.name: Storage_total_status_service_GetTotalSize_0000
 * @tc.desc: Test function of GetTotalSize interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0371
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetTotalSize_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetTotalSize_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t totalSize;
    int32_t result = service.GetTotalSize(totalSize);
    EXPECT_EQ(result, E_OK);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetTotalSize_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetFreeSize_0000
 * @tc.name: Storage_total_status_service_GetFreeSize_0000
 * @tc.desc: Test function of GetFreeSize interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0371
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetFreeSize_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetFreeSize_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t freeSize;
    int32_t result = service.GetFreeSize(freeSize);
    EXPECT_EQ(result, E_OK);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetFreeSize_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_status_service_GetBundleStats_0000
 * @tc.name: Storage_status_service_GetBundleStats_0000
 * @tc.desc: Test function of GetBundleStats interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_status_GetBundleStats_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_status_service_GetBundleStats_0000";
    StorageStatusManager &service = StorageStatusManager::GetInstance();
    string pkgName = "com.test";
    BundleStats bundleStats;
    int32_t result = service.GetBundleStats(pkgName, bundleStats, 0, 0);
    EXPECT_EQ(result, E_BUNDLEMGR_ERROR);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_status_service_GetBundleStats_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_status_service_GetBundleStats_0001
 * @tc.name: Storage_status_service_GetBundleStats_0001
 * @tc.desc: Test function of GetBundleStats interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_status_GetBundleStats_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_status_service_GetBundleStats_0001";
    StorageStatusManager &service = StorageStatusManager::GetInstance();
    int userId = 100;
    string pkgName = "com.test";
    BundleStats bundleStats;
    int32_t result = service.GetBundleStats(pkgName, userId, bundleStats, 0, 0);
    EXPECT_EQ(result, E_BUNDLEMGR_ERROR);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_status_service_GetBundleStats_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_status_service_GetCurrentBundleStats_0000
 * @tc.name: Storage_status_service_GetCurrentBundleStats_0000
 * @tc.desc: Test function of GetCurrentBundleStats when caller is not a hap.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_status_GetCurrentBundleStats_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_status_service_GetCurrentBundleStats_0000";
    StorageStatusManager &service = StorageStatusManager::GetInstance();
    BundleStats bundleStats;
    int32_t result = service.GetCurrentBundleStats(bundleStats, 0);
    EXPECT_NE(result, E_GET_BUNDLE_NAME_FAILED);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_status_service_GetCurrentBundleStats_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_status_service_ResetBundleMgrProxy_0000
 * @tc.name: Storage_total_status_ResetBundleMgrProxy_0000
 * @tc.desc: Test function of ResetBundleMgrProxy interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0F7I
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_ResetBundleMgrProxy_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_ResetBundleMgrProxy_0000";
    BundleMgrConnector& service = BundleMgrConnector::GetInstance();
    int64_t result = service.ResetBundleMgrProxy();
    EXPECT_GE(result, 0);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_ResetBundleMgrProxy_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_status_service_StartStorageMonitorTask_0000
 * @tc.name: Storage_status_service_StartStorageMonitorTask_0000
 * @tc.desc: Test function of StartStorageMonitorTask interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_status_StartStorageMonitorTask_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_status_service_StartStorageMonitorTask_0000";
    StorageMonitorService& service = StorageMonitorService::GetInstance();
    service.StartStorageMonitorTask();
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_status_service_StartStorageMonitorTask_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_status_service_Execute_0000
 * @tc.name: Storage_status_service_Execute_0000
 * @tc.desc: Test function of Execute interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_status_Execute_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_status_service_Execute_0000";
    StorageMonitorService& service = StorageMonitorService::GetInstance();
    service.Execute();
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_status_service_Execute_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_status_service_MonitorAndManageStorage_0000
 * @tc.name: Storage_status_service_MonitorAndManageStorage_0000
 * @tc.desc: Test function of MonitorAndManageStorage interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_status_MonitorAndManageStorage_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_status_service_MonitorAndManageStorage_0000";
    StorageMonitorService& service = StorageMonitorService::GetInstance();
    service.MonitorAndManageStorage();
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_status_service_MonitorAndManageStorage_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetInodeOfPath_0000
 * @tc.name: Storage_total_status_service_GetInodeOfPath_0000
 * @tc.desc: Test function of GetInodeOfPath interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0371
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetInodeOfPath_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetInodeOfPath_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int32_t type = static_cast<int32_t>(StorageStatType::TOTAL);
    int64_t inodeCnt = 0;
    int32_t result = service.GetInodeOfPath(PATH_DATA, type, inodeCnt);
    EXPECT_EQ(result, E_OK);

    type = static_cast<int32_t>(StorageStatType::FREE);
    result = service.GetInodeOfPath(PATH_DATA, type, inodeCnt);
    EXPECT_EQ(result, E_OK);

    type = static_cast<int32_t>(StorageStatType::USED);
    result = service.GetInodeOfPath(PATH_DATA, type, inodeCnt);
    EXPECT_EQ(result, E_OK);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetInodeOfPath_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetInodeOfPath_0001
 * @tc.name: Storage_total_status_service_GetInodeOfPath_0001
 * @tc.desc: Test GetInodeOfPath with nullptr path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0371
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetInodeOfPath_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetInodeOfPath_0001";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int32_t type = static_cast<int32_t>(StorageStatType::TOTAL);
    int64_t inodeCnt = 0;
    int32_t result = service.GetInodeOfPath(nullptr, type, inodeCnt);
    EXPECT_EQ(result, E_PARAMS_NULLPTR_ERR);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetInodeOfPath_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetUsedInodes_0000
 * @tc.name: Storage_total_status_service_GetUsedInodes_0000
 * @tc.desc: Test function of GetUsedInodesinterface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetUsedInodes_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetUsedInodes_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t usedInodes = 0;
    int32_t result = service.GetUsedInodes(usedInodes);
    EXPECT_EQ(result, E_OK);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetUsedInodes_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetTotalInodes_0000
 * @tc.name: Storage_total_status_service_GetTotalInodes_0000
 * @tc.desc: Test function of GetTotalInodes interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725395
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetTotalInodes_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetTotalInodes_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t totalInodes = 0;
    int32_t result = service.GetTotalInodes(totalInodes);
    EXPECT_EQ(result, E_OK);
    EXPECT_GT(totalInodes, 0);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetTotalInodes_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetFreeInodes_0000
 * @tc.name: Storage_total_status_service_GetFreeInodes_0000
 * @tc.desc: Test function of GetFreeInodes interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725395
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetFreeInodes_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetFreeInodes_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t freeInodes = 0;
    int32_t result = service.GetFreeInodes(freeInodes);
    EXPECT_EQ(result, E_OK);
    EXPECT_GE(freeInodes, 0);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetFreeInodes_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetCurrentBundleInodes_0000
 * @tc.name: Storage_total_status_service_GetCurrentBundleInodes_0000
 * @tc.desc: Test function of GetCurrentBundleInodes when caller is not a hap.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725395
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetCurrentBundleInodes_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetCurrentBundleInodes_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t curInodes = 0;
    int32_t result = service.GetCurrentBundleInodes(curInodes);
    EXPECT_EQ(result, E_OK);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetCurrentBundleInodes_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetCurrentBundleInodes_0001
 * @tc.name: Storage_total_status_service_GetCurrentBundleInodes_0001
 * @tc.desc: Test function of GetCurrentBundleInodes when appUid < 0.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725395
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetCurrentBundleInodes_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetCurrentBundleInodes_0001";
    g_callingAppUid = -1;

    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t curInodes = 0;
    int32_t result = service.GetCurrentBundleInodes(curInodes);

    EXPECT_EQ(result, E_GET_BUNDLE_INODES_ERROR);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetCurrentBundleInodes_0001";

    g_callingAppUid = 5523;
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetCurrentBundleInodes_0002
 * @tc.name: Storage_total_status_service_GetCurrentBundleInodes_0002
 * @tc.desc: Test function of GetCurrentBundleInodes when GetBundleInodeCount fails (ret != ERR_OK).
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725395
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetCurrentBundleInodes_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetCurrentBundleInodes_0002";
    g_bundleName = "";
    g_getNameIndexRet = ERR_APPEXECFWK_PARCEL_ERROR;

    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t curInodes = 0;
    int32_t result = service.GetCurrentBundleInodes(curInodes);

    EXPECT_EQ(result, E_GET_BUNDLE_INODES_ERROR);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetCurrentBundleInodes_0002";

    g_bundleName = "com.test.app";
    g_getNameIndexRet = ERR_OK;
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetCurrentBundleInodes_0003
 * @tc.name: Storage_total_status_service_GetCurrentBundleInodes_0003
 * @tc.desc: Test function of GetCurrentBundleInodes when GetBundleInodeCount fails (ret != ERR_OK).
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725395
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetCurrentBundleInodes_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetCurrentBundleInodes_0003";
    g_getBundleInodeCountRet = ERR_APPEXECFWK_PARCEL_ERROR;
    g_callingAppUid = 5523;

    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t curInodes = 0;
    int32_t result = service.GetCurrentBundleInodes(curInodes);

    EXPECT_TRUE(result == E_GET_BUNDLE_INODES_ERROR);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetCurrentBundleInodes_0003";

    g_callingAppUid = 5523;
    g_getBundleInodeCountRet = ERR_OK;
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetDataTotalSize_0000
 * @tc.name: Storage_total_status_service_GetDataTotalSize_0000
 * @tc.desc: Test function of GetDataTotalSize interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetDataTotalSize_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetDataTotalSize_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t dataTotalSize = 0;
    int32_t result = service.GetDataTotalSize(dataTotalSize);
    EXPECT_EQ(result, E_OK);
    EXPECT_GT(dataTotalSize, 0);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetDataTotalSize_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetFreeSize_WithMetadata_0000
 * @tc.name: Storage_total_status_service_GetFreeSize_WithMetadata_0000
 * @tc.desc: Test GetFreeSize with metadata calculation (graceful degradation on Linux without HMFS).
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetFreeSize_WithMetadata_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_GetFreeSize_WithMetadata_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t freeSize = 0;
    int32_t result = service.GetFreeSize(freeSize);
    // On Linux without HMFS, GetMetaDataSize will fail but GetFreeSize should still return E_OK (graceful degradation)
    EXPECT_EQ(result, E_OK);
    EXPECT_GE(freeSize, 0);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_GetFreeSize_WithMetadata_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetRawFreeSize_0000
 * @tc.name: Storage_total_status_service_GetRawFreeSize_0000
 * @tc.desc: Test function of GetRawFreeSize interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetRawFreeSize_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetRawFreeSize_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t rawFreeSize = 0;
    int32_t result = service.GetRawFreeSize(rawFreeSize);
    EXPECT_EQ(result, E_OK);
    EXPECT_GE(rawFreeSize, 0);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetRawFreeSize_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetRawFreeSize_0001
 * @tc.name: Storage_total_status_service_GetRawFreeSize_0001
 * @tc.desc: Test GetRawFreeSize returns value not less than 0 and GetFreeSize >= GetRawFreeSize.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetRawFreeSize_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetRawFreeSize_0001";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t rawFreeSize = 0;
    int64_t freeSize = 0;
    int32_t rawRet = service.GetRawFreeSize(rawFreeSize);
    int32_t freeRet = service.GetFreeSize(freeSize);
    EXPECT_EQ(rawRet, E_OK);
    EXPECT_EQ(freeRet, E_OK);
    // GetFreeSize = rawFreeSize + freeMetadata (>= rawFreeSize when metadata available)
    EXPECT_GE(freeSize, rawFreeSize);
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetRawFreeSize_0001";
}

#ifdef SLC_DYNAMIC_DISPLAY
/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetSlcSize_0000
 * @tc.name: Storage_total_status_service_GetSlcSize_0000
 * @tc.desc: Test GetSlcSize is environment-independent. Whether the device has an SLC partition
 *           (E_OK) or not (E_ERR) is a legitimate outcome; assert the corresponding contract.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetSlcSize_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetSlcSize_0000";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t slcPartitionSize = 0;
    int64_t slcCapacityLoss = 0;
    int32_t result = service.GetSlcSize(slcPartitionSize, slcCapacityLoss);
    if (result == E_OK) {
        // SLC partition present: valid partition size, non-negative and MB-aligned capacity loss
        EXPECT_GT(slcPartitionSize, 0);
        EXPECT_GE(slcCapacityLoss, 0);
        EXPECT_EQ(slcCapacityLoss % (1024 * 1024), 0);
    } else if (result == E_ERR) {
        // No SLC partition: graceful degradation resets both outputs to 0
        EXPECT_EQ(slcPartitionSize, 0);
        EXPECT_EQ(slcCapacityLoss, 0);
    } else {
        FAIL() << "GetSlcSize returned unexpected result: " << result;
    }
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetSlcSize_0000";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetSlcSize_0001
 * @tc.name: Storage_total_status_service_GetSlcSize_0001
 * @tc.desc: Test GetSlcSize overwrites pre-set non-zero output parameters with the real result,
 *           branching on the return value so the case is environment-independent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetSlcSize_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetSlcSize_0001";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    constexpr int64_t presetPartitionSize = 12345;
    constexpr int64_t presetCapacityLoss = 67890;
    int64_t slcPartitionSize = presetPartitionSize;
    int64_t slcCapacityLoss = presetCapacityLoss;
    int32_t result = service.GetSlcSize(slcPartitionSize, slcCapacityLoss);
    if (result == E_OK) {
        // Outputs must be overwritten by the real SLC figures, not left at the preset values
        EXPECT_NE(slcPartitionSize, presetPartitionSize);
        EXPECT_GT(slcPartitionSize, 0);
        EXPECT_NE(slcCapacityLoss, presetCapacityLoss);
        EXPECT_GE(slcCapacityLoss, 0);
        EXPECT_EQ(slcCapacityLoss % (1024 * 1024), 0);
    } else if (result == E_ERR) {
        EXPECT_EQ(slcPartitionSize, 0);
        EXPECT_EQ(slcCapacityLoss, 0);
    } else {
        FAIL() << "GetSlcSize returned unexpected result: " << result;
    }
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetSlcSize_0001";
}

/**
 * @tc.number: SUB_STORAGE_Storage_total_status_service_GetSlcSize_0002
 * @tc.name: Storage_total_status_service_GetSlcSize_0002
 * @tc.desc: Test GetSlcSize returns stable result across repeated calls, branching on the
 *           return value so the case is environment-independent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR20260114725643
 */
HWTEST_F(StorageTotalStatusServiceTest, Storage_total_status_GetSlcSize_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-begin Storage_total_status_service_GetSlcSize_0002";
    StorageTotalStatusService& service = StorageTotalStatusService::GetInstance();
    int64_t firstPartitionSize = 0;
    int64_t firstCapacityLoss = 0;
    int32_t firstResult = service.GetSlcSize(firstPartitionSize, firstCapacityLoss);
    int64_t secondPartitionSize = 0;
    int64_t secondCapacityLoss = 0;
    int32_t secondResult = service.GetSlcSize(secondPartitionSize, secondCapacityLoss);
    ASSERT_EQ(firstResult, secondResult) << "GetSlcSize returned inconsistent results across calls";
    if (firstResult == E_OK) {
        EXPECT_EQ(firstPartitionSize, secondPartitionSize);
        EXPECT_EQ(firstCapacityLoss, secondCapacityLoss);
        EXPECT_GT(firstPartitionSize, 0);
        EXPECT_GE(firstCapacityLoss, 0);
        EXPECT_EQ(firstCapacityLoss % (1024 * 1024), 0);
    } else if (firstResult == E_ERR) {
        EXPECT_EQ(firstPartitionSize, 0);
        EXPECT_EQ(secondPartitionSize, 0);
        EXPECT_EQ(firstCapacityLoss, 0);
        EXPECT_EQ(secondCapacityLoss, 0);
    } else {
        FAIL() << "GetSlcSize returned unexpected result: " << firstResult;
    }
    GTEST_LOG_(INFO) << "StorageTotalStatusServiceTest-end Storage_total_status_service_GetSlcSize_0002";
}
#endif // SLC_DYNAMIC_DISPLAY