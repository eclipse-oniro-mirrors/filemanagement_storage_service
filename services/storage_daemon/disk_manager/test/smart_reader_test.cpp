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

#include <cerrno>
#include <cstdarg>
#include <cstring>
#include <deque>

#include <fcntl.h>
#include <linux/hdreg.h>
#include <linux/nvme_ioctl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <securec.h>

// 直接包含实现文件，以便覆盖匿名命名空间中的内部工具函数（Le16ToUint64 / AtaSmartCmd 等）。
#define private public
#include "disk/smart_reader.cpp"
#undef private

namespace OHOS {
namespace StorageDaemon {

namespace {
/* ---- mock 全局量，供 --wrap= 符号读取与控制 ---- */
bool g_openFail = false;
int g_ioctlDefaultRet = 0;
std::deque<int> g_ioctlRetQueue;
errno_t g_memcpySysRet = EOK;
bool g_emulateAtaSmart = false;
uint8_t g_emulateAttrId = 0;
uint8_t g_emulateRawValue[ATA_RAW_VALUE_SIZE] = {};
} // namespace

extern "C" int __wrap_open(const char *pathname, int flags, ...)
{
    (void)pathname;
    (void)flags;
    return g_openFail ? -1 : 3;
}

extern "C" int __wrap_close(int fd)
{
    (void)fd;
    return 0;
}

extern "C" int __wrap_ioctl(int fd, unsigned long request, ...)
{
    (void)fd;
    int ret = g_ioctlDefaultRet;
    if (!g_ioctlRetQueue.empty()) {
        ret = g_ioctlRetQueue.front();
        g_ioctlRetQueue.pop_front();
    }
    if (ret < 0) {
        return -1;
    }
    if (request == HDIO_DRIVE_CMD && g_emulateAtaSmart) {
        va_list ap;
        va_start(ap, request);
        unsigned char *cdb = va_arg(ap, unsigned char *);
        va_end(ap);
        if (cdb != nullptr && cdb[ATA_CDB_FEATURE_INDEX] == ATA_SMART_READ_VALUES) {
            AtaSmartValues *values = reinterpret_cast<AtaSmartValues *>(cdb + ATA_CDB_HEADER_SIZE);
            std::memset(values, 0, sizeof(*values));
            values->vendorAttributes[0].id = g_emulateAttrId;
            std::memcpy(values->vendorAttributes[0].rawValue, g_emulateRawValue, ATA_RAW_VALUE_SIZE);
        }
    }
    return 0;
}

extern "C" errno_t __wrap_memcpy_s(void *dest, size_t destMax, const void *src, size_t count)
{
    if (g_memcpySysRet != EOK) {
        return g_memcpySysRet;
    }
    if (dest == nullptr || src == nullptr) {
        return EINVAL;
    }
    if (count > destMax) {
        return EOVERFLOW;
    }
    std::memcpy(dest, src, count);
    return EOK;
}

using namespace testing;
using namespace testing::ext;

class SmartReaderTest : public testing::Test {
public:
    void SetUp() override
    {
        g_openFail = false;
        g_ioctlDefaultRet = 0;
        g_ioctlRetQueue.clear();
        g_memcpySysRet = EOK;
        g_emulateAtaSmart = false;
        g_emulateAttrId = 0;
        std::memset(g_emulateRawValue, 0, sizeof(g_emulateRawValue));
    }
};

/* ---------------- 内部工具函数 ---------------- */

/**
 * @tc.name: Le16ToUint64_Basic
 * @tc.desc: 16 字节小端缓冲区取低 8 字节，正确还原 uint64。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, Le16ToUint64_Basic, TestSize.Level1)
{
    uint8_t data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(Le16ToUint64(data), 0x0807060504030201ULL);
}

/**
 * @tc.name: CombineRawValue_Basic
 * @tc.desc: 6 字节 raw value 按指定字节序组合为 uint64。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, CombineRawValue_Basic, TestSize.Level1)
{
    uint8_t raw[ATA_RAW_VALUE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
    EXPECT_EQ(CombineRawValue(raw), 0xBC009A78563412ULL);
}

/**
 * @tc.name: NvmeGetSmartLogCmd_SuccessAndFail
 * @tc.desc: NVMe Get Log Page 直接透传 ioctl 结果。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, NvmeGetSmartLogCmd_SuccessAndFail, TestSize.Level1)
{
    NvmeSmartLog log = {};
    g_ioctlDefaultRet = 0;
    EXPECT_EQ(NvmeGetSmartLogCmd(3, log), 0);

    g_ioctlDefaultRet = -1;
    EXPECT_EQ(NvmeGetSmartLogCmd(3, log), -1);
}

/**
 * @tc.name: AtaSmartCmd_Branches
 * @tc.desc: 覆盖 ioctl 失败、outData 为空、memcpy_s 失败、成功四条路径。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, AtaSmartCmd_Branches, TestSize.Level1)
{
    uint8_t data[ATA_CDB_DATA_SIZE] = {};

    g_ioctlDefaultRet = -1;
    EXPECT_LT(AtaSmartCmd(3, ATA_SMART_READ_VALUES, ATA_SMART_READ_SECTOR_COUNT, data), 0);

    g_ioctlDefaultRet = 0;
    EXPECT_EQ(AtaSmartCmd(3, ATA_SMART_READ_VALUES, ATA_SMART_READ_SECTOR_COUNT, nullptr), 0);

    g_memcpySysRet = EOVERFLOW;
    EXPECT_NE(AtaSmartCmd(3, ATA_SMART_READ_VALUES, ATA_SMART_READ_SECTOR_COUNT, data), 0);

    g_memcpySysRet = EOK;
    EXPECT_EQ(AtaSmartCmd(3, ATA_SMART_READ_VALUES, ATA_SMART_READ_SECTOR_COUNT, data), 0);
}

/**
 * @tc.name: IsSataSmartSupported_Branches
 * @tc.desc: SATA SMART 支持检测的 ioctl 成功/失败分支。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, IsSataSmartSupported_Branches, TestSize.Level1)
{
    g_ioctlDefaultRet = 0;
    EXPECT_TRUE(IsSataSmartSupported(3));

    g_ioctlDefaultRet = -1;
    EXPECT_FALSE(IsSataSmartSupported(3));
}

/**
 * @tc.name: EnableSataSmart_Branches
 * @tc.desc: SATA SMART 使能的 ioctl 成功/失败分支。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, EnableSataSmart_Branches, TestSize.Level1)
{
    g_ioctlDefaultRet = 0;
    EXPECT_TRUE(EnableSataSmart(3));

    g_ioctlDefaultRet = -1;
    EXPECT_FALSE(EnableSataSmart(3));
}

/**
 * @tc.name: ReadSataSmartAttrById_NotFound
 * @tc.desc: SMART 可用但目标属性不存在时返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, ReadSataSmartAttrById_NotFound, TestSize.Level1)
{
    g_ioctlRetQueue = {0, 0, 0};
    EXPECT_EQ(ReadSataSmartAttrById(3, ATA_ATTR_POWER_CYCLE_COUNT), 0U);
}

/**
 * @tc.name: ReadSataSmartAttrById_Found
 * @tc.desc: SMART 可用且属性命中时返回组合后的 raw value。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, ReadSataSmartAttrById_Found, TestSize.Level1)
{
    g_emulateAtaSmart = true;
    g_emulateAttrId = ATA_ATTR_POWER_CYCLE_COUNT;
    uint8_t raw[ATA_RAW_VALUE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
    std::memcpy(g_emulateRawValue, raw, sizeof(raw));
    g_ioctlRetQueue = {0, 0, 0};

    EXPECT_EQ(ReadSataSmartAttrById(3, ATA_ATTR_POWER_CYCLE_COUNT), 0xBC009A78563412ULL);
}

/**
 * @tc.name: ReadSataSmartAttrById_SupportFailed
 * @tc.desc: SMART 不支持，走不支持分支返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, ReadSataSmartAttrById_SupportFailed, TestSize.Level1)
{
    g_ioctlRetQueue = {-1};
    EXPECT_EQ(ReadSataSmartAttrById(3, ATA_ATTR_POWER_CYCLE_COUNT), 0U);
}

/**
 * @tc.name: ReadSataSmartAttrById_EnableFailed
 * @tc.desc: SMART 支持但使能失败，走使能失败分支返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, ReadSataSmartAttrById_EnableFailed, TestSize.Level1)
{
    g_ioctlRetQueue = {0, -1};
    EXPECT_EQ(ReadSataSmartAttrById(3, ATA_ATTR_POWER_CYCLE_COUNT), 0U);
}

/**
 * @tc.name: ReadSataSmartAttrById_ReadFailed
 * @tc.desc: SMART 支持且使能成功，但 READ_VALUES 失败返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, ReadSataSmartAttrById_ReadFailed, TestSize.Level1)
{
    g_ioctlRetQueue = {0, 0, -1};
    EXPECT_EQ(ReadSataSmartAttrById(3, ATA_ATTR_POWER_CYCLE_COUNT), 0U);
}

/* ---------------- SmartReader 公共/私有接口 ---------------- */

/**
 * @tc.name: IsNvmeDevice_Branches
 * @tc.desc: 前缀判断 nvme 与非 nvme 两条分支。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, IsNvmeDevice_Branches, TestSize.Level1)
{
    EXPECT_TRUE(SmartReader::IsNvmeDevice("nvme0n1"));
    EXPECT_FALSE(SmartReader::IsNvmeDevice("sda"));
}

/**
 * @tc.name: GetDiskPowerCycleCount_EmptyName
 * @tc.desc: 设备名为空直接返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerCycleCount_EmptyName, TestSize.Level1)
{
    EXPECT_EQ(SmartReader::GetDiskPowerCycleCount(""), 0U);
}

/**
 * @tc.name: GetDiskPowerCycleCount_OpenFailed
 * @tc.desc: open 失败返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerCycleCount_OpenFailed, TestSize.Level1)
{
    g_openFail = true;
    EXPECT_EQ(SmartReader::GetDiskPowerCycleCount("sda"), 0U);
}

/**
 * @tc.name: GetDiskPowerCycleCount_SataFound
 * @tc.desc: SATA 路径命中 power cycle 属性返回 raw value。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerCycleCount_SataFound, TestSize.Level1)
{
    g_emulateAtaSmart = true;
    g_emulateAttrId = ATA_ATTR_POWER_CYCLE_COUNT;
    uint8_t raw[ATA_RAW_VALUE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
    std::memcpy(g_emulateRawValue, raw, sizeof(raw));

    EXPECT_EQ(SmartReader::GetDiskPowerCycleCount("sda"), 0xBC009A78563412ULL);
}

/**
 * @tc.name: GetDiskPowerCycleCount_NvmeSuccess
 * @tc.desc: NVMe 路径 ioctl 成功，log 全零时返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerCycleCount_NvmeSuccess, TestSize.Level1)
{
    EXPECT_EQ(SmartReader::GetDiskPowerCycleCount("nvme0n1"), 0U);
}

/**
 * @tc.name: GetDiskPowerCycleCount_NvmeFailed
 * @tc.desc: NVMe 路径 ioctl 失败返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerCycleCount_NvmeFailed, TestSize.Level1)
{
    g_ioctlDefaultRet = -1;
    EXPECT_EQ(SmartReader::GetDiskPowerCycleCount("nvme0n1"), 0U);
}

/**
 * @tc.name: GetDiskPowerOnHours_EmptyName
 * @tc.desc: 设备名为空直接返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerOnHours_EmptyName, TestSize.Level1)
{
    EXPECT_EQ(SmartReader::GetDiskPowerOnHours(""), 0U);
}

/**
 * @tc.name: GetDiskPowerOnHours_OpenFailed
 * @tc.desc: open 失败返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerOnHours_OpenFailed, TestSize.Level1)
{
    g_openFail = true;
    EXPECT_EQ(SmartReader::GetDiskPowerOnHours("sda"), 0U);
}

/**
 * @tc.name: GetDiskPowerOnHours_SataFound
 * @tc.desc: SATA 路径命中 power on hours 属性返回 raw value。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerOnHours_SataFound, TestSize.Level1)
{
    g_emulateAtaSmart = true;
    g_emulateAttrId = ATA_ATTR_POWER_ON_HOURS;
    uint8_t raw[ATA_RAW_VALUE_SIZE] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    std::memcpy(g_emulateRawValue, raw, sizeof(raw));

    EXPECT_EQ(SmartReader::GetDiskPowerOnHours("sda"), 0x66005544332211ULL);
}

/**
 * @tc.name: GetDiskPowerOnHours_NvmeSuccess
 * @tc.desc: NVMe 路径 ioctl 成功，log 全零时返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerOnHours_NvmeSuccess, TestSize.Level1)
{
    EXPECT_EQ(SmartReader::GetDiskPowerOnHours("nvme0n1"), 0U);
}

/**
 * @tc.name: GetDiskPowerOnHours_NvmeFailed
 * @tc.desc: NVMe 路径 ioctl 失败返回 0。
 * @tc.type: FUNC
 */
HWTEST_F(SmartReaderTest, GetDiskPowerOnHours_NvmeFailed, TestSize.Level1)
{
    g_ioctlDefaultRet = -1;
    EXPECT_EQ(SmartReader::GetDiskPowerOnHours("nvme0n1"), 0U);
}

} // namespace StorageDaemon
} // namespace OHOS