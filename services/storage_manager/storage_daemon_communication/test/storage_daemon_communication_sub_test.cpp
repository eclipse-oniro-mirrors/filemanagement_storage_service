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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "storage_daemon_communication/storage_daemon_communication.h"

#include "if_system_ability_manager.h"
#include "storage_service_errno.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "storage_daemon_stub_mock.h"

namespace OHOS {
class ISystemAbilityBase {
public:
    virtual ~ISystemAbilityBase() = default;
    virtual sptr<ISystemAbilityManager> GetSystemAbilityManager() = 0;
public:
    static inline std::shared_ptr<ISystemAbilityBase> sab = nullptr;
};

class SystemAbilityMock : public ISystemAbilityBase {
public:
    MOCK_METHOD((sptr<ISystemAbilityManager>), GetSystemAbilityManager, ());
};

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    return ISystemAbilityBase::sab->GetSystemAbilityManager();
}

class SystemAbilityManagerMock : public ISystemAbilityManager {
public:
    MOCK_METHOD((sptr<IRemoteObject>), AsObject, ());
    MOCK_METHOD((std::vector<std::u16string>), ListSystemAbilities, (unsigned int));
    MOCK_METHOD((sptr<IRemoteObject>), GetSystemAbility, (int32_t));
    MOCK_METHOD((sptr<IRemoteObject>), CheckSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, RemoveSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, SubscribeSystemAbility, (int32_t, (const sptr<ISystemAbilityStatusChange>&)));
    MOCK_METHOD(int32_t, UnSubscribeSystemAbility, (int32_t, (const sptr<ISystemAbilityStatusChange>&)));
    MOCK_METHOD((sptr<IRemoteObject>), GetSystemAbility, (int32_t, const std::string&));
    MOCK_METHOD((sptr<IRemoteObject>), CheckSystemAbility, (int32_t, const std::string&));
    MOCK_METHOD(int32_t, AddOnDemandSystemAbilityInfo, (int32_t, const std::u16string&));
    MOCK_METHOD((sptr<IRemoteObject>), CheckSystemAbility, (int32_t, bool&));
    MOCK_METHOD(int32_t, AddSystemAbility, (int32_t, (const sptr<IRemoteObject>&), const SAExtraProp&));
    MOCK_METHOD(int32_t, AddSystemProcess, (const std::u16string&, (const sptr<IRemoteObject>&)));
    MOCK_METHOD((sptr<IRemoteObject>), LoadSystemAbility, (int32_t, int32_t));
    MOCK_METHOD(int32_t, LoadSystemAbility, (int32_t, (const sptr<ISystemAbilityLoadCallback>&)));
    MOCK_METHOD(int32_t, LoadSystemAbility, (int32_t, const std::string&, (const sptr<ISystemAbilityLoadCallback>&)));
    MOCK_METHOD(int32_t, UnloadSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, CancelUnloadSystemAbility, (int32_t));
    MOCK_METHOD(int32_t, UnloadAllIdleSystemAbility, ());
    MOCK_METHOD(int32_t, GetSystemProcessInfo, (int32_t, SystemProcessInfo&));
    MOCK_METHOD(int32_t, GetRunningSystemProcess, (std::list<SystemProcessInfo>&));
    MOCK_METHOD(int32_t, SubscribeSystemProcess, (const sptr<ISystemProcessStatusChange>&));
    MOCK_METHOD(int32_t, SendStrategy, (int32_t, (std::vector<int32_t>&), int32_t, std::string&));
    MOCK_METHOD(int32_t, UnSubscribeSystemProcess, (const sptr<ISystemProcessStatusChange>&));
    MOCK_METHOD(int32_t, GetOnDemandReasonExtraData, (int64_t, MessageParcel&));
    MOCK_METHOD(int32_t, GetOnDemandPolicy, (int32_t, OnDemandPolicyType, (std::vector<SystemAbilityOnDemandEvent>&)));
    MOCK_METHOD(int32_t, UpdateOnDemandPolicy, (int32_t, OnDemandPolicyType,
        (const std::vector<SystemAbilityOnDemandEvent>&)));
    MOCK_METHOD(int32_t, GetOnDemandSystemAbilityIds, (std::vector<int32_t>&));
    MOCK_METHOD(int32_t, GetExtensionSaIds, (const std::string&, std::vector<int32_t>&));
    MOCK_METHOD(int32_t, GetExtensionRunningSaList, (const std::string&, (std::vector<sptr<IRemoteObject>>&)));
    MOCK_METHOD(int32_t, GetRunningSaExtensionInfoList, (const std::string&, (std::vector<SaExtensionInfo>&)));
    MOCK_METHOD(int32_t, GetCommonEventExtraDataIdlist, (int32_t, (std::vector<int64_t>&), const std::string&));
    MOCK_METHOD((sptr<IRemoteObject>), GetLocalAbilityManagerProxy, (int32_t));
};
}

