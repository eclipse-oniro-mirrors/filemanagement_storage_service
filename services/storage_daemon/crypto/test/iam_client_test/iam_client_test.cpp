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

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "iam_client.h"
#include "storage_service_errno.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS::StorageDaemon {
class IamClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void IamClientTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
}

void IamClientTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase";
}

void IamClientTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp";
}

void IamClientTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
}

/**
 * @tc.name: iam_client_GetSecureUid
 * @tc.desc: Verify the iam_client GetSecureUid.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(IamClientTest, iam_client_GetSecureUid, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "iam_client_GetSecureUid start";
    uint32_t userId = 100;
    uint64_t secureUid = 1;
    IamClient &client = IamClient::GetInstance();
    EXPECT_FALSE(client.GetSecureUid(userId, secureUid));
    EXPECT_EQ(secureUid, 1);
    GTEST_LOG_(INFO) << "iam_client_GetSecureUid end";
}

/**
 * @tc.name: iam_client_NotifyGetSecureUid
 * @tc.desc: Verify the iam_client NotifyGetSecureUid.
 * @tc.type: FUNC
 * @tc.require: AR000GK0BP
 */
HWTEST_F(IamClientTest, iam_client_NotifyGetSecureUid, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "iam_client_NotifyGetSecureUid start";
    IamClient &client = IamClient::GetInstance();
    EXPECT_EQ(client.secureUidStatus_, 0);
    int32_t result = client.NotifyGetSecureUid();
    EXPECT_EQ(result, 0);
    EXPECT_EQ(client.secureUidStatus_, 0);
    GTEST_LOG_(INFO) << "iam_client_NotifyGetSecureUid end";
}
} // OHOS::StorageDaemon