/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "adapter/bundle_manager_adapter_proxy.h"
#include "bundle_mgr_interface.h"
#include "iremote_broker.h"
#include "iremote_stub.h"
#include "message_parcel_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace OHOS::StorageSpaceManager::Test {
using namespace OHOS::StorageSpaceManager;
using namespace testing;
using namespace testing::ext;
using namespace std;

class BundleMgrMock : public IRemoteStub<AppExecFwk::IBundleMgr> {
public:
    int code_;
    BundleMgrMock() : code_(0) {}
    virtual ~BundleMgrMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));
};

class BundleManagerAdapterProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

public:
    static inline shared_ptr<BundleManagerAdapterProxy> proxy_ = nullptr;
    static inline sptr<BundleMgrMock> mock_ = nullptr;
    static inline shared_ptr<MessageParcelMock> messageParcelMock_ = nullptr;
};

void BundleManagerAdapterProxyTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
}

void BundleManagerAdapterProxyTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase";
}

void BundleManagerAdapterProxyTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp";
    mock_ = sptr(new BundleMgrMock());
    proxy_ = make_shared<BundleManagerAdapterProxy>(mock_);
    messageParcelMock_ = make_shared<MessageParcelMock>();
    MessageParcelMock::messageParcel = messageParcelMock_;
}

void BundleManagerAdapterProxyTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
    mock_ = nullptr;
    proxy_ = nullptr;
    messageParcelMock_ = nullptr;
    MessageParcelMock::messageParcel = nullptr;
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleNameForUid_0000
 * @tc.desc: The execution of the ConnectDfs failed.
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetBundleNameForUid_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleNameForUid_0000 Start";

    string bundleName = "default-bundleName";
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->GetBundleNameForUid(0, bundleName);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleNameForUid(0, bundleName);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetBundleNameForUid(0, bundleName);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(false));
    ret = proxy_->GetBundleNameForUid(0, bundleName);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    ret = proxy_->GetBundleNameForUid(0, bundleName);
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_ConnectDfs_0100 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0000
 * @tc.desc: The execution of the ConnectDfs failed.
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0000 Start";

    auto ret = proxy_->CleanBundleCacheFilesAutomatic(0);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_INVALID_PARAMETER);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    ret = proxy_->CleanBundleCacheFilesAutomatic(100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(false));
    ret = proxy_->CleanBundleCacheFilesAutomatic(100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->CleanBundleCacheFilesAutomatic(100);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_IPC_TRANSACTION);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(0));
    ret = proxy_->CleanBundleCacheFilesAutomatic(100);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_ConnectDfs_0100 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleInfosV9_0000
 * @tc.desc: The execution of the ConnectDfs failed.
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetBundleInfosV9_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleInfosV9_0000 Start";

    std::vector<BundleInfo> vector = {};

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->GetBundleInfosV9(0, vector, 100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleInfosV9(0, vector, 100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
    ret = proxy_->GetBundleInfosV9(0, vector, 100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetBundleInfosV9(0, vector, 100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetBundleInfosV9(0, vector, 100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(0)).WillOnce(Return(0));
    ret = proxy_->GetBundleInfosV9(0, vector, 100);
    EXPECT_EQ(ret, ERR_OK);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_ConnectDfs_0100 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetNameAndIndexForUid_0000
 * @tc.desc: Test GetNameAndIndexForUid function with various scenarios
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetNameAndIndexForUid_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetNameAndIndexForUid_0000 Start";

    std::string bundleName;
    int32_t appIndex = 0;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->GetNameAndIndexForUid(1000, bundleName, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->GetNameAndIndexForUid(1000, bundleName, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetNameAndIndexForUid(1000, bundleName, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_SERVICE_NOT_READY));
    ret = proxy_->GetNameAndIndexForUid(1000, bundleName, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SERVICE_NOT_READY);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK)).WillOnce(Return(1));
    EXPECT_CALL(*messageParcelMock_, ReadString()).WillOnce(Return("com.example.app"));
    ret = proxy_->GetNameAndIndexForUid(1000, bundleName, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(bundleName, "com.example.app");
    EXPECT_EQ(appIndex, 1);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetNameAndIndexForUid_0000 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleStats_0000
 * @tc.desc: Test GetBundleStats function with various scenarios
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetBundleStats_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleStats_0000 Start";

    std::vector<int64_t> bundleStats;
    std::string bundleName = "com.example.app";

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
    ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleStats_0000 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleStats_0001
 * @tc.desc: Test GetBundleStats function with various scenarios
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetBundleStats_0001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleStats_0001 Start";

    std::vector<int64_t> bundleStats;
    std::string bundleName = "com.example.app";

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(false));
    auto ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt64Vector(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt64Vector(_)).WillOnce(Return(true));
    ret = proxy_->GetBundleStats(bundleName, 100, bundleStats, 0, 1);
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleStats_0001 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CreateBundleDataDirWithEl_0000
 * @tc.desc: Test CreateBundleDataDirWithEl function with various scenarios
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_CreateBundleDataDirWithEl_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CreateBundleDataDirWithEl_0000 Start";

    DataDirEl dirEl = DataDirEl::EL1;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->CreateBundleDataDirWithEl(100, dirEl);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->CreateBundleDataDirWithEl(100, dirEl);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint8(_)).WillOnce(Return(false));
    ret = proxy_->CreateBundleDataDirWithEl(100, dirEl);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint8(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->CreateBundleDataDirWithEl(100, dirEl);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint8(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_SERVICE_NOT_READY));
    ret = proxy_->CreateBundleDataDirWithEl(100, dirEl);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SERVICE_NOT_READY);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint8(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    ret = proxy_->CreateBundleDataDirWithEl(100, dirEl);
    EXPECT_EQ(ret, ERR_OK);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CreateBundleDataDirWithEl_0000 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetAllBundleStats_0000
 * @tc.desc: Test GetAllBundleStats function with various scenarios
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetAllBundleStats_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetAllBundleStats_0000 Start";

    std::vector<int64_t> bundleStats;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->GetAllBundleStats(100, bundleStats);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->GetAllBundleStats(100, bundleStats);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetAllBundleStats(100, bundleStats);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(false));
    ret = proxy_->GetAllBundleStats(100, bundleStats);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt64Vector(_)).WillOnce(Return(false));
    ret = proxy_->GetAllBundleStats(100, bundleStats);
    EXPECT_EQ(ret, false);

    std::vector<int64_t> expectedStats = {1024, 2048, 4096, 8192, 16384};
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt64Vector(_)).WillOnce(DoAll(
        SetArgPointee<0>(expectedStats), Return(true)));
    ret = proxy_->GetAllBundleStats(100, bundleStats);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(bundleStats.size(), 5);
    EXPECT_EQ(bundleStats[0], 1024);
    EXPECT_EQ(bundleStats[1], 2048);
    EXPECT_EQ(bundleStats[2], 4096);
    EXPECT_EQ(bundleStats[3], 8192);
    EXPECT_EQ(bundleStats[4], 16384);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetAllBundleStats_0000 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0000
 * @tc.desc: Test InnerGetVectorFromParcelIntelligent with zero data size
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0000 Start";

    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;

    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(0));

    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(parcelableInfos.empty());

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0000 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0001
 * @tc.desc: Test InnerGetVectorFromParcelIntelligent with small data size
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_0001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0001 Start";

    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;
    const size_t dataSize = 1024; // Small data size

    // Simulate small data case
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(dataSize));
    EXPECT_CALL(*messageParcelMock_, ReadRawData(_)).WillOnce(Return(nullptr)); // Return null to test error path

    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0001 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0002
 * @tc.desc: Test InnerGetVectorFromParcelIntelligent with large data size using ashmem
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_0002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0002 Start";

    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;
    const size_t largeDataSize = MessageParcel::MAX_RAWDATA_SIZE + 1; // Force ashmem path

    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(largeDataSize));

    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0002 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0003
 * @tc.desc: Test InnerGetVectorFromParcelIntelligent with valid small data
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_0003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0003 Start";

    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;
    const size_t dataSize = 1024;
    char buffer[dataSize] = {0};

    // Create a valid message parcel structure
    // First int32_t: number of BundleInfo entries (0 in this case)
    *reinterpret_cast<int32_t*>(buffer) = 0;

    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(dataSize)).WillOnce(Return(0));
    EXPECT_CALL(*messageParcelMock_, ReadRawData(_)).WillOnce(Return(buffer));

    // Mock ParseFrom to succeed
    EXPECT_CALL(*messageParcelMock_, ParseFrom(_, _)).WillOnce(Return(true));

    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(parcelableInfos.empty());

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0003 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0004
 * @tc.desc: Test InnerGetVectorFromParcelIntelligent with ParseFrom failure
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_0004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0004 Start";

    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;
    const size_t dataSize = 1024;
    char buffer[dataSize] = {0};

    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(dataSize));
    EXPECT_CALL(*messageParcelMock_, ReadRawData(_)).WillOnce(Return(buffer));
    EXPECT_CALL(*messageParcelMock_, ParseFrom(_, _)).WillOnce(Return(false));

    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0004 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetData_0000
 * @tc.desc: Test GetData with null data pointer
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetData_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0000 Start";

    void* buffer = nullptr;
    const size_t size = 1024;
    const void* data = nullptr; // Null data

    auto ret = proxy_->GetData(buffer, size, data);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(buffer, nullptr);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0000 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetData_0001
 * @tc.desc: Test GetData with zero size
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetData_0001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0001 Start";

    void* buffer = nullptr;
    const size_t size = 0; // Zero size
    char data[1024] = {0};

    auto ret = proxy_->GetData(buffer, size, data);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(buffer, nullptr);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0001 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetData_0002
 * @tc.desc: Test GetData with size exceeding max capacity
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetData_0002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0002 Start";

    void* buffer = nullptr;
    const size_t size = Constants::MAX_PARCEL_CAPACITY + 1; // Exceeds max
    char data[1024] = {0};

    auto ret = proxy_->GetData(buffer, size, data);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(buffer, nullptr);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0002 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetData_0003
 * @tc.desc: Test GetData with malloc failure
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, BundleManagerAdapterProxy_GetData_0003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0003 Start";

    void* buffer = nullptr;
    const size_t size = 1024;
    char data[size] = {0};

    auto ret = proxy_->GetData(buffer, size, data);
    EXPECT_EQ(ret, true);
    EXPECT_NE(buffer, nullptr);

    if (buffer != nullptr) {
        free(buffer);
    }

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetData_0003 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleInodeCount_0000
 * @tc.desc: Test GetBundleInodeCount function with various scenarios
 * @tc.type: FUNC
 * @tc.require: AR20260114725395
 */
