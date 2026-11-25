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

#include <dlfcn.h>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <gtest/gtest.h>
#include <singleton.h>

#include "cache_clean_controller/cache_clean_controller.h"
#include "include/i_quota_calculator.h"
#include "storage_space_manager_errno.h"

namespace OHOS {
namespace StorageSpaceManager {
namespace {
constexpr int32_t TOP_COUNT = 20;
constexpr int32_t QUOTA_SIZE = 100;
constexpr const char *EXT_ADAPTER_LIBRARY_NAME = "libstorage_service_ext_adapter.z.so";

bool IsExtAdapterAvailable()
{
    void *handle = dlopen(EXT_ADAPTER_LIBRARY_NAME, RTLD_NOW | RTLD_NODELETE | RTLD_NOLOAD);
    if (handle == nullptr) {
        handle = dlopen(EXT_ADAPTER_LIBRARY_NAME, RTLD_NOW | RTLD_NODELETE);
    }
    if (handle == nullptr) {
        return false;
    }
    if (dlsym(handle, "CreateQuotaCalculatorObject") == nullptr) {
        dlclose(handle);
        return false;
    }
    dlclose(handle);
    return true;
}

class LoadQuotaCalculatorMock : public IQuotaCalculator {
public:
    void Init() override {}
    int32_t ParseConfig(const std::string &configPath) override { return E_OK; }
    CacheAutoCleanSwitch GetCacheAutoCleanSwitch() const override { return CacheAutoCleanSwitch::OPEN; }
    int32_t GetAutoCacheCleanSpan() const override { return 168; }
    std::unordered_map<std::string, int32_t> GetSystemAppCacheSize() const override { return {}; }
    int32_t GetTopRankingHoursSpan() const override { return 336; }
    int32_t GetNoUseHoursForCleanAll() const override { return 2160; }
    int32_t GetTopAppCount(int64_t totalSize, int32_t &topCount) const override
    {
        topCount = TOP_COUNT;
        return E_OK;
    }
    int32_t GetQuotaByRank(int32_t appRank, int64_t totalStorage, int32_t &quota) override
    {
        quota = QUOTA_SIZE;
        return E_OK;
    }
    int32_t GetQuotasByRanks(const std::vector<int32_t> &appRanks, int64_t totalStorage,
                             std::unordered_map<int32_t, int32_t> &quotas) override { return E_OK; }
    int32_t GetQuota(const std::string &tierName, int64_t totalStorage, int32_t &quota) override
    {
        quota = QUOTA_SIZE;
        return E_OK;
    }
    int32_t GetAllQuotas(int64_t totalStorage, std::map<std::string, int32_t> &quotas) override { return E_OK; }
    bool IsConfigLoaded() const override { return true; }
};

class QuotaCalculatorStateGuard {
public:
    explicit QuotaCalculatorStateGuard(CacheCleanController *controller)
        : controller_(controller), originalQuota_(controller->quotaCalculator_),
          originalLoaded_(controller->quotaCalculatorSoLoaded_), originalHandle_(controller->quotaCalculatorSoHandle_)
    {}

    ~QuotaCalculatorStateGuard()
    {
        controller_->quotaCalculator_ = originalQuota_;
        if (closeHandleOnExit_ && controller_->quotaCalculatorSoHandle_ != nullptr) {
            dlclose(controller_->quotaCalculatorSoHandle_);
        }
        controller_->quotaCalculatorSoLoaded_ = originalLoaded_;
        controller_->quotaCalculatorSoHandle_ = originalHandle_;
    }

    void CloseHandleOnExit()
    {
        closeHandleOnExit_ = true;
    }

private:
    CacheCleanController *controller_;
    std::shared_ptr<IQuotaCalculator> originalQuota_;
    bool originalLoaded_;
    void *originalHandle_;
    bool closeHandleOnExit_ = false;
};
} // namespace

class CacheCleanControllerLoadQuotaTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        controller_ = DelayedSingleton<CacheCleanController>::GetInstance().get();
        mockQuota_ = std::make_shared<LoadQuotaCalculatorMock>();
    }

    static CacheCleanController *controller_;
    static std::shared_ptr<LoadQuotaCalculatorMock> mockQuota_;
};

CacheCleanController *CacheCleanControllerLoadQuotaTest::controller_ = nullptr;
std::shared_ptr<LoadQuotaCalculatorMock> CacheCleanControllerLoadQuotaTest::mockQuota_ = nullptr;

/**
 * @tc.number: SUB_STORAGE_CacheCleanController_LoadQuotaCalculator_0001
 * @tc.name: LoadQuotaCalculator_AlreadyLoadedNonNull
 * @tc.desc: Test LoadQuotaCalculator returns true and re-inits when already loaded with a live instance.
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR20260812027362
 */
HWTEST_F(CacheCleanControllerLoadQuotaTest, LoadQuotaCalculator_AlreadyLoadedNonNull, TestSize.Level1)
{
    ASSERT_NE(controller_, nullptr);
    QuotaCalculatorStateGuard guard(controller_);
    controller_->quotaCalculatorSoLoaded_ = true;
    controller_->quotaCalculator_ = mockQuota_;

    EXPECT_TRUE(controller_->LoadQuotaCalculator());
    EXPECT_EQ(controller_->quotaCalculator_, mockQuota_);
}

/**
 * @tc.number: SUB_STORAGE_CacheCleanController_LoadQuotaCalculator_0002
 * @tc.name: LoadQuotaCalculator_LoadedFlagButNullInstance
 * @tc.desc: Test LoadQuotaCalculator reloads when the loaded flag is set but the instance is null.
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: SR20260812027362
 */
HWTEST_F(CacheCleanControllerLoadQuotaTest, LoadQuotaCalculator_LoadedFlagButNullInstance, TestSize.Level2)
{
    ASSERT_NE(controller_, nullptr);
    const bool adapterAvailable = IsExtAdapterAvailable();
    QuotaCalculatorStateGuard guard(controller_);
    guard.CloseHandleOnExit();
    controller_->quotaCalculatorSoLoaded_ = true;
    controller_->quotaCalculator_ = nullptr;
    controller_->quotaCalculatorSoHandle_ = nullptr;

    const bool ret = controller_->LoadQuotaCalculator();
    EXPECT_EQ(ret, adapterAvailable);
    if (adapterAvailable) {
        EXPECT_NE(controller_->quotaCalculator_, nullptr);
        EXPECT_NE(controller_->quotaCalculatorSoHandle_, nullptr);
        EXPECT_TRUE(controller_->quotaCalculatorSoLoaded_);
    } else {
        EXPECT_EQ(controller_->quotaCalculator_, nullptr);
        EXPECT_EQ(controller_->quotaCalculatorSoHandle_, nullptr);
    }
}

/**
 * @tc.number: SUB_STORAGE_CacheCleanController_LoadQuotaCalculator_0003
 * @tc.name: LoadQuotaCalculator_AdapterAvailability
 * @tc.desc: Test LoadQuotaCalculator result and state against actual adapter availability.
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: SR20260812027362
 */
HWTEST_F(CacheCleanControllerLoadQuotaTest, LoadQuotaCalculator_AdapterAvailability, TestSize.Level2)
{
    ASSERT_NE(controller_, nullptr);
    const bool adapterAvailable = IsExtAdapterAvailable();
    QuotaCalculatorStateGuard guard(controller_);
    guard.CloseHandleOnExit();
    controller_->quotaCalculatorSoLoaded_ = false;
    controller_->quotaCalculator_ = nullptr;
    controller_->quotaCalculatorSoHandle_ = nullptr;

    const bool ret = controller_->LoadQuotaCalculator();
    EXPECT_EQ(ret, adapterAvailable);
    if (adapterAvailable) {
        EXPECT_NE(controller_->quotaCalculator_, nullptr);
        EXPECT_NE(controller_->quotaCalculatorSoHandle_, nullptr);
        EXPECT_TRUE(controller_->quotaCalculatorSoLoaded_);
    } else {
        EXPECT_EQ(controller_->quotaCalculator_, nullptr);
        EXPECT_EQ(controller_->quotaCalculatorSoHandle_, nullptr);
    }
}

} // namespace StorageSpaceManager
} // namespace OHOS
