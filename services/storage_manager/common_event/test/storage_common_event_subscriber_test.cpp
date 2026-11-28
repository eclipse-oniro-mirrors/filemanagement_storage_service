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

#include <cstdio>
#include <gtest/gtest.h>

#include "common_event/storage_common_event_subscriber.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"

namespace OHOS {
namespace StorageManager {
using namespace std;
using namespace testing::ext;

class StorageCommonEventSubscriberTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp();
    void TearDown();
    static inline shared_ptr<StorageCommonEventSubscriber> subscriberPtr_ = nullptr;
};

void StorageCommonEventSubscriberTest::SetUp()
{
    subscriberPtr_ = make_shared<StorageCommonEventSubscriber>();
}

void StorageCommonEventSubscriberTest::TearDown()
{
    subscriberPtr_ = nullptr;
}

/**
* @tc.number: Storage_subscriber_SubscribeCommonEvent_test_0000
* @tc.name: Storage_subscriber_SubscribeCommonEvent_test_0000
* @tc.desc: Test function of SubscribeCommonEvent
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_SubscribeCommonEvent_test_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_SubscribeCommonEvent_test_0000 begin";
    StorageCommonEventSubscriber::SubscribeCommonEvent();
    StorageCommonEventSubscriber::SubscribeCommonEvent();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_SubscribeCommonEvent_test_0000 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0000
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0000
* @tc.desc: Test function of OnReceiveEvent
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0000 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0000 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0001
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0001
* @tc.desc: Test function of OnReceiveEvent
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0001 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    int32_t userId = 100;
    std::string bundleName = "test";
    want.SetParam("userId", userId);
    want.SetParam("bundleName", bundleName);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0001 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0002
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0002
* @tc.desc: Test function of OnReceiveEvent
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0002 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0002 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0003
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0003
* @tc.desc: Test function of OnReceiveEvent
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0003 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 5);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);

    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 5);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0003 end";
}

/**
* @tc.number: Storage_subscriber_UpdateDeviceState_test_0001
* @tc.name: Storage_subscriber_UpdateDeviceState_test_0001
* @tc.desc: Test function of UpdateDeviceState
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_UpdateDeviceState_test_0001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0001 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);

    subscriberPtr_->batteryCapacity_.store(5);
    uint8_t oldState = subscriberPtr_->deviceState_.load();
    subscriberPtr_->UpdateDeviceState(STATE_SCREEN_OFF, true);
    uint8_t newState = subscriberPtr_->deviceState_.load();
    EXPECT_EQ(newState, oldState | STATE_SCREEN_OFF);

    oldState = subscriberPtr_->deviceState_.load();
    subscriberPtr_->UpdateDeviceState(STATE_SCREEN_OFF, false);
    newState = subscriberPtr_->deviceState_.load();
    EXPECT_EQ(newState, oldState & ~STATE_SCREEN_OFF);

    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0001 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0001
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0001
* @tc.desc: Test function of CheckAndTriggerStatistic
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0001 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);

    subscriberPtr_->deviceState_ = STATE_SCREEN_OFF;
    subscriberPtr_->batteryCapacity_ = 15;
    uint8_t stateBefore = subscriberPtr_->deviceState_.load();
    subscriberPtr_->CheckAndTriggerStatistic();
    uint8_t stateAfter = subscriberPtr_->deviceState_.load();
    EXPECT_EQ(stateBefore, stateAfter);

    subscriberPtr_->deviceState_ = STATE_CHARGING_SCREEN_OFF;
    subscriberPtr_->batteryCapacity_ = 5;
    stateBefore = subscriberPtr_->deviceState_.load();
    subscriberPtr_->CheckAndTriggerStatistic();
    stateAfter = subscriberPtr_->deviceState_.load();
    EXPECT_EQ(stateBefore, stateAfter);

    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0001 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0004
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0004
* @tc.desc: Test function of OnReceiveEvent with COMMON_EVENT_USER_UNLOCKED
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR20260114725643
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0004 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_UNLOCKED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0004 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0005
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0005
* @tc.desc: Test function of OnReceiveEvent with multiple events including USER_UNLOCKED
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: AR20260114725643
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0005 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;

    // Test USER_UNLOCKED event
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_UNLOCKED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);

    // Test SCREEN_ON event
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);

    // Test BATTERY_CHANGED event
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 85);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 85);

    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0005 end";
}

HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0006 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction("usual.event.clone.CommonEventCloneState");
    want.SetParam("cloneState", 1);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0006 end";
}

HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0007 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction("usual.event.clone.CommonEventCloneState");
    want.SetParam("cloneState", 0);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0007 end";
}

HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0008 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction("usual.event.clone.CommonEventCloneState");
    want.SetParam("cloneState", -1);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0008 end";
}

HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0009 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction("usual.event.clone.CommonEventCloneState");
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0009 end";
}

HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0010 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction("usual.event.clone.CommonEventCloneState");
    want.SetParam("cloneState", 2);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0010 end";
}

/**
* @tc.number: Storage_subscriber_Constructor_test_0001
* @tc.name: Storage_subscriber_Constructor_test_0001
* @tc.desc: Test default constructor creates valid object with initial state
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_Constructor_test_0001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_Constructor_test_0001 begin";
    auto subscriber = std::make_shared<StorageCommonEventSubscriber>();
    ASSERT_TRUE(subscriber != nullptr);
    EXPECT_EQ(subscriber->deviceState_.load(), static_cast<uint8_t>(0x00));
    EXPECT_EQ(subscriber->batteryCapacity_.load(), 0);
    EXPECT_EQ(subscriber->isSleep_.load(), false);
    GTEST_LOG_(INFO) << "Storage_subscriber_Constructor_test_0001 end";
}

/**
* @tc.number: Storage_subscriber_Constructor_test_0002
* @tc.name: Storage_subscriber_Constructor_test_0002
* @tc.desc: Test constructor with CommonEventSubscribeInfo
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_Constructor_test_0002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_Constructor_test_0002 begin";
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriber = std::make_shared<StorageCommonEventSubscriber>(subscribeInfo);
    ASSERT_TRUE(subscriber != nullptr);
    EXPECT_EQ(subscriber->deviceState_.load(), static_cast<uint8_t>(0x00));
    GTEST_LOG_(INFO) << "Storage_subscriber_Constructor_test_0002 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0011
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0011
* @tc.desc: Test OnReceiveEvent with USER_SLEEP event (subscribed but no handler branch)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0011 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction("common.event.USER_NOT_CARE_CHARGE_SLEEP");
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0011 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0012
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0012
* @tc.desc: Test OnReceiveEvent SCREEN_OFF sets deviceState correctly
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0012 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_SCREEN_OFF);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0012 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0013
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0013
* @tc.desc: Test OnReceiveEvent SCREEN_ON clears screen off bit
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0013 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_SCREEN_OFF);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), static_cast<uint8_t>(0x00));
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0013 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0014
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0014
* @tc.desc: Test OnReceiveEvent POWER_CONNECTED sets power bit
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0014 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_POWER_CONNECTED);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0014 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0015
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0015
* @tc.desc: Test OnReceiveEvent POWER_DISCONNECTED clears power bit
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0015 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_POWER_CONNECTED);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), static_cast<uint8_t>(0x00));
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0015 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0016
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0016
* @tc.desc: Test OnReceiveEvent BATTERY_CHANGED updates batteryCapacity
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0016 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->batteryCapacity_.store(0);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 50);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 50);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0016 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0017
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0017
* @tc.desc: Test OnReceiveEvent BATTERY_CHANGED with default soc (no param set)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0017 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->batteryCapacity_.store(99);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 0);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0017 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0018
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0018
* @tc.desc: Test OnReceiveEvent PACKAGE_REMOVED with negative userId returns early
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0018 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetParam("userId", -100);
    want.SetParam("bundleName", std::string("com.test.app"));
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0018 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0019
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0019
* @tc.desc: Test OnReceiveEvent PACKAGE_REMOVED with userId=0 returns early
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0019 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetParam("userId", 0);
    want.SetParam("bundleName", std::string("com.test.app"));
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0019 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0020
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0020
* @tc.desc: Test OnReceiveEvent PACKAGE_REMOVED with large valid userId
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0020 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetParam("userId", 10738);
    want.SetParam("bundleName", std::string("com.test.large.app"));
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0020 end";
}

/**
* @tc.number: Storage_subscriber_UpdateDeviceState_test_0002
* @tc.name: Storage_subscriber_UpdateDeviceState_test_0002
* @tc.desc: Test UpdateDeviceState with STATE_POWER_CONNECTED set and unset
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_UpdateDeviceState_test_0002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0002 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);

    subscriberPtr_->UpdateDeviceState(STATE_POWER_CONNECTED, true);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_POWER_CONNECTED);

    subscriberPtr_->UpdateDeviceState(STATE_POWER_CONNECTED, false);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), static_cast<uint8_t>(0x00));
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0002 end";
}

/**
* @tc.number: Storage_subscriber_UpdateDeviceState_test_0003
* @tc.name: Storage_subscriber_UpdateDeviceState_test_0003
* @tc.desc: Test UpdateDeviceState sets both bits to reach STATE_CHARGING_SCREEN_OFF
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_UpdateDeviceState_test_0003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0003 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);

    subscriberPtr_->UpdateDeviceState(STATE_SCREEN_OFF, true);
    subscriberPtr_->UpdateDeviceState(STATE_POWER_CONNECTED, true);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0003 end";
}

/**
* @tc.number: Storage_subscriber_UpdateDeviceState_test_0004
* @tc.name: Storage_subscriber_UpdateDeviceState_test_0004
* @tc.desc: Test UpdateDeviceState clearing one bit keeps the other
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_UpdateDeviceState_test_0004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0004 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);

    subscriberPtr_->UpdateDeviceState(STATE_SCREEN_OFF, false);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_POWER_CONNECTED);

    subscriberPtr_->UpdateDeviceState(STATE_POWER_CONNECTED, false);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), static_cast<uint8_t>(0x00));
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0004 end";
}

/**
* @tc.number: Storage_subscriber_UpdateDeviceState_test_0005
* @tc.name: Storage_subscriber_UpdateDeviceState_test_0005
* @tc.desc: Test UpdateDeviceState idempotent set (set twice same value)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_UpdateDeviceState_test_0005, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0005 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);

    subscriberPtr_->UpdateDeviceState(STATE_SCREEN_OFF, true);
    uint8_t firstSet = subscriberPtr_->deviceState_.load();
    subscriberPtr_->UpdateDeviceState(STATE_SCREEN_OFF, true);
    uint8_t secondSet = subscriberPtr_->deviceState_.load();
    EXPECT_EQ(firstSet, secondSet);
    EXPECT_EQ(secondSet, STATE_SCREEN_OFF);
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0005 end";
}

/**
* @tc.number: Storage_subscriber_UpdateDeviceState_test_0006
* @tc.name: Storage_subscriber_UpdateDeviceState_test_0006
* @tc.desc: Test UpdateDeviceState unset when bit not set (no-op)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_UpdateDeviceState_test_0006, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0006 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);

    subscriberPtr_->UpdateDeviceState(STATE_SCREEN_OFF, false);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), static_cast<uint8_t>(0x00));

    subscriberPtr_->UpdateDeviceState(STATE_POWER_CONNECTED, false);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), static_cast<uint8_t>(0x00));
    GTEST_LOG_(INFO) << "Storage_subscriber_UpdateDeviceState_test_0006 end";
}

/**
* @tc.number: Storage_subscriber_HandleBatteryChangedEvent_test_0001
* @tc.name: Storage_subscriber_HandleBatteryChangedEvent_test_0001
* @tc.desc: Test HandleBatteryChangedEvent stores battery capacity
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_HandleBatteryChangedEvent_test_0001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_HandleBatteryChangedEvent_test_0001 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->batteryCapacity_.store(0);

    subscriberPtr_->HandleBatteryChangedEvent(50);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 50);

    subscriberPtr_->HandleBatteryChangedEvent(100);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 100);
    GTEST_LOG_(INFO) << "Storage_subscriber_HandleBatteryChangedEvent_test_0001 end";
}

/**
* @tc.number: Storage_subscriber_HandleBatteryChangedEvent_test_0002
* @tc.name: Storage_subscriber_HandleBatteryChangedEvent_test_0002
* @tc.desc: Test HandleBatteryChangedEvent with zero and negative values
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_HandleBatteryChangedEvent_test_0002, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_HandleBatteryChangedEvent_test_0002 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->batteryCapacity_.store(99);

    subscriberPtr_->HandleBatteryChangedEvent(0);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 0);

    subscriberPtr_->HandleBatteryChangedEvent(-5);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), -5);
    GTEST_LOG_(INFO) << "Storage_subscriber_HandleBatteryChangedEvent_test_0002 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0002
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0002
* @tc.desc: Test CheckAndTriggerStatistic triggers scan when charging+screenoff+battery>10
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0002 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);
    subscriberPtr_->batteryCapacity_.store(80);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0002 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0003
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0003
* @tc.desc: Test CheckAndTriggerStatistic with battery exactly 11 (just above threshold)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0003, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0003 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);
    subscriberPtr_->batteryCapacity_.store(11);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0003 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0004
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0004
* @tc.desc: Test CheckAndTriggerStatistic with battery exactly 10 (at threshold, not >10)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0004, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0004 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);
    subscriberPtr_->batteryCapacity_.store(10);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0004 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0005
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0005
* @tc.desc: Test CheckAndTriggerStatistic with battery 9 (below threshold)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0005, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0005 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);
    subscriberPtr_->batteryCapacity_.store(9);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0005 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0006
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0006
* @tc.desc: Test CheckAndTriggerStatistic not triggered when only screen off (no power)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0006 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_SCREEN_OFF);
    subscriberPtr_->batteryCapacity_.store(80);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0006 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0007
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0007
* @tc.desc: Test CheckAndTriggerStatistic not triggered when only power connected (screen on)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0007 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_POWER_CONNECTED);
    subscriberPtr_->batteryCapacity_.store(80);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0007 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0008
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0008
* @tc.desc: Test CheckAndTriggerStatistic not triggered when state is 0x00 (screen on, no power)
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0008, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0008 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    subscriberPtr_->batteryCapacity_.store(80);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0008 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0009
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0009
* @tc.desc: Test CheckAndTriggerStatistic with zero battery and charging+screenoff
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0009, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0009 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);
    subscriberPtr_->batteryCapacity_.store(0);
    subscriberPtr_->CheckAndTriggerStatistic();
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0009 end";
}

/**
* @tc.number: Storage_subscriber_CheckAndTriggerStatistic_test_0010
* @tc.name: Storage_subscriber_CheckAndTriggerStatistic_test_0010
* @tc.desc: Test CheckAndTriggerStatistic does not modify deviceState or battery
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_CheckAndTriggerStatistic_test_0010, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0010 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);
    subscriberPtr_->batteryCapacity_.store(50);
    uint8_t stateBefore = subscriberPtr_->deviceState_.load();
    int batteryBefore = subscriberPtr_->batteryCapacity_.load();
    subscriberPtr_->CheckAndTriggerStatistic();
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), stateBefore);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), batteryBefore);
    GTEST_LOG_(INFO) << "Storage_subscriber_CheckAndTriggerStatistic_test_0010 end";
}

/**
* @tc.number: Storage_subscriber_EventSequence_test_0001
* @tc.name: Storage_subscriber_EventSequence_test_0001
* @tc.desc: Test full event sequence: screen off, power connected, battery high triggers scan
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_EventSequence_test_0001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0001 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    subscriberPtr_->batteryCapacity_.store(0);
    AAFwk::Want want;

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventData eventData1(want);
    subscriberPtr_->OnReceiveEvent(eventData1);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_SCREEN_OFF);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    EventFwk::CommonEventData eventData2(want);
    subscriberPtr_->OnReceiveEvent(eventData2);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 85);
    EventFwk::CommonEventData eventData3(want);
    subscriberPtr_->OnReceiveEvent(eventData3);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 85);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0001 end";
}

/**
* @tc.number: Storage_subscriber_EventSequence_test_0002
* @tc.name: Storage_subscriber_EventSequence_test_0002
* @tc.desc: Test event sequence: trigger scan then screen on stops scan
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_EventSequence_test_0002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0002 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    subscriberPtr_->batteryCapacity_.store(0);
    AAFwk::Want want;

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventData eventData1(want);
    subscriberPtr_->OnReceiveEvent(eventData1);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    EventFwk::CommonEventData eventData2(want);
    subscriberPtr_->OnReceiveEvent(eventData2);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 90);
    EventFwk::CommonEventData eventData3(want);
    subscriberPtr_->OnReceiveEvent(eventData3);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    EventFwk::CommonEventData eventData4(want);
    subscriberPtr_->OnReceiveEvent(eventData4);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_POWER_CONNECTED);
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0002 end";
}

/**
* @tc.number: Storage_subscriber_EventSequence_test_0003
* @tc.name: Storage_subscriber_EventSequence_test_0003
* @tc.desc: Test event sequence: trigger scan then power disconnected stops scan
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_EventSequence_test_0003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0003 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    subscriberPtr_->batteryCapacity_.store(0);
    AAFwk::Want want;

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventData eventData1(want);
    subscriberPtr_->OnReceiveEvent(eventData1);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    EventFwk::CommonEventData eventData2(want);
    subscriberPtr_->OnReceiveEvent(eventData2);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 95);
    EventFwk::CommonEventData eventData3(want);
    subscriberPtr_->OnReceiveEvent(eventData3);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
    EventFwk::CommonEventData eventData4(want);
    subscriberPtr_->OnReceiveEvent(eventData4);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_SCREEN_OFF);
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0003 end";
}

/**
* @tc.number: Storage_subscriber_EventSequence_test_0004
* @tc.name: Storage_subscriber_EventSequence_test_0004
* @tc.desc: Test alternating screen on/off preserves power bit
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_EventSequence_test_0004, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0004 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    AAFwk::Want want;

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    EventFwk::CommonEventData eventDataPower(want);
    subscriberPtr_->OnReceiveEvent(eventDataPower);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_POWER_CONNECTED);

    for (int i = 0; i < 3; i++) {
        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
        EventFwk::CommonEventData eventDataOff(want);
        subscriberPtr_->OnReceiveEvent(eventDataOff);
        EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);

        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
        EventFwk::CommonEventData eventDataOn(want);
        subscriberPtr_->OnReceiveEvent(eventDataOn);
        EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_POWER_CONNECTED);
    }
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0004 end";
}

/**
* @tc.number: Storage_subscriber_EventSequence_test_0005
* @tc.name: Storage_subscriber_EventSequence_test_0005
* @tc.desc: Test alternating power connected/disconnected preserves screen bit
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_EventSequence_test_0005, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0005 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    AAFwk::Want want;

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventData eventDataScreen(want);
    subscriberPtr_->OnReceiveEvent(eventDataScreen);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_SCREEN_OFF);

    for (int i = 0; i < 3; i++) {
        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
        EventFwk::CommonEventData eventDataConn(want);
        subscriberPtr_->OnReceiveEvent(eventDataConn);
        EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);

        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
        EventFwk::CommonEventData eventDataDisc(want);
        subscriberPtr_->OnReceiveEvent(eventDataDisc);
        EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_SCREEN_OFF);
    }
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0005 end";
}

/**
* @tc.number: Storage_subscriber_EventSequence_test_0006
* @tc.name: Storage_subscriber_EventSequence_test_0006
* @tc.desc: Test full lifecycle: all events in sequence with state verification
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_EventSequence_test_0006, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0006 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    subscriberPtr_->batteryCapacity_.store(0);
    AAFwk::Want want;

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 5);
    EventFwk::CommonEventData eventData1(want);
    subscriberPtr_->OnReceiveEvent(eventData1);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 5);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventData eventData2(want);
    subscriberPtr_->OnReceiveEvent(eventData2);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_SCREEN_OFF);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    EventFwk::CommonEventData eventData3(want);
    subscriberPtr_->OnReceiveEvent(eventData3);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 100);
    EventFwk::CommonEventData eventData4(want);
    subscriberPtr_->OnReceiveEvent(eventData4);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 100);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_CHARGING_SCREEN_OFF);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    EventFwk::CommonEventData eventData5(want);
    subscriberPtr_->OnReceiveEvent(eventData5);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), STATE_POWER_CONNECTED);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
    EventFwk::CommonEventData eventData6(want);
    subscriberPtr_->OnReceiveEvent(eventData6);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), static_cast<uint8_t>(0x00));
    GTEST_LOG_(INFO) << "Storage_subscriber_EventSequence_test_0006 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0021
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0021
* @tc.desc: Test OnReceiveEvent PACKAGE_REMOVED with valid userId and empty bundleName
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0021, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0021 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetParam("userId", 100);
    want.SetParam("bundleName", std::string(""));
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0021 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0022
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0022
* @tc.desc: Test OnReceiveEvent BATTERY_CHANGED with negative soc value
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0022, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0022 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->batteryCapacity_.store(50);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", -20);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), -20);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0022 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0023
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0023
* @tc.desc: Test OnReceiveEvent BATTERY_CHANGED with soc over 100
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0023, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0023 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    want.SetParam("soc", 200);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->batteryCapacity_.load(), 200);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0023 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0024
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0024
* @tc.desc: Test OnReceiveEvent with various unknown actions
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0024, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0024 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    std::vector<std::string> unknownActions = {
        "unknown.action.test",
        "usual.event.UNKNOWN_EVENT",
        "common.event.SOME_RANDOM_EVENT",
        " ",
        "ACTION_WITHOUT_MATCH"
    };
    for (const auto& action : unknownActions) {
        EventFwk::CommonEventData testData;
        AAFwk::Want want;
        want.SetAction(action);
        testData.SetWant(want);
        subscriberPtr_->OnReceiveEvent(testData);
    }
    ASSERT_TRUE(true);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0024 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0025
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0025
* @tc.desc: Test OnReceiveEvent CLONE_EVENT_NAME does not affect deviceState
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0025, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0025 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(0x00);
    uint8_t stateBefore = subscriberPtr_->deviceState_.load();

    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction("usual.event.clone.CommonEventCloneState");
    want.SetParam("cloneState", 1);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), stateBefore);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0025 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0026
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0026
* @tc.desc: Test OnReceiveEvent USER_UNLOCKED does not affect deviceState
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0026, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0026 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_POWER_CONNECTED);
    uint8_t stateBefore = subscriberPtr_->deviceState_.load();

    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_UNLOCKED);
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), stateBefore);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0026 end";
}

/**
* @tc.number: Storage_subscriber_OnReceiveEvent_test_0027
* @tc.name: Storage_subscriber_OnReceiveEvent_test_0027
* @tc.desc: Test OnReceiveEvent PACKAGE_REMOVED does not affect deviceState
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 2
*/
HWTEST_F(StorageCommonEventSubscriberTest, Storage_subscriber_OnReceiveEvent_test_0027, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0027 begin";
    ASSERT_TRUE(subscriberPtr_ != nullptr);
    subscriberPtr_->deviceState_.store(STATE_CHARGING_SCREEN_OFF);
    uint8_t stateBefore = subscriberPtr_->deviceState_.load();

    EventFwk::CommonEventData testData;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetParam("userId", 100);
    want.SetParam("bundleName", std::string("com.test.app"));
    testData.SetWant(want);
    subscriberPtr_->OnReceiveEvent(testData);
    EXPECT_EQ(subscriberPtr_->deviceState_.load(), stateBefore);
    GTEST_LOG_(INFO) << "Storage_subscriber_OnReceiveEvent_test_0027 end";
}
} // namespace StorageManager
} // namespace OHOS
