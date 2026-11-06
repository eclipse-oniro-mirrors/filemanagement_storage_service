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

#include "mtp/usb_event_subscriber.h"

namespace OHOS {
namespace StorageDaemon {
using namespace testing::ext;

class UsbEventSubscriberTest : public testing::Test {};

/**
 * @tc.name: IsPossibleMtpDeviceClass_Whitelist_001
 * @tc.desc: Verify whitelisted device classes return true.
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventSubscriberTest, IsPossibleMtpDeviceClass_Whitelist_001, TestSize.Level1)
{
    EXPECT_TRUE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x00));  // per-interface
    EXPECT_TRUE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x02));  // COMM
    EXPECT_TRUE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x06));  // PTP / Still Image
    EXPECT_TRUE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0xEF));  // IAD
    EXPECT_TRUE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0xFF));  // vendor-specific
}

/**
 * @tc.name: IsPossibleMtpDeviceClass_NonWhitelist_001
 * @tc.desc: Verify non-MTP device classes return false.
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventSubscriberTest, IsPossibleMtpDeviceClass_NonWhitelist_001, TestSize.Level1)
{
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x01));  // audio
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x03));  // HID / keyboard
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x05));  // physical
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x07));  // printer
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x08));  // mass storage
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x09));  // hub
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0x0B));  // smart card
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0xDC));  // diagnostic
    EXPECT_FALSE(UsbEventSubscriber::IsPossibleMtpDeviceClass(0xFE));  // application specific
}

/**
 * @tc.name: ShouldHandleMtpDevice_NonWhitelist_001
 * @tc.desc: No string/interface hit + non-whitelist class must be filtered out before dlopen.
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventSubscriberTest, ShouldHandleMtpDevice_NonWhitelist_001, TestSize.Level1)
{
    DeviceType deviceType = DeviceType::CAMERA;
    const std::string usbInfo =
        "{\"productName\":\"USB Keyboard\",\"clazz\":3,\"vendorId\":1111,\"productId\":2222}";
    bool result = UsbEventSubscriber::ShouldHandleMtpDevice(usbInfo, deviceType);
    EXPECT_FALSE(result);
    // deviceType untouched means GetDeviceType (and dlopen) was not triggered.
    EXPECT_EQ(deviceType, DeviceType::CAMERA);
}

/**
 * @tc.name: ShouldHandleMtpDevice_Whitelist_001
 * @tc.desc: No string/interface hit but whitelisted class goes on to GetDeviceType.
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventSubscriberTest, ShouldHandleMtpDevice_Whitelist_001, TestSize.Level1)
{
    DeviceType deviceType = DeviceType::CAMERA;
    const std::string usbInfo =
        "{\"productName\":\"Some Device\",\"clazz\":6,\"vendorId\":1111,\"productId\":2222}";
    bool result = UsbEventSubscriber::ShouldHandleMtpDevice(usbInfo, deviceType);
    EXPECT_FALSE(result);
    // libmtp unavailable in UT env, GetDeviceType downgrades to UNKNOWN.
    EXPECT_EQ(deviceType, DeviceType::UNKNOWN);
}
} // namespace StorageDaemon
} // namespace OHOS