namespace OHOS {
using namespace std;
using namespace testing;
using namespace testing::ext;
using namespace StorageManager;
using namespace StorageDaemon;

class StorageDaemonCommunicationTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};
public:
    static inline shared_ptr<StorageDaemonCommunication> sdCommunication = nullptr;
    static inline shared_ptr<SystemAbilityMock> sa = nullptr;
    static inline sptr<SystemAbilityManagerMock> sam = nullptr;
    static inline sptr<StorageDaemonStubMock> sd = nullptr;
};

void StorageDaemonCommunicationTest::SetUpTestCase()
{
    sdCommunication = make_shared<StorageDaemonCommunication>();
    sa = make_shared<SystemAbilityMock>();
    SystemAbilityMock::sab = sa;
    sam = sptr(new SystemAbilityManagerMock());
    sd = sptr(new StorageDaemonStubMock());
}

void StorageDaemonCommunicationTest::TearDownTestCase()
{
    sam = nullptr;
    sd = nullptr;
    SystemAbilityMock::sab = nullptr;
    sa = nullptr;
    sdCommunication->storageDaemon_ = nullptr;
    sdCommunication = nullptr;
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_Connect_0000
* @tc.name: Daemon_communication_Connect_0000
* @tc.desc: Test function of Connect interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_Connect_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_Connect_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->Connect(), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->Connect(), E_REMOTE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_EQ(sdCommunication->Connect(), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_Connect_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_PrepareAddUser_0000
* @tc.name: Daemon_communication_PrepareAddUser_0000
* @tc.desc: Test function of PrepareAddUser interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_PrepareAddUser_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_PrepareAddUser_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->PrepareAddUser(0, 0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->PrepareAddUser(0, 0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, PrepareUserDirs(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->PrepareAddUser(0, 0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_PrepareAddUser_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_RemoveUser_0000
* @tc.name: Daemon_communication_RemoveUser_0000
* @tc.desc: Test function of RemoveUser interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_RemoveUser_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_RemoveUser_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->RemoveUser(0, 0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->RemoveUser(0, 0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, DestroyUserDirs(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->RemoveUser(0, 0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_RemoveUser_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_PrepareStartUser_0000
* @tc.name: Daemon_communication_PrepareStartUser_0000
* @tc.desc: Test function of PrepareStartUser interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_PrepareStartUser_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_PrepareStartUser_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->PrepareStartUser(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->PrepareStartUser(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, StartUser(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->PrepareStartUser(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_PrepareStartUser_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_StopUser_0000
* @tc.name: Daemon_communication_StopUser_0000
* @tc.desc: Test function of StopUser interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_StopUser_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_StopUser_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->StopUser(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->StopUser(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, StopUser(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->StopUser(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_StopUser_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_CompleteAddUser_0000
* @tc.name: Daemon_communication_CompleteAddUser_0000
* @tc.desc: Test function of CompleteAddUser interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_CompleteAddUser_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_CompleteAddUser_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->CompleteAddUser(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->CompleteAddUser(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, CompleteAddUser(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->CompleteAddUser(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_CompleteAddUser_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_Mount_0000
* @tc.name: Daemon_communication_Mount_0000
* @tc.desc: Test function of Mount interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_Mount_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_Mount_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->Mount("", 0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->Mount("", 0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, Mount(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->Mount("", 0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_Mount_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_Unmount_0000
* @tc.name: Daemon_communication_Unmount_0000
* @tc.desc: Test function of Unmount interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_Unmount_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_Unmount_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->Unmount(""), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->Unmount(""), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UMount(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->Unmount(""), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_Unmount_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_Check_0000
* @tc.name: Daemon_communication_Check_0000
* @tc.desc: Test function of Check interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_Check_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_Check_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->Check(""), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->Check(""), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, Check(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->Check(""), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_Check_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_Partition_0000
* @tc.name: Daemon_communication_Partition_0000
* @tc.desc: Test function of Partition interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_Partition_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_Partition_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->Partition("", 0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->Partition("", 0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, Partition(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->Partition("", 0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_Partition_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_Format_0000
* @tc.name: Daemon_communication_Format_0000
* @tc.desc: Test function of Format interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_Format_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_Format_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->Format("", ""), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->Format("", ""), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, Format(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->Format("", ""), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_Format_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_SetVolumeDescription_0000
* @tc.name: Daemon_communication_SetVolumeDescription_0000
* @tc.desc: Test function of SetVolumeDescription interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_SetVolumeDescription_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_SetVolumeDescription_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->SetVolumeDescription("", ""), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->SetVolumeDescription("", ""), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, SetVolumeDescription(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->SetVolumeDescription("", ""), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_SetVolumeDescription_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_QueryUsbIsInUse_0000
* @tc.name: Daemon_communication_QueryUsbIsInUse_0000
* @tc.desc: Test function of QueryUsbIsInUse interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_QueryUsbIsInUse_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_QueryUsbIsInUse_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    string diskPath;
    bool isInUse = false;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->QueryUsbIsInUse(diskPath, isInUse), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->QueryUsbIsInUse(diskPath, isInUse), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, QueryUsbIsInUse(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->QueryUsbIsInUse(diskPath, isInUse), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_QueryUsbIsInUse_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_GenerateUserKeys_0000
* @tc.name: Daemon_communication_GenerateUserKeys_0000
* @tc.desc: Test function of GenerateUserKeys interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_GenerateUserKeys_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_GenerateUserKeys_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->GenerateUserKeys(0, 0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->GenerateUserKeys(0, 0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, GenerateUserKeys(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->GenerateUserKeys(0, 0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_GenerateUserKeys_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_DeleteUserKeys_0000
* @tc.name: Daemon_communication_DeleteUserKeys_0000
* @tc.desc: Test function of DeleteUserKeys interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_DeleteUserKeys_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_DeleteUserKeys_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->DeleteUserKeys(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->DeleteUserKeys(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, DeleteUserKeys(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->DeleteUserKeys(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_DeleteUserKeys_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UpdateUserAuth_0000
* @tc.name: Daemon_communication_UpdateUserAuth_0000
* @tc.desc: Test function of UpdateUserAuth interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UpdateUserAuth_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UpdateUserAuth_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<uint8_t> token;
    vector<uint8_t> oldSecret;
    vector<uint8_t> newSecret;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UpdateUserAuth(0, 0, token, oldSecret, newSecret), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UpdateUserAuth(0, 0, token, oldSecret, newSecret), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UpdateUserAuth(_, _, _, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UpdateUserAuth(0, 0, token, oldSecret, newSecret), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UpdateUserAuth_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UpdateUseAuthWithRecoveryKey_0000
* @tc.name: Daemon_communication_UpdateUseAuthWithRecoveryKey_0000
* @tc.desc: Test function of UpdateUseAuthWithRecoveryKey interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UpdateUseAuthWithRecoveryKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UpdateUseAuthWithRecoveryKey_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<uint8_t> authToken;
    vector<uint8_t> newSecret;
    vector<std::vector<uint8_t>> plainText;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UpdateUseAuthWithRecoveryKey(authToken, newSecret, 0, 0, plainText), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(
        sdCommunication->UpdateUseAuthWithRecoveryKey(authToken, newSecret, 0, 0, plainText), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UpdateUseAuthWithRecoveryKey(_, _, _, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UpdateUseAuthWithRecoveryKey(authToken, newSecret, 0, 0, plainText), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UpdateUseAuthWithRecoveryKey_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_ActiveUserKey_0000
* @tc.name: Daemon_communication_ActiveUserKey_0000
* @tc.desc: Test function of ActiveUserKey interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_ActiveUserKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_ActiveUserKey_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<uint8_t> token;
    vector<uint8_t> secret;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->ActiveUserKey(0, token, secret), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->ActiveUserKey(0, token, secret), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, ActiveUserKey(_, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->ActiveUserKey(0, token, secret), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_ActiveUserKey_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_InactiveUserKey_0000
* @tc.name: Daemon_communication_InactiveUserKey_0000
* @tc.desc: Test function of InactiveUserKey interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_InactiveUserKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_InactiveUserKey_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->InactiveUserKey(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->InactiveUserKey(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, InactiveUserKey(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->InactiveUserKey(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_InactiveUserKey_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_LockUserScreen_0000
* @tc.name: Daemon_communication_LockUserScreen_0000
* @tc.desc: Test function of LockUserScreen interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_LockUserScreen_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_LockUserScreen_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->LockUserScreen(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->LockUserScreen(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, LockUserScreen(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->LockUserScreen(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_LockUserScreen_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_GetFileEncryptStatus_0000
* @tc.name: Daemon_communication_GetFileEncryptStatus_0000
* @tc.desc: Test function of GetFileEncryptStatus interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_GetFileEncryptStatus_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_GetFileEncryptStatus_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    bool isEncrypted = false;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->GetFileEncryptStatus(0, isEncrypted, false), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->GetFileEncryptStatus(0, isEncrypted, false), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, GetFileEncryptStatus(_, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->GetFileEncryptStatus(0, isEncrypted, false), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_GetFileEncryptStatus_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_GetUserNeedActiveStatus_0000
* @tc.name: Daemon_communication_GetUserNeedActiveStatus_0000
* @tc.desc: Test function of GetUserNeedActiveStatus interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_GetUserNeedActiveStatus_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_GetUserNeedActiveStatus_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    bool needActive = false;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->GetUserNeedActiveStatus(0, needActive), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->GetUserNeedActiveStatus(0, needActive), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, GetUserNeedActiveStatus(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->GetUserNeedActiveStatus(0, needActive), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_GetUserNeedActiveStatus_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UnlockUserScreen_0000
* @tc.name: Daemon_communication_UnlockUserScreen_0000
* @tc.desc: Test function of UnlockUserScreen interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UnlockUserScreen_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UnlockUserScreen_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<uint8_t> token;
    vector<uint8_t> secret;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UnlockUserScreen(0, token, secret), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UnlockUserScreen(0, token, secret), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UnlockUserScreen(_, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UnlockUserScreen(0, token, secret), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UnlockUserScreen_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_GetLockScreenStatus_0000
* @tc.name: Daemon_communication_GetLockScreenStatus_0000
* @tc.desc: Test function of GetLockScreenStatus interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_GetLockScreenStatus_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_GetLockScreenStatus_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    bool lockScreenStatus = false;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->GetLockScreenStatus(0, lockScreenStatus), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->GetLockScreenStatus(0, lockScreenStatus), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, GetLockScreenStatus(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->GetLockScreenStatus(0, lockScreenStatus), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_GetLockScreenStatus_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UpdateKeyContext_0000
* @tc.name: Daemon_communication_UpdateKeyContext_0000
* @tc.desc: Test function of UpdateKeyContext interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UpdateKeyContext_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UpdateKeyContext_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UpdateKeyContext(0, false), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UpdateKeyContext(0, false), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UpdateKeyContext(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UpdateKeyContext(0, false), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UpdateKeyContext_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_ResetSdProxy_0000
* @tc.name: Daemon_communication_ResetSdProxy_0000
* @tc.desc: Test function of ResetSdProxy interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_ResetSdProxy_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_ResetSdProxy_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_EQ(sdCommunication->ResetSdProxy(), E_OK);

    sdCommunication->storageDaemon_ = sd;
    EXPECT_CALL(*sd, RemoveDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_EQ(sdCommunication->ResetSdProxy(), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_ResetSdProxy_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_CreateShareFile_0000
* @tc.name: Daemon_communication_CreateShareFile_0000
* @tc.desc: Test function of CreateShareFile interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_CreateShareFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_CreateShareFile_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<string> uriList;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->CreateShareFile(uriList, 0, 0)[0], E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->CreateShareFile(uriList, 0, 0)[0], 0);

    vector<int32_t> funcResult;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, CreateShareFile(_, _, _, _)).WillOnce(DoAll(SetArgReferee<3>(funcResult), Return(E_OK)));
    EXPECT_EQ(sdCommunication->CreateShareFile(uriList, 0, 0).size(), 0);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_CreateShareFile_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_DeleteShareFile_0000
* @tc.name: Daemon_communication_DeleteShareFile_0000
* @tc.desc: Test function of DeleteShareFile interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_DeleteShareFile_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_DeleteShareFile_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<string> uriList;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->DeleteShareFile(0, uriList), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->DeleteShareFile(0, uriList), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, DeleteShareFile(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->DeleteShareFile(0, uriList), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_DeleteShareFile_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_SetBundleQuota_0000
* @tc.name: Daemon_communication_SetBundleQuota_0000
* @tc.desc: Test function of SetBundleQuota interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_SetBundleQuota_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_SetBundleQuota_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    string bundleName;
    string bundleDataDirPath;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->SetBundleQuota(bundleName, 0, bundleDataDirPath, 0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->SetBundleQuota(bundleName, 0, bundleDataDirPath, 0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, SetBundleQuota(_, _, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->SetBundleQuota(bundleName, 0, bundleDataDirPath, 0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_SetBundleQuota_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_GetOccupiedSpace_0000
* @tc.name: Daemon_communication_GetOccupiedSpace_0000
* @tc.desc: Test function of GetOccupiedSpace interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_GetOccupiedSpace_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_GetOccupiedSpace_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    int64_t size = 0;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->GetOccupiedSpace(0, 0, size), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->GetOccupiedSpace(0, 0, size), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, GetOccupiedSpace(_, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->GetOccupiedSpace(0, 0, size), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_GetOccupiedSpace_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_MountCryptoPathAgain_0000
* @tc.name: Daemon_communication_MountCryptoPathAgain_0000
* @tc.desc: Test function of MountCryptoPathAgain interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_MountCryptoPathAgain_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_MountCryptoPathAgain_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->MountCryptoPathAgain(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->MountCryptoPathAgain(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, MountCryptoPathAgain(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->MountCryptoPathAgain(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_MountCryptoPathAgain_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_GenerateAppkey_0000
* @tc.name: Daemon_communication_GenerateAppkey_0000
* @tc.desc: Test function of GenerateAppkey interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_GenerateAppkey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_GenerateAppkey_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    string keyId;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->GenerateAppkey(0, 0, keyId), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->GenerateAppkey(0, 0, keyId), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, GenerateAppkey(_, _, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->GenerateAppkey(0, 0, keyId), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_GenerateAppkey_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_DeleteAppkey_0000
* @tc.name: Daemon_communication_DeleteAppkey_0000
* @tc.desc: Test function of DeleteAppkey interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_DeleteAppkey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_DeleteAppkey_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->DeleteAppkey(0, ""), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->DeleteAppkey(0, ""), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, DeleteAppkey(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->DeleteAppkey(0, ""), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_DeleteAppkey_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_CreateRecoverKey_0000
* @tc.name: Daemon_communication_CreateRecoverKey_0000
* @tc.desc: Test function of CreateRecoverKey interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_CreateRecoverKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_CreateRecoverKey_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<uint8_t> token;
    vector<uint8_t> secret;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->CreateRecoverKey(0, 0, token, secret), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->CreateRecoverKey(0, 0, token, secret), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, CreateRecoverKey(_, _, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->CreateRecoverKey(0, 0, token, secret), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_CreateRecoverKey_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_SetRecoverKey_0000
* @tc.name: Daemon_communication_SetRecoverKey_0000
* @tc.desc: Test function of SetRecoverKey interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_SetRecoverKey_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_SetRecoverKey_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<uint8_t> key;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->SetRecoverKey(key), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->SetRecoverKey(key), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, SetRecoverKey(_)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->SetRecoverKey(key), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_SetRecoverKey_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_GetBundleStatsForIncrease_0000
* @tc.name: Daemon_communication_GetBundleStatsForIncrease_0000
* @tc.desc: Test function of GetBundleStatsForIncrease interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_GetBundleStatsForIncrease_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_GetBundleStatsForIncrease_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    vector<string> bundleNames;
    vector<int64_t> incTimes;
    vector<int64_t> pkgFileSizes;
    vector<int64_t> incSizes;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    auto ret = sdCommunication->GetBundleStatsForIncrease(0, bundleNames, incTimes, pkgFileSizes, incSizes);
    EXPECT_EQ(ret, E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    ret = sdCommunication->GetBundleStatsForIncrease(0, bundleNames, incTimes, pkgFileSizes, incSizes);
    EXPECT_EQ(ret, E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, GetBundleStatsForIncrease(_, _, _, _, _)).WillOnce(Return(E_OK));
    ret = sdCommunication->GetBundleStatsForIncrease(0, bundleNames, incTimes, pkgFileSizes, incSizes);
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_GetBundleStatsForIncrease_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UpdateMemoryPara_0000
* @tc.name: Daemon_communication_UpdateMemoryPara_0000
* @tc.desc: Test function of UpdateMemoryPara interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UpdateMemoryPara_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UpdateMemoryPara_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    int32_t oldSize;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UpdateMemoryPara(0, oldSize), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UpdateMemoryPara(0, oldSize), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UpdateMemoryPara(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UpdateMemoryPara(0, oldSize), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UpdateMemoryPara_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_MountDfsDocs_0000
* @tc.name: Daemon_communication_MountDfsDocs_0000
* @tc.desc: Test function of MountDfsDocs interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_MountDfsDocs_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_MountDfsDocs_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    string relativePath;
    string networkId;
    string deviceId;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->MountDfsDocs(0, relativePath, networkId, deviceId), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->MountDfsDocs(0, relativePath, networkId, deviceId), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, MountDfsDocs(_, _, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->MountDfsDocs(0, relativePath, networkId, deviceId), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_MountDfsDocs_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UMountDfsDocs_0000
* @tc.name: Daemon_communication_UMountDfsDocs_0000
* @tc.desc: Test function of UMountDfsDocs interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UMountDfsDocs_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UMountDfsDocs_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    string relativePath;
    string networkId;
    string deviceId;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UMountDfsDocs(0, relativePath, networkId, deviceId), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UMountDfsDocs(0, relativePath, networkId, deviceId), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UMountDfsDocs(_, _, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UMountDfsDocs(0, relativePath, networkId, deviceId), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UMountDfsDocs_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_MountMediaFuse_0000
* @tc.name: Daemon_communication_MountMediaFuse_0000
* @tc.desc: Test function of MountMediaFuse interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_MountMediaFuse_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_MountMediaFuse_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    int32_t devFd = 0;
#ifdef STORAGE_SERVICE_MEDIA_FUSE
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->MountMediaFuse(0, devFd), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->MountMediaFuse(0, devFd), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, MountMediaFuse(_, _)).WillOnce(Return(E_OK));
#endif
    EXPECT_EQ(sdCommunication->MountMediaFuse(0, devFd), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_MountMediaFuse_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UMountMediaFuse_0000
* @tc.name: Daemon_communication_UMountMediaFuse_0000
* @tc.desc: Test function of UMountMediaFuse interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UMountMediaFuse_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UMountMediaFuse_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

#ifdef STORAGE_SERVICE_MEDIA_FUSE
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UMountMediaFuse(0), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UMountMediaFuse(0), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UMountMediaFuse(_)).WillOnce(Return(E_OK));
#endif
    EXPECT_EQ(sdCommunication->UMountMediaFuse(0), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UMountMediaFuse_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_MountFileMgrFuse_0000
* @tc.name: Daemon_communication_MountFileMgrFuse_0000
* @tc.desc: Test function of MountFileMgrFuse interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_MountFileMgrFuse_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_MountFileMgrFuse_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    string path;
    int32_t fuseFd = 0;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->MountFileMgrFuse(0, path, fuseFd), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->MountFileMgrFuse(0, path, fuseFd), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, MountFileMgrFuse(_, _, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->MountFileMgrFuse(0, path, fuseFd), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_MountFileMgrFuse_0000";
}

/**
* @tc.number: SUB_STORAGE_Daemon_communication_UMountFileMgrFuse_0000
* @tc.name: Daemon_communication_UMountFileMgrFuse_0000
* @tc.desc: Test function of UMountFileMgrFuse interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR000GK4HB
*/
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UMountFileMgrFuse_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UMountFileMgrFuse_0000";
    ASSERT_TRUE(sdCommunication != nullptr);

    string path;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UMountFileMgrFuse(0, path), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UMountFileMgrFuse(0, path), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UMountFileMgrFuse(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UMountFileMgrFuse(0, path), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UMountFileMgrFuse_0000";
}

/**
 * @tc.number: SUB_STORAGE_Daemon_communication_MountDisShareFile_0000
 * @tc.name: Daemon_communication_MountDisShareFile_0000
 * @tc.desc: Test function of MountDisShareFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0FG3
 */
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_MountDisShareFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_MountDisShareFile_0001 SUCCESS";
    ASSERT_TRUE(sdCommunication != nullptr);

    std::map<std::string, std::string> shareFiles;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->MountDisShareFile(0, shareFiles), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->MountDisShareFile(0, shareFiles), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, MountDisShareFile(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->MountDisShareFile(0, shareFiles), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_MountDisShareFile_0001 SUCCESS";
}

/**
 * @tc.number: SUB_STORAGE_Daemon_communication_UMountDisShareFile_0000
 * @tc.name: Daemon_communication_UMountDisShareFile_0000
 * @tc.desc: Test function of UMountDisShareFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000H0FG3
 */
HWTEST_F(StorageDaemonCommunicationTest, Daemon_communication_UMountDisShareFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-begin Daemon_communication_UMountDisShareFile_0001 SUCCESS";
    ASSERT_TRUE(sdCommunication != nullptr);

    std::string networkId;
    sdCommunication->storageDaemon_ = nullptr;
    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(nullptr));
    EXPECT_EQ(sdCommunication->UMountDisShareFile(0, networkId), E_SA_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(DoAll(Invoke([sdCommunication {sdCommunication}] () {
        sdCommunication->storageDaemon_ = nullptr;
    }), Return(true)));
    EXPECT_EQ(sdCommunication->UMountDisShareFile(0, networkId), E_SERVICE_IS_NULLPTR);

    EXPECT_CALL(*sa, GetSystemAbilityManager()).WillOnce(Return(sam));
    EXPECT_CALL(*sam, GetSystemAbility(_)).WillOnce(Return(sd));
    EXPECT_CALL(*sd, AddDeathRecipient(_)).WillOnce(Return(true));
    EXPECT_CALL(*sd, UMountDisShareFile(_, _)).WillOnce(Return(E_OK));
    EXPECT_EQ(sdCommunication->UMountDisShareFile(0, networkId), E_OK);
    GTEST_LOG_(INFO) << "StorageDaemonCommunicationTest-end Daemon_communication_UMountDisShareFile_0001 SUCCESS";
}
}