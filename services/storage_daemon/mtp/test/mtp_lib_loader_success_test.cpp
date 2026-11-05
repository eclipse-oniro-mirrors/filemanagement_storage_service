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
#include <cstring>

#include "mtp/mtp_lib_loader.h"

namespace OHOS {
namespace StorageDaemon {

namespace {
int g_dlopenCalls = 0;
int g_dlsymCalls = 0;
int g_dlcloseCalls = 0;

int FakeCheckIsMtpDevice(uint8_t deviceClass, uint16_t vendorId, uint16_t productId)
{
    (void)deviceClass;
    (void)vendorId;
    (void)productId;
    return 1;
}

LIBMTP_error_number_t FakeDetectRawDevices(LIBMTP_raw_device_t **rawDevices, int *numDevices)
{
    if (rawDevices != nullptr) {
        *rawDevices = nullptr;
    }
    if (numDevices != nullptr) {
        *numDevices = 0;
    }
    return LIBMTP_ERROR_NO_DEVICE_ATTACHED;
}
}

// Mock dlfcn via linker --wrap to exercise the libmtp load-success path.
extern "C" {
void *__wrap_dlopen(const char *filename, int flags) // NOLINT: C-style function
{
    (void)filename;
    (void)flags;
    ++g_dlopenCalls;
    return reinterpret_cast<void *>(0x1);
}

void *__wrap_dlsym(void *handle, const char *symbol) // NOLINT: C-style function
{
    (void)handle;
    ++g_dlsymCalls;
    if (std::strcmp(symbol, "LIBMTP_check_is_mtp_device") == 0) {
        return reinterpret_cast<void *>(&FakeCheckIsMtpDevice);
    }
    if (std::strcmp(symbol, "LIBMTP_Detect_Raw_Devices") == 0) {
        return reinterpret_cast<void *>(&FakeDetectRawDevices);
    }
    return nullptr;
}

int __wrap_dlclose(void *handle) // NOLINT: C-style function
{
    (void)handle;
    ++g_dlcloseCalls;
    return 0;
}
}

using namespace testing::ext;

class MtpLibLoaderSuccessTest : public testing::Test {};

/**
 * @tc.name: Init_Success_001
 * @tc.desc: dlopen/dlsym success triggers load-once and resolves both symbols.
 * @tc.type: FUNC
 */
HWTEST_F(MtpLibLoaderSuccessTest, Init_Success_001, TestSize.Level1)
{
    int result = MtpLibLoader::GetInstance().CheckIsMtpDevice(0x06, 0x12d1, 0x107e);
    EXPECT_EQ(result, 1);
    EXPECT_GE(g_dlopenCalls, 1);
    EXPECT_GE(g_dlsymCalls, 2);
}

/**
 * @tc.name: CheckIsMtpDevice_Success_001
 * @tc.desc: libmtp loaded, CheckIsMtpDevice goes through the resolved function pointer.
 * @tc.type: FUNC
 */
HWTEST_F(MtpLibLoaderSuccessTest, CheckIsMtpDevice_Success_001, TestSize.Level1)
{
    int result = MtpLibLoader::GetInstance().CheckIsMtpDevice(0x06, 0x12d1, 0x107e);
    EXPECT_EQ(result, 1);
}

/**
 * @tc.name: DetectRawDevices_Success_001
 * @tc.desc: libmtp loaded, DetectRawDevices goes through the resolved function pointer.
 * @tc.type: FUNC
 */
HWTEST_F(MtpLibLoaderSuccessTest, DetectRawDevices_Success_001, TestSize.Level1)
{
    LIBMTP_raw_device_t *rawDevices = nullptr;
    int numDevices = -1;
    LIBMTP_error_number_t err = MtpLibLoader::GetInstance().DetectRawDevices(&rawDevices, &numDevices);
    EXPECT_EQ(err, LIBMTP_ERROR_NO_DEVICE_ATTACHED);
    EXPECT_EQ(rawDevices, nullptr);
    EXPECT_EQ(numDevices, 0);
}
} // namespace StorageDaemon
} // namespace OHOS