HWTEST_F(BundleManagerAdapterProxyTest, GetBundleInodeCount_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleInodeCount_0000 Start";

    std::string bundleName = "com.example.app";
    int32_t appIndex = 0;
    int32_t userId = 100;
    uint64_t inodeCount = 0;

    // Scenario 1: WriteInterfaceToken fails
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // Scenario 2: WriteString(bundleName) fails
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // Scenario 3: WriteInt32(appIndex) fails
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // Scenario 4: WriteInt32(userId) fails
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
    ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // Scenario 5: SendRequest fails with IPC transaction error
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleInodeCount_0000 End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleInodeCount_0001
 * @tc.desc: Test GetBundleInodeCount function with various scenarios
 * @tc.type: FUNC
 * @tc.require: AR20260114725395
 */
HWTEST_F(BundleManagerAdapterProxyTest, GetBundleInodeCount_0001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleInodeCount_0001 Start";

    std::string bundleName = "com.example.app";
    int32_t appIndex = 0;
    int32_t userId = 100;
    uint64_t inodeCount = 0;

    // Scenario 6: Service returns error code
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_SERVICE_NOT_READY));
    auto ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SERVICE_NOT_READY);

    // Scenario 7: ReadUint64 fails
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*messageParcelMock_, ReadUint64(_)).WillOnce(Return(false));
    ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // Scenario 8: Success case
    uint64_t expectedInodeCount = 12345;
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*messageParcelMock_, ReadUint64(_))
        .WillOnce(DoAll(SetArgReferee<0>(expectedInodeCount), Return(true)));
    ret = proxy_->GetBundleInodeCount(bundleName, appIndex, userId, inodeCount);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(inodeCount, expectedInodeCount);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleInodeCount_0001 End";
}

HWTEST_F(BundleManagerAdapterProxyTest, GetApplicationInfosV9_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetApplicationInfosV9_0000 Start";
    std::vector<ApplicationInfo> appInfos;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->GetApplicationInfosV9(0, 100, appInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    ret = proxy_->GetApplicationInfosV9(0, 100, appInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(false));
    ret = proxy_->GetApplicationInfosV9(0, 100, appInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->GetApplicationInfosV9(0, 100, appInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetApplicationInfosV9_0000 End";
}

HWTEST_F(BundleManagerAdapterProxyTest, CleanBundlePartialCacheAutomatic_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_0000 Start";
    CleanCacheInfo cacheInfo;
    cacheInfo.bundleName = "com.example.app";
    cacheInfo.userId = 100;
    cacheInfo.appIndex = 0;
    cacheInfo.cacheThreshold = 1024;
    uint64_t beforeSize = 0;
    uint64_t afterSize = 0;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    auto ret = proxy_->CleanBundlePartialCacheAutomatic(cacheInfo, beforeSize, afterSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteParcelable(_)).WillOnce(Return(false));
    ret = proxy_->CleanBundlePartialCacheAutomatic(cacheInfo, beforeSize, afterSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteParcelable(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->CleanBundlePartialCacheAutomatic(cacheInfo, beforeSize, afterSize);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_IPC_TRANSACTION);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_0000 End";
}

HWTEST_F(BundleManagerAdapterProxyTest, CleanBundleCacheFilesAutomatic_0002_HasValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0002 Start";
    std::optional<uint64_t> cleanedSize;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt8(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadUint64()).WillOnce(Return(500ULL));
    auto ret = proxy_->CleanBundleCacheFilesAutomatic(100, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(cleanedSize.has_value());

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0002 End";
}

HWTEST_F(BundleManagerAdapterProxyTest, GetBundleNameForUid_HighUid, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleNameForUid_HighUid Start";
    string bundleName = "default-bundleName";
    int uid = Constants::BASE_APP_UID + 1;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(false));
    auto ret = proxy_->GetBundleNameForUid(uid, bundleName);
    EXPECT_EQ(ret, false);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleNameForUid_HighUid End";
}

HWTEST_F(BundleManagerAdapterProxyTest, SendTransactCmd_NullRemote, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmd_NullRemote Start";
    auto nullProxy = make_shared<BundleManagerAdapterProxy>(nullptr);
    MessageParcel data;
    MessageParcel reply;
    bool ret = nullProxy->SendTransactCmd(BundleMgrInterfaceCode::GET_BUNDLE_NAME_FOR_UID, data, reply);
    EXPECT_EQ(ret, false);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmd_NullRemote End";
}

HWTEST_F(BundleManagerAdapterProxyTest, SendTransactCmdWithLog_NullRemote, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmdWithLog_NullRemote Start";
    auto nullProxy = make_shared<BundleManagerAdapterProxy>(nullptr);
    MessageParcel data;
    MessageParcel reply;
    bool ret = nullProxy->SendTransactCmdWithLog(BundleMgrInterfaceCode::GET_NAME_AND_APPINDEX_FOR_UID, data, reply);
    EXPECT_EQ(ret, false);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmdWithLog_NullRemote End";
}

HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_NegativeSize, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_NegativeSize Start";
    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;

    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(-1));
    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_NegativeSize End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0005
 * @tc.desc: Test InnerGetVectorFromParcelIntelligent with negative infoSize after ParseFrom
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_InfoSizeNegative, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_InfoSizeNegative Start";

    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;
    const size_t dataSize = 1024;
    char buffer[dataSize] = {0};

    // rawSize = dataSize -> small data path; ParseFrom succeed; infoSize = -1 -> security verify fail
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(dataSize)).WillOnce(Return(-1));
    EXPECT_CALL(*messageParcelMock_, ReadRawData(_)).WillOnce(Return(buffer));
    EXPECT_CALL(*messageParcelMock_, ParseFrom(_, _)).WillOnce(Return(true));

    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_InfoSizeNegative End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_0006
 * @tc.desc: Test InnerGetVectorFromParcelIntelligent with infoSize exceeding readable data
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, InnerGetVectorFromParcelIntelligent_SecurityVerifyFail, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_SecurityVerifyFail Start";

    MessageParcel reply;
    std::vector<BundleInfo> parcelableInfos;
    const size_t dataSize = 1024;
    char buffer[dataSize] = {0};

    // infoSize = 1 > readable data size 0 of the fresh temp parcel -> CONTAINER_SECURITY_VERIFY fail
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(dataSize)).WillOnce(Return(1));
    EXPECT_CALL(*messageParcelMock_, ReadRawData(_)).WillOnce(Return(buffer));
    EXPECT_CALL(*messageParcelMock_, ParseFrom(_, _)).WillOnce(Return(true));

    auto ret = proxy_->InnerGetVectorFromParcelIntelligent(reply, parcelableInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_InnerGetVectorFromParcelIntelligent_SecurityVerifyFail End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleNameForUid_0001
 * @tc.desc: Test GetBundleNameForUid success path returns bundle name from reply
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, GetBundleNameForUid_WithBundleName, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleNameForUid_WithBundleName Start";

    string bundleName;
    const string expectedName = "com.example.withname";

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadString()).WillOnce(Return(expectedName));
    auto ret = proxy_->GetBundleNameForUid(1000, bundleName);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(bundleName, expectedName);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleNameForUid_WithBundleName End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0003
 * @tc.desc: Test CleanBundleCacheFilesAutomatic with zero cache size and write failures
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, CleanBundleCacheFilesAutomatic_CleanType_InvalidParam, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_InvalidParam Start";

    std::optional<uint64_t> cleanedSize;

    // cache size 0 -> invalid parameter, no parcel write happens
    auto ret = proxy_->CleanBundleCacheFilesAutomatic(
        0, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_INVALID_PARAMETER);

    // write interface token fail
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
    ret = proxy_->CleanBundleCacheFilesAutomatic(100, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // write cache size fail
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(false));
    ret = proxy_->CleanBundleCacheFilesAutomatic(100, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // write clean type fail
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt8(_)).WillOnce(Return(false));
    ret = proxy_->CleanBundleCacheFilesAutomatic(100, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_InvalidParam End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0004
 * @tc.desc: Test CleanBundleCacheFilesAutomatic with send failure and cleaned size handling
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, CleanBundleCacheFilesAutomatic_CleanType_SendFail, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_SendFail Start";

    std::optional<uint64_t> cleanedSize;

    // remote SendRequest fail -> IPC transaction error
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt8(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    auto ret = proxy_->CleanBundleCacheFilesAutomatic(100, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_IPC_TRANSACTION);
    EXPECT_FALSE(cleanedSize.has_value());

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_SendFail End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0005
 * @tc.desc: Test CleanBundleCacheFilesAutomatic without cleaned size in reply
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, CleanBundleCacheFilesAutomatic_CleanType_NoValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_NoValue Start";

    std::optional<uint64_t> cleanedSize;

    // reply has no cleaned size -> cleanedSize keeps empty
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt8(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(false));
    auto ret = proxy_->CleanBundleCacheFilesAutomatic(100, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(cleanedSize.has_value());

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_NoValue End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_0006
 * @tc.desc: Test CleanBundleCacheFilesAutomatic result error while reply still carries value
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, CleanBundleCacheFilesAutomatic_CleanType_ResultError, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_ResultError Start";

    std::optional<uint64_t> cleanedSize;

    // server returns error code, but the reply still contains the cleaned size
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint64(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt8(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_SERVICE_NOT_READY));
    EXPECT_CALL(*messageParcelMock_, ReadBool()).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadUint64()).WillOnce(Return(300ULL));
    auto ret = proxy_->CleanBundleCacheFilesAutomatic(100, OHOS::AppExecFwk::CleanType::CACHE_SPACE, cleanedSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SERVICE_NOT_READY);
    EXPECT_TRUE(cleanedSize.has_value());
    EXPECT_EQ(*cleanedSize, 300ULL);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundleCacheFilesAutomatic_CleanType_ResultError End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetBundleInfosV9_0001
 * @tc.desc: Test GetBundleInfosV9 reply error code and empty vector success
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, GetBundleInfosV9_ReplyErrorAndEmpty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleInfosV9_ReplyErrorAndEmpty Start";

    std::vector<BundleInfo> bundleInfos;

    // server reply carries a non-ERR_OK result -> propagate the error
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_SERVICE_NOT_READY));
    auto ret = proxy_->GetBundleInfosV9(0, bundleInfos, 100);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SERVICE_NOT_READY);

    // success with zero raw data size -> empty vector and ERR_OK
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK)).WillOnce(Return(0));
    ret = proxy_->GetBundleInfosV9(0, bundleInfos, 100);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(bundleInfos.empty());

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetBundleInfosV9_ReplyErrorAndEmpty End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetApplicationInfosV9_0001
 * @tc.desc: Test GetApplicationInfosV9 reply error code and empty vector success
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, GetApplicationInfosV9_ReplyErrorAndEmpty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetApplicationInfosV9_ReplyErrorAndEmpty Start";

    std::vector<ApplicationInfo> appInfos;

    // server reply carries a non-ERR_OK result -> propagate the error
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_SERVICE_NOT_READY));
    auto ret = proxy_->GetApplicationInfosV9(0, 100, appInfos);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SERVICE_NOT_READY);

    // success with zero raw data size -> empty vector and ERR_OK
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK)).WillOnce(Return(0));
    ret = proxy_->GetApplicationInfosV9(0, 100, appInfos);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(appInfos.empty());

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetApplicationInfosV9_ReplyErrorAndEmpty End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_0001
 * @tc.desc: Test CleanBundlePartialCacheAutomatic server error code and read failures
 * @tc.type: FUNC
 * @tc.require: AR20260114725395
 */
HWTEST_F(BundleManagerAdapterProxyTest, CleanBundlePartialCacheAutomatic_ReplyErrorAndReadFail, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_ReplyErrorAndReadFail Start";

    CleanCacheInfo cacheInfo;
    cacheInfo.bundleName = "com.example.app";
    cacheInfo.userId = 100;
    cacheInfo.appIndex = 0;
    cacheInfo.cacheThreshold = 1024;
    uint64_t beforeSize = 0;
    uint64_t afterSize = 0;

    // server reply carries a non-ERR_OK result -> propagate the error
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteParcelable(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_APPEXECFWK_SERVICE_NOT_READY));
    auto ret = proxy_->CleanBundlePartialCacheAutomatic(cacheInfo, beforeSize, afterSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SERVICE_NOT_READY);

    // read beforeCleanedSize fail
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteParcelable(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*messageParcelMock_, ReadUint64(_)).WillOnce(Return(false));
    ret = proxy_->CleanBundlePartialCacheAutomatic(cacheInfo, beforeSize, afterSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    // read afterCleanedSize fail
    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteParcelable(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*messageParcelMock_, ReadUint64(_)).WillOnce(Return(true)).WillOnce(Return(false));
    ret = proxy_->CleanBundlePartialCacheAutomatic(cacheInfo, beforeSize, afterSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_ReplyErrorAndReadFail End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_0002
 * @tc.desc: Test CleanBundlePartialCacheAutomatic success with both sizes read from reply
 * @tc.type: FUNC
 * @tc.require: AR20260114725395
 */
HWTEST_F(BundleManagerAdapterProxyTest, CleanBundlePartialCacheAutomatic_Success, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_Success Start";

    CleanCacheInfo cacheInfo;
    cacheInfo.bundleName = "com.example.app";
    cacheInfo.userId = 100;
    cacheInfo.appIndex = 0;
    cacheInfo.cacheThreshold = 1024;
    uint64_t beforeSize = 0;
    uint64_t afterSize = 0;
    const uint64_t expectedBefore = 4096;
    const uint64_t expectedAfter = 2048;

    EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteParcelable(_)).WillOnce(Return(true));
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    EXPECT_CALL(*messageParcelMock_, ReadInt32()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*messageParcelMock_, ReadUint64(_))
        .WillOnce(DoAll(SetArgReferee<0>(expectedBefore), Return(true)))
        .WillOnce(DoAll(SetArgReferee<0>(expectedAfter), Return(true)));
    auto ret = proxy_->CleanBundlePartialCacheAutomatic(cacheInfo, beforeSize, afterSize);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(beforeSize, expectedBefore);
    EXPECT_EQ(afterSize, expectedAfter);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_CleanBundlePartialCacheAutomatic_Success End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_SendTransactCmd_0000
 * @tc.desc: Test SendTransactCmd success with mocked remote SendRequest
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, SendTransactCmd_Success, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmd_Success Start";

    MessageParcel data;
    MessageParcel reply;

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    bool ret = proxy_->SendTransactCmd(BundleMgrInterfaceCode::GET_BUNDLE_NAME_FOR_UID, data, reply);
    EXPECT_EQ(ret, true);

    // remote SendRequest returns non-zero -> false
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->SendTransactCmd(BundleMgrInterfaceCode::GET_BUNDLE_NAME_FOR_UID, data, reply);
    EXPECT_EQ(ret, false);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmd_Success End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_SendTransactCmdWithLog_0000
 * @tc.desc: Test SendTransactCmdWithLog success with mocked remote SendRequest
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, SendTransactCmdWithLog_Success, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmdWithLog_Success Start";

    MessageParcel data;
    MessageParcel reply;

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(NO_ERROR));
    bool ret = proxy_->SendTransactCmdWithLog(BundleMgrInterfaceCode::GET_NAME_AND_APPINDEX_FOR_UID, data, reply);
    EXPECT_EQ(ret, true);

    // remote SendRequest returns non-zero -> false
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(ERR_APPEXECFWK_PARCEL_ERROR));
    ret = proxy_->SendTransactCmdWithLog(BundleMgrInterfaceCode::GET_NAME_AND_APPINDEX_FOR_UID, data, reply);
    EXPECT_EQ(ret, false);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_SendTransactCmdWithLog_Success End";
}

/**
 * @tc.name: BundleManagerAdapterProxy_GetParcelInfoFromAshMem_0000
 * @tc.desc: Test GetParcelInfoFromAshMem with an empty reply parcel
 * @tc.type: FUNC
 * @tc.require: I7TDJK
 */
HWTEST_F(BundleManagerAdapterProxyTest, GetParcelInfoFromAshMem_EmptyReply, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetParcelInfoFromAshMem_EmptyReply Start";

    MessageParcel reply;
    void *data = nullptr;
    int32_t ashMemSize = 0;

    // an empty parcel holds no ashmem -> nullptr ashmem branch
    auto ret = proxy_->GetParcelInfoFromAshMem(reply, data, ashMemSize);
    EXPECT_EQ(ret, ERR_APPEXECFWK_PARCEL_ERROR);
    EXPECT_EQ(ashMemSize, 0);

    GTEST_LOG_(INFO) << "BundleManagerAdapterProxy_GetParcelInfoFromAshMem_EmptyReply End";
}
} // namespace OHOS::StorageSpaceManager::Test
