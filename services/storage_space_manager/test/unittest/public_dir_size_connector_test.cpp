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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "file_data_service/public_dir_size_connector.h"
#include "storage_space_manager_errno.h"

namespace OHOS {
namespace StorageSpaceManager {
using namespace testing;
using namespace testing::ext;

namespace {
    constexpr int32_t INVALID_USER_ID = -1;
    constexpr int32_t NORMAL_USER_ID = 100;
    constexpr int64_t NON_ZERO_SIZE = 1024;

    bool AdapterLoaded()
    {
        auto &connector = PublicDirSizeConnector::GetInstance();
        return connector.adapterHandle_ != nullptr && !connector.loadFailed_;
    }
}

class PublicDirSizeConnectorTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp()
    {
        // private=public: reset the singleton's cached failure state so cases stay independent
        auto &connector = PublicDirSizeConnector::GetInstance();
        connector.loadFailed_ = false;
        connector.queryFunc_ = nullptr;
        connector.adapterHandle_ = nullptr;
    }
    void TearDown() {}
};

/**
 * @tc.number: SUB_STORAGE_PublicDirSizeConnector_GetInstance_0000
 * @tc.name: PublicDirSizeConnector_GetInstance_0000
 * @tc.desc: Verify GetInstance returns the same singleton instance.
 * @tc.type: FUNC
 * @tc.require: SR20260812026556
 */
HWTEST_F(PublicDirSizeConnectorTest, PublicDirSizeConnector_GetInstance_0000, TestSize.Level1)
{
    auto &first = PublicDirSizeConnector::GetInstance();
    auto &second = PublicDirSizeConnector::GetInstance();
    EXPECT_EQ(&first, &second);
}

/**
 * @tc.number: SUB_STORAGE_PublicDirSizeConnector_LoadFailed_0000
 * @tc.name: PublicDirSizeConnector_LoadFailed_0000
 * @tc.desc: Path A (adapter so absent, pure UT env): returns E_FAIL, zeroes output, loadFailed_ set.
 *           Path B (adapter deployed, device env): returns E_OK, output written >= 0.
 * @tc.type: FUNC
 * @tc.require: SR20260812026556
 */
HWTEST_F(PublicDirSizeConnectorTest, PublicDirSizeConnector_LoadFailed_0000, TestSize.Level1)
{
    auto &connector = PublicDirSizeConnector::GetInstance();
    int64_t publicDirSize = NON_ZERO_SIZE;
    int32_t ret = connector.QueryPublicDirSize(NORMAL_USER_ID, publicDirSize);
    if (AdapterLoaded()) {
        // Device path: adapter deployed, size goes through the real chain (SA8650 -> file data service)
        EXPECT_EQ(ret, E_OK);
        EXPECT_GE(publicDirSize, 0);
        EXPECT_FALSE(connector.loadFailed_);
    } else {
        // Unit-test env: adapter .so missing, dlopen fails
        EXPECT_EQ(ret, E_FAIL);
        EXPECT_EQ(publicDirSize, 0);
        EXPECT_TRUE(connector.loadFailed_);
    }
}

/**
 * @tc.number: SUB_STORAGE_PublicDirSizeConnector_LoadFailedCached_0000
 * @tc.name: PublicDirSizeConnector_LoadFailedCached_0000
 * @tc.desc: Path A: load failure is cached, repeated calls fail fast without re-attempting dlopen.
 *           Path B: adapter loaded, repeated calls keep succeeding with consistent results.
 * @tc.type: FUNC
 * @tc.require: SR20260812026556
 */
HWTEST_F(PublicDirSizeConnectorTest, PublicDirSizeConnector_LoadFailedCached_0000, TestSize.Level1)
{
    auto &connector = PublicDirSizeConnector::GetInstance();
    int64_t publicDirSize = NON_ZERO_SIZE;
    int32_t firstRet = connector.QueryPublicDirSize(NORMAL_USER_ID, publicDirSize);
    if (!AdapterLoaded()) {
        // Path A: load-failure path
        ASSERT_EQ(firstRet, E_FAIL);
        ASSERT_TRUE(connector.loadFailed_);
        ASSERT_EQ(connector.adapterHandle_, nullptr);

        // Second call: loadFailed_ cached, no dlopen retry, fail fast
        int64_t secondSize = NON_ZERO_SIZE;
        int32_t secondRet = connector.QueryPublicDirSize(NORMAL_USER_ID, secondSize);
        EXPECT_EQ(secondRet, E_FAIL);
        EXPECT_EQ(secondSize, 0);
        EXPECT_TRUE(connector.loadFailed_);
        EXPECT_EQ(connector.adapterHandle_, nullptr);
        EXPECT_EQ(connector.queryFunc_, nullptr);
    } else {
        // Path B: the real chain succeeds on repeated calls; handle/func pointer reused
        ASSERT_EQ(firstRet, E_OK);
        int64_t secondSize = NON_ZERO_SIZE;
        int32_t secondRet = connector.QueryPublicDirSize(NORMAL_USER_ID, secondSize);
        EXPECT_EQ(secondRet, E_OK);
        EXPECT_GE(secondSize, 0);
        EXPECT_NE(connector.adapterHandle_, nullptr);
        EXPECT_NE(connector.queryFunc_, nullptr);
    }
}

/**
 * @tc.number: SUB_STORAGE_PublicDirSizeConnector_InvalidUserId_0000
 * @tc.name: PublicDirSizeConnector_InvalidUserId_0000
 * @tc.desc: Verify the connector does not crash on invalid userId (argument is only forwarded,
 *           final validation is done by the closed-source adapter).
 * @tc.type: FUNC
 * @tc.require: SR20260812026556
 */
HWTEST_F(PublicDirSizeConnectorTest, PublicDirSizeConnector_InvalidUserId_0000, TestSize.Level1)
{
    auto &connector = PublicDirSizeConnector::GetInstance();
    int64_t publicDirSize = NON_ZERO_SIZE;
    // Invalid userId: load-failure path in unit tests; rejected by the closed-source
    // adapter on devices. Neither path may crash; the core contract is that invalid
    // input never yields a successful result.
    int32_t ret = connector.QueryPublicDirSize(INVALID_USER_ID, publicDirSize);
    EXPECT_NE(ret, E_OK);
    EXPECT_GE(publicDirSize, 0);
}
} // namespace StorageSpaceManager
} // namespace OHOS
