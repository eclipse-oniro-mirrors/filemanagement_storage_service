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
#include "mtp/mtp_lib_loader.h"

#include <dlfcn.h>
#include <mutex>

#include "storage_service_log.h"

namespace OHOS {
namespace StorageDaemon {
namespace {
constexpr const char *LIBMTP_SO_NAME = "libmtp.z.so";
constexpr const char *MTP_CHECK_IS_MTP_DEVICE_FUNC = "LIBMTP_check_is_mtp_device";
constexpr const char *MTP_DETECT_RAW_DEVICES_FUNC = "LIBMTP_Detect_Raw_Devices";
std::once_flag g_mtpLibOnceFlag;
}

MtpLibLoader::~MtpLibLoader()
{
    Deinit();
}

bool MtpLibLoader::EnsureLoaded()
{
    std::call_once(g_mtpLibOnceFlag, [this] { Init(); });
    return checkIsMtpDeviceFunc_ != nullptr && detectRawDevicesFunc_ != nullptr;
}

void MtpLibLoader::Init()
{
    if (libmtpHandle_ != nullptr) {
        LOGW("[L2:MtpLibLoader] Init: libmtp already loaded, skip");
        return;
    }
    libmtpHandle_ = dlopen(LIBMTP_SO_NAME, RTLD_LAZY);
    if (libmtpHandle_ == nullptr) {
        LOGE("[L2:MtpLibLoader] Init: dlopen %{public}s failed: %{public}s", LIBMTP_SO_NAME, dlerror());
        return;
    }

    dlerror();
    checkIsMtpDeviceFunc_ = reinterpret_cast<MtpLibLoader::MtpCheckIsMtpDeviceFunc>(
        dlsym(libmtpHandle_, MTP_CHECK_IS_MTP_DEVICE_FUNC));
    if (dlerror() != nullptr || checkIsMtpDeviceFunc_ == nullptr) {
        LOGE("[L2:MtpLibLoader] Init: dlsym %{public}s failed", MTP_CHECK_IS_MTP_DEVICE_FUNC);
        Deinit();
        return;
    }

    dlerror();
    detectRawDevicesFunc_ = reinterpret_cast<MtpLibLoader::MtpDetectRawDevicesFunc>(
        dlsym(libmtpHandle_, MTP_DETECT_RAW_DEVICES_FUNC));
    if (dlerror() != nullptr || detectRawDevicesFunc_ == nullptr) {
        LOGE("[L2:MtpLibLoader] Init: dlsym %{public}s failed", MTP_DETECT_RAW_DEVICES_FUNC);
        Deinit();
        return;
    }

    LOGI("[L2:MtpLibLoader] Init: libmtp loaded on demand");
}

void MtpLibLoader::Deinit()
{
    if (libmtpHandle_ != nullptr) {
        dlclose(libmtpHandle_);
        libmtpHandle_ = nullptr;
    }
    checkIsMtpDeviceFunc_ = nullptr;
    detectRawDevicesFunc_ = nullptr;
}

int MtpLibLoader::CheckIsMtpDevice(uint8_t deviceClass, uint16_t vendorId, uint16_t productId)
{
    if (!EnsureLoaded()) {
        LOGW("[L2:MtpLibLoader] CheckIsMtpDevice: libmtp not loaded, downgrade to non-MTP");
        return 0;
    }
    return checkIsMtpDeviceFunc_(deviceClass, vendorId, productId);
}

LIBMTP_error_number_t MtpLibLoader::DetectRawDevices(LIBMTP_raw_device_t **rawDevices, int *numDevices)
{
    if (rawDevices != nullptr) {
        *rawDevices = nullptr;
    }
    if (numDevices != nullptr) {
        *numDevices = 0;
    }
    if (!EnsureLoaded()) {
        LOGW("[L2:MtpLibLoader] DetectRawDevices: libmtp not loaded, downgrade to no device");
        return LIBMTP_ERROR_NO_DEVICE_ATTACHED;
    }
    return detectRawDevicesFunc_(rawDevices, numDevices);
}
} // namespace StorageDaemon
} // namespace OHOS