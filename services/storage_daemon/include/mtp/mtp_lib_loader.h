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
#ifndef OHOS_STORAGE_DAEMON_MTP_LIB_LOADER_H
#define OHOS_STORAGE_DAEMON_MTP_LIB_LOADER_H

#include <cstdint>
#include <nocopyable.h>

// Minimal libmtp ABI contract, mirrored from libmtp.h to avoid including the
// original header on platforms that lack the libmtp component.
extern "C" {
struct LIBMTP_device_entry_struct {
    char *vendor;         // vendor string
    uint16_t vendor_id;   // vendor id
    char *product;        // product string
    uint16_t product_id;  // product id
    uint32_t device_flags;
};
using LIBMTP_device_entry_t = LIBMTP_device_entry_struct;

struct LIBMTP_raw_device_struct {
    LIBMTP_device_entry_t device_entry;
    uint32_t bus_location;  // usb bus location
    uint8_t devnum;         // device number on the bus
};
using LIBMTP_raw_device_t = LIBMTP_raw_device_struct;

enum LIBMTP_error_number_t {
    LIBMTP_ERROR_NONE = 0,
    LIBMTP_ERROR_GENERAL,
    LIBMTP_ERROR_PTP_LAYER,
    LIBMTP_ERROR_USB_LAYER,
    LIBMTP_ERROR_MEMORY_ALLOCATION,
    LIBMTP_ERROR_NO_DEVICE_ATTACHED,
    LIBMTP_ERROR_STORAGE_FULL,
    LIBMTP_ERROR_CONNECTING,
    LIBMTP_ERROR_CANCELLED
};
}

namespace OHOS {
namespace StorageDaemon {

class MtpLibLoader : public NoCopyable {
public:
    static MtpLibLoader &GetInstance()
    {
        static MtpLibLoader instance;
        return instance;
    }

    // Returns 0 (non-MTP) if libmtp fails to load.
    int CheckIsMtpDevice(uint8_t deviceClass, uint16_t vendorId, uint16_t productId);

    // Returns LIBMTP_ERROR_NO_DEVICE_ATTACHED with null outputs if libmtp fails to load.
    LIBMTP_error_number_t DetectRawDevices(LIBMTP_raw_device_t **rawDevices, int *numDevices);

    MtpLibLoader(const MtpLibLoader &) = delete;
    MtpLibLoader &operator=(const MtpLibLoader &) = delete;

private:
    MtpLibLoader() = default;
    ~MtpLibLoader();

    void Init();
    void Deinit();
    bool EnsureLoaded();

private:
    using MtpCheckIsMtpDeviceFunc = int (*)(uint8_t deviceClass, uint16_t vendorId, uint16_t productId);
    using MtpDetectRawDevicesFunc = LIBMTP_error_number_t (*)(LIBMTP_raw_device_t **rawDevices, int *numDevices);

    void *libmtpHandle_ = nullptr;
    MtpCheckIsMtpDeviceFunc checkIsMtpDeviceFunc_ = nullptr;
    MtpDetectRawDevicesFunc detectRawDevicesFunc_ = nullptr;
};
} // namespace StorageDaemon
} // namespace OHOS

#endif // OHOS_STORAGE_DAEMON_MTP_LIB_LOADER_H