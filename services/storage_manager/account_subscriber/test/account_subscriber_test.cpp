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

#include <cstdio>
#include <gtest/gtest.h>

#include "account_subscriber/account_subscriber.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "common_event_manager.h"
namespace OHOS {
namespace StorageManager {
using namespace std;
using namespace testing::ext;

class AccountSubscriberTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() {};
    void TearDown() {};
public:
    static inline shared_ptr<AccountSubscriber> accountSubscriber_ = nullptr;
};

void AccountSubscriberTest::SetUpTestCase()
{
    accountSubscriber_ = make_shared<AccountSubscriber>();
}

void AccountSubscriberTest::TearDownTestCase(void)
{
    accountSubscriber_ = nullptr;
}

/**
* @tc.number: SUB_STORAGE_Account_Subscriber_Subscriber_0000
* @tc.name: Account_Subscriber_Subscriber_0000
* @tc.desc: Test function of Subscriber interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
 */
HWTEST_F(AccountSubscriberTest, Account_Subscriber_Subscriber_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Account_Subscriber_Subscriber_0000-begin";

    ASSERT_TRUE(accountSubscriber_ != nullptr);
    AccountSubscriber::Subscriber();
    GTEST_LOG_(INFO) << "Account_Subscriber_Subscriber_0000 end";
}

/**
* @tc.number: SUB_STORAGE_Account_Subscriber_Subscriber_0001
* @tc.name: Account_Subscriber_Subscriber_0001
* @tc.desc: Test function of Subscriber interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
 */
HWTEST_F(AccountSubscriberTest, Account_Subscriber_Subscriber_0001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Account_Subscriber_Subscriber_0001-begin";
    accountSubscriber_ == nullptr;
    ASSERT_TRUE(accountSubscriber_ == nullptr);
    AccountSubscriber::Subscriber();
    GTEST_LOG_(INFO) << "Account_Subscriber_Subscriber_0001 end";
}

/**
* @tc.number: SUB_STORAGE_Account_Subscriber_ResetUserEventRecord_0000
* @tc.name: Account_Subscriber_ResetUserEventRecord_0000
* @tc.desc: Test function of ResetUserEventRecord interface for SUCCESS.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: SR000GGUPF
 */
HWTEST_F(AccountSubscriberTest, Account_Subscriber_ResetUserEventRecord_0000, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "Account_Subscriber_ResetUserEventRecord_0000-begin";

    ASSERT_TRUE(accountSubscriber_ != nullptr);
    AccountSubscriber::Subscriber();
    int32_t userId = 99;
    AccountSubscriber::ResetUserEventRecord(userId);
    GTEST_LOG_(INFO) << "Account_Subscriber_ResetUserEventRecord_0000 end";
}
}
}