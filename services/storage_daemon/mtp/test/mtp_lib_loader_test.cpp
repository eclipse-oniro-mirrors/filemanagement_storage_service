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

#include "mtp/mtp_lib_loader.h"

namespace {
int g_dlopenCalls = 0;
}

extern "C" {
void *__wrap_dlopen(const char *filename, int flags) // NOLINT: C-style function
{
    (void)filename;
    (void)flags;
    ++g_dlopenCalls;
    return nullptr;
}

void *__wrap_dlsym(void *handle, const char *symbol) // NOLINT: C-style function
{
    (void)handle;
    (void)symbol;
    return nullptr;
}

int __wrap_dlclose(void *handle) // NOLINT: C-style function
{
    (void)handle;
    return 0;
}
}
namespace OHOS {
namespace StorageDaemon {
using namespace testing::ext;

class MtpLibLoaderTest : public testing::Test {};

/**
 * @tc.name: CheckIsMtpDevice_Downgrade_001
 * @tc.desc: dlopen fails, CheckIsMtpDevice must downgrade to non-MTP instead of crashing.
 * @tc.type: FUNC
 */
HWTEST_F(MtpLibLoaderTest, CheckIsMtpDevice_Downgrade_001, TestSize.Level1)
{
    int result = MtpLibLoader::GetInstance().CheckIsMtpDevice(0x06, 0x12d1, 0x107e);
    EXPECT_EQ(result, 0);
    EXPECT_GE(g_dlopenCalls, 1);
}

/**
 * @tc.name: DetectRawDevices_Downgrade_001
 * @tc.desc: dlopen fails, DetectRawDevices must downgrade to no-device with null outputs.
 * @tc.type: FUNC
 */
HWTEST_F(MtpLibLoaderTest, DetectRawDevices_Downgrade_001, TestSize.Level1)
{
    LIBMTP_raw_device_t *rawDevices = nullptr;
    int numDevices = -1;
    LIBMTP_error_number_t err = MtpLibLoader::GetInstance().DetectRawDevices(&rawDevices, &numDevices);
    EXPECT_EQ(err, LIBMTP_ERROR_NO_DEVICE_ATTACHED);
    EXPECT_EQ(rawDevices, nullptr);
    EXPECT_EQ(numDevices, 0);
}

/**
 * @tc.name: DetectRawDevices_NullArgs_001
 * @tc.desc: DetectRawDevices with null output args must not crash, still downgrade to no-device.
 * @tc.type: FUNC
 */
HWTEST_F(MtpLibLoaderTest, DetectRawDevices_NullArgs_001, TestSize.Level1)
{
    LIBMTP_error_number_t err = MtpLibLoader::GetInstance().DetectRawDevices(nullptr, nullptr);
    EXPECT_EQ(err, LIBMTP_ERROR_NO_DEVICE_ATTACHED);
}
} // namespace StorageDaemon
} // namespace OHOS