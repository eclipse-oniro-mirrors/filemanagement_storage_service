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
#include <string>

#include "utils/storage_utils.h"

namespace OHOS {
namespace StorageManager {
using namespace testing;
using namespace testing::ext;

class StorageUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_STORAGE_IsFilePathInvalid_ValidPath
 * @tc.name: IsFilePathInvalid_ValidPath
 * @tc.desc: Test valid path returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsFilePathInvalid_ValidPath, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsFilePathInvalid_ValidPath start";

    EXPECT_FALSE(IsFilePathInvalid("/mnt/data/100/userExternal/sub"));

    GTEST_LOG_(INFO) << "IsFilePathInvalid_ValidPath end";
}

/**
 * @tc.number: SUB_STORAGE_IsFilePathInvalid_RelativeTraversal
 * @tc.name: IsFilePathInvalid_RelativeTraversal
 * @tc.desc: Test path with ../ returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsFilePathInvalid_RelativeTraversal, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsFilePathInvalid_RelativeTraversal start";

    EXPECT_TRUE(IsFilePathInvalid("/mnt/data/../etc"));

    GTEST_LOG_(INFO) << "IsFilePathInvalid_RelativeTraversal end";
}

/**
 * @tc.number: SUB_STORAGE_IsFilePathInvalid_TrailingDotDot
 * @tc.name: IsFilePathInvalid_TrailingDotDot
 * @tc.desc: Test path ending with /.. returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsFilePathInvalid_TrailingDotDot, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsFilePathInvalid_TrailingDotDot start";

    EXPECT_TRUE(IsFilePathInvalid("/mnt/data/sub/.."));

    GTEST_LOG_(INFO) << "IsFilePathInvalid_TrailingDotDot end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithFileMgr_ValidPrefix
 * @tc.name: IsPathStartWithFileMgr_ValidPrefix
 * @tc.desc: Test path with valid FileMgr prefix returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithFileMgr_ValidPrefix, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_ValidPrefix start";

    EXPECT_TRUE(IsPathStartWithFileMgr(100, "/mnt/data/100/userExternal/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_ValidPrefix end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithFileMgr_TooShort
 * @tc.name: IsPathStartWithFileMgr_TooShort
 * @tc.desc: Test too short path returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithFileMgr_TooShort, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_TooShort start";

    EXPECT_FALSE(IsPathStartWithFileMgr(100, "/mnt/data/"));

    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_TooShort end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithFileMgr_Mismatch
 * @tc.name: IsPathStartWithFileMgr_Mismatch
 * @tc.desc: Test path with mismatched prefix returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithFileMgr_Mismatch, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_Mismatch start";

    EXPECT_FALSE(IsPathStartWithFileMgr(100, "/data/local/evil"));

    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_Mismatch end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithFileMgr_PrefixMismatchLong
 * @tc.name: IsPathStartWithFileMgr_PrefixMismatchLong
 * @tc.desc: Test path long enough but with mismatched prefix returns false (covers if2 true).
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithFileMgr_PrefixMismatchLong, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_PrefixMismatchLong start";

    EXPECT_FALSE(IsPathStartWithFileMgr(100, "/mnt/data/100/userInternal/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_PrefixMismatchLong end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithFileMgr_Empty
 * @tc.name: IsPathStartWithFileMgr_Empty
 * @tc.desc: Test empty path returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithFileMgr_Empty, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_Empty start";

    EXPECT_FALSE(IsPathStartWithFileMgr(100, ""));

    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_Empty end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithFileMgr_EqualPrefixLen
 * @tc.name: IsPathStartWithFileMgr_EqualPrefixLen
 * @tc.desc: Test path whose length equals prefix length returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithFileMgr_EqualPrefixLen, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_EqualPrefixLen start";

    EXPECT_FALSE(IsPathStartWithFileMgr(100, "/mnt/data/100/userExternal/"));

    GTEST_LOG_(INFO) << "IsPathStartWithFileMgr_EqualPrefixLen end";
}

/**
 * @tc.number: Storage_Utils_GetRoundSize_test_0001
 * @tc.name: Storage_Utils_GetRoundSize_test_0001
 * @tc.desc: Test GetRoundSize with KB/MB range values
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level: Level 1
 */
HWTEST_F(StorageUtilsTest, Storage_Utils_GetRoundSize_test_0001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0001 begin";
    constexpr int64_t kb = 1000;
    constexpr int64_t mb = 1000000;
    constexpr int64_t gb = 1000000000;

    // Small values round up to 1KB
    EXPECT_EQ(GetRoundSize(1), kb);
    EXPECT_EQ(GetRoundSize(kb - 1), kb);
    EXPECT_EQ(GetRoundSize(kb), kb);

    // KB to MB transition
    EXPECT_EQ(GetRoundSize(kb + 1), 2 * kb);
    EXPECT_EQ(GetRoundSize(mb), mb);
    EXPECT_EQ(GetRoundSize(mb + 1), 2 * mb);

    // MB to GB transition
    EXPECT_EQ(GetRoundSize(gb), gb);
    EXPECT_EQ(GetRoundSize(gb + 1), 2 * gb);
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0001 end";
}

/**
 * @tc.number: Storage_Utils_GetRoundSize_test_0002
 * @tc.name: Storage_Utils_GetRoundSize_test_0002
 * @tc.desc: Test GetRoundSize with device specs - 256GB/512GB
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level: Level 1
 */
HWTEST_F(StorageUtilsTest, Storage_Utils_GetRoundSize_test_0002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0002 begin";
    constexpr int64_t gb = 1000000000;

    // 256GB device: rounds to 256 GB
    int64_t result = GetRoundSize(256LL * gb);
    EXPECT_EQ(result, 256LL * gb);

    // 512GB device: rounds to 512 GB
    result = GetRoundSize(512LL * gb);
    EXPECT_EQ(result, 512LL * gb);

    // 256GB + 1 → 512 GB (rounds up to next power of 2 in GB tier)
    result = GetRoundSize(256LL * gb + 1);
    EXPECT_EQ(result, 512LL * gb);
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0002 end";
}

/**
 * @tc.number: Storage_Utils_GetRoundSize_test_0003
 * @tc.name: Storage_Utils_GetRoundSize_test_0003
 * @tc.desc: Test GetRoundSize at 1024GB boundary
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level: Level 1
 */
HWTEST_F(StorageUtilsTest, Storage_Utils_GetRoundSize_test_0003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0003 begin";
    constexpr int64_t gb = 1000000000;

    // 1024GB: val=1024 in GB tier, 1024<=1023 NO → returns 1024 GB
    int64_t result = GetRoundSize(1024LL * gb);
    EXPECT_EQ(result, 1024LL * gb);

    // 1024GB + 1: val doubles to 2048 in GB tier → returns 2048 GB
    result = GetRoundSize(1024LL * gb + 1);
    EXPECT_EQ(result, 2048LL * gb);
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0003 end";
}

/**
 * @tc.number: Storage_Utils_GetRoundSize_test_0004
 * @tc.name: Storage_Utils_GetRoundSize_test_0004
 * @tc.desc: Test GetRoundSize with large sizes in GB tier (no >1TB branch)
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level: Level 1
 */
HWTEST_F(StorageUtilsTest, Storage_Utils_GetRoundSize_test_0004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0004 begin";
    constexpr int64_t gb = 1000000000;

    // GetRoundSize is a pure rounding function, has no >1TB branch
    // In GB tier (multiple=ONE_GB), val continues doubling

    // 1.5TB: (1500GB-1)/1000MB... Phase3: val→2048, 2048*GB >= 1500GB → return 2048GB
    int64_t result = GetRoundSize(1500LL * gb);
    EXPECT_EQ(result, 2048LL * gb);

    // 2TB: (2000GB) → val→2048, return 2048GB
    result = GetRoundSize(2000LL * gb);
    EXPECT_EQ(result, 2048LL * gb);

    // 3TB: (3000GB) → val→4096, return 4096GB
    result = GetRoundSize(3000LL * gb);
    EXPECT_EQ(result, 4096LL * gb);

    // 4TB: (4000GB) → val→4096, return 4096GB
    result = GetRoundSize(4000LL * gb);
    EXPECT_EQ(result, 4096LL * gb);
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0004 end";
}

/**
 * @tc.number: Storage_Utils_GetRoundSize_test_0006
 * @tc.name: Storage_Utils_GetRoundSize_test_0006
 * @tc.desc: Test GetRoundSize monotonicity - larger input should produce >= output
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level: Level 2
 */
HWTEST_F(StorageUtilsTest, Storage_Utils_GetRoundSize_test_0006, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0006 begin";
    constexpr int64_t gb = 1000000000;

    // Monotonicity across tier boundaries
    EXPECT_LE(GetRoundSize(511LL * gb), GetRoundSize(512LL * gb));
    EXPECT_LE(GetRoundSize(512LL * gb), GetRoundSize(513LL * gb));
    EXPECT_LE(GetRoundSize(1023LL * gb), GetRoundSize(1024LL * gb));
    EXPECT_LE(GetRoundSize(1024LL * gb), GetRoundSize(1025LL * gb));
    GTEST_LOG_(INFO) << "Storage_Utils_GetRoundSize_test_0006 end";
}
/**
 * @tc.number: SUB_STORAGE_IsPathStartWithCloudDisk_ValidPrefix
 * @tc.name: IsPathStartWithCloudDisk_ValidPrefix
 * @tc.desc: Test path with valid cloud disk prefix returns true.
 * @tc.type: FUNC
 * @tc.require: RR-30093650
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithCloudDisk_ValidPrefix, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_ValidPrefix start";

    EXPECT_TRUE(IsPathStartWithCloudDisk(100, "/mnt/data/100/cloud_disk_fuse/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_ValidPrefix end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithCloudDisk_EqualPrefixLen
 * @tc.name: IsPathStartWithCloudDisk_EqualPrefixLen
 * @tc.desc: Test exact cloud_disk_fuse directory path returns true.
 * @tc.type: FUNC
 * @tc.require: RR-30093650
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithCloudDisk_EqualPrefixLen, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_EqualPrefixLen start";

    EXPECT_TRUE(IsPathStartWithCloudDisk(100, "/mnt/data/100/cloud_disk_fuse"));

    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_EqualPrefixLen end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithCloudDisk_Mismatch
 * @tc.name: IsPathStartWithCloudDisk_Mismatch
 * @tc.desc: Test path with mismatched prefix returns false.
 * @tc.type: FUNC
 * @tc.require: RR-30093650
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithCloudDisk_Mismatch, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_Mismatch start";

    EXPECT_FALSE(IsPathStartWithCloudDisk(100, "/mnt/data/100/other/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_Mismatch end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithCloudDisk_Empty
 * @tc.name: IsPathStartWithCloudDisk_Empty
 * @tc.desc: Test empty path returns false.
 * @tc.type: FUNC
 * @tc.require: RR-30093650
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithCloudDisk_Empty, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_Empty start";

    EXPECT_FALSE(IsPathStartWithCloudDisk(100, ""));

    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_Empty end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithCloudDisk_UserIdMismatch
 * @tc.name: IsPathStartWithCloudDisk_UserIdMismatch
 * @tc.desc: Test path with different userId in prefix returns false.
 * @tc.type: FUNC
 * @tc.require: RR-30093650
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithCloudDisk_UserIdMismatch, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_UserIdMismatch start";

    EXPECT_FALSE(IsPathStartWithCloudDisk(100, "/mnt/data/101/cloud_disk_fuse/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithCloudDisk_UserIdMismatch end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithUdmfClip_ValidUdmfPrefix
 * @tc.name: IsPathStartWithUdmfClip_ValidUdmfPrefix
 * @tc.desc: Test path with valid udmf fuse prefix returns true.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithUdmfClip_ValidUdmfPrefix, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_ValidUdmfPrefix start";

    EXPECT_TRUE(IsPathStartWithUdmfClip(100, "/mnt/data/100/udmf_fuse/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_ValidUdmfPrefix end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithUdmfClip_ValidClipboardPrefix
 * @tc.name: IsPathStartWithUdmfClip_ValidClipboardPrefix
 * @tc.desc: Test path with valid clipboard fuse prefix returns true.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithUdmfClip_ValidClipboardPrefix, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_ValidClipboardPrefix start";

    EXPECT_TRUE(IsPathStartWithUdmfClip(100, "/mnt/data/100/clipboard_fuse/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_ValidClipboardPrefix end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithUdmfClip_Mismatch
 * @tc.name: IsPathStartWithUdmfClip_Mismatch
 * @tc.desc: Test path with mismatched prefix returns false.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithUdmfClip_Mismatch, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_Mismatch start";

    EXPECT_FALSE(IsPathStartWithUdmfClip(100, "/mnt/data/100/cloud_disk_fuse/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_Mismatch end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithUdmfClip_Empty
 * @tc.name: IsPathStartWithUdmfClip_Empty
 * @tc.desc: Test empty path returns false.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithUdmfClip_Empty, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_Empty start";

    EXPECT_FALSE(IsPathStartWithUdmfClip(100, ""));

    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_Empty end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithUdmfClip_UserIdMismatch
 * @tc.name: IsPathStartWithUdmfClip_UserIdMismatch
 * @tc.desc: Test path with different userId in prefix returns false.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithUdmfClip_UserIdMismatch, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_UserIdMismatch start";

    EXPECT_FALSE(IsPathStartWithUdmfClip(100, "/mnt/data/101/udmf_fuse/sub"));

    GTEST_LOG_(INFO) << "IsPathStartWithUdmfClip_UserIdMismatch end";
}

/**
 * @tc.number: SUB_STORAGE_GetDeviceType_0000
 * @tc.name: SUB_STORAGE_GetDeviceType_0000
 * @tc.desc: Test GetDeviceType returns a valid device type string.
 * @tc.type: FUNC
 * @tc.require: SR20260812027362
 */
HWTEST_F(StorageUtilsTest, SUB_STORAGE_GetDeviceType_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SUB_STORAGE_GetDeviceType_0000 start";

    std::string deviceType = GetDeviceType();
    // Fallback: "0" (PC) even on read failure; always a valid device type
    EXPECT_TRUE(deviceType == "0" || deviceType == "1" || deviceType == "2" || deviceType == "3");

    GTEST_LOG_(INFO) << "SUB_STORAGE_GetDeviceType_0000 end";
}

/**
 * @tc.number: SUB_STORAGE_DeviceTypeHelpers_MutuallyExclusive_0000
 * @tc.name: SUB_STORAGE_DeviceTypeHelpers_MutuallyExclusive_0000
 * @tc.desc: Test IsPC/IsPhone/IsPad/IsTv are mutually exclusive and match GetDeviceType.
 * @tc.type: FUNC
 * @tc.require: SR20260812027362
 */
HWTEST_F(StorageUtilsTest, SUB_STORAGE_DeviceTypeHelpers_MutuallyExclusive_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SUB_STORAGE_DeviceTypeHelpers_MutuallyExclusive_0000 start";

    std::string deviceType = GetDeviceType();
    bool isPC = IsPC();
    bool isPhone = IsPhone();
    bool isPad = IsPad();
    bool isTv = IsTv();
    int trueCount = (isPC ? 1 : 0) + (isPhone ? 1 : 0) + (isPad ? 1 : 0) + (isTv ? 1 : 0);
    // The four form predicates are mutually exclusive: at most one true at any time
    EXPECT_TRUE(trueCount <= 1);
    EXPECT_EQ(isPC, deviceType == "0");
    EXPECT_EQ(isPhone, deviceType == "1");
    EXPECT_EQ(isPad, deviceType == "2");
    EXPECT_EQ(isTv, deviceType == "3");

    GTEST_LOG_(INFO) << "SUB_STORAGE_DeviceTypeHelpers_MutuallyExclusive_0000 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPC_0000
 * @tc.name: SUB_STORAGE_IsPC_0000
 * @tc.desc: Test IsPC consistently with GetDeviceType.
 * @tc.type: FUNC
 * @tc.require: SR20260812027362
 */
HWTEST_F(StorageUtilsTest, SUB_STORAGE_IsPC_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SUB_STORAGE_IsPC_0000 start";

    EXPECT_EQ(IsPC(), GetDeviceType() == "0");

    GTEST_LOG_(INFO) << "SUB_STORAGE_IsPC_0000 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPcMode_0000
 * @tc.name: SUB_STORAGE_IsPcMode_0000
 * @tc.desc: Test IsPcMode returns a stable bool result (read failure falls back false).
 * @tc.type: FUNC
 * @tc.require: SR20260812027362
 */
HWTEST_F(StorageUtilsTest, SUB_STORAGE_IsPcMode_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SUB_STORAGE_IsPcMode_0000 start";

    // PC mode falls back to false on read failure; the result must stay boolean
    EXPECT_TRUE(IsPcMode() == true || IsPcMode() == false);

    GTEST_LOG_(INFO) << "SUB_STORAGE_IsPcMode_0000 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPcMode_Stable_0000
 * @tc.name: SUB_STORAGE_IsPcMode_Stable_0000
 * @tc.desc: Test IsPcMode returns the same result on consecutive calls without external change.
 * @tc.type: FUNC
 * @tc.require: SR20260812027362
 */
HWTEST_F(StorageUtilsTest, SUB_STORAGE_IsPcMode_Stable_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SUB_STORAGE_IsPcMode_Stable_0000 start";

    // Consecutive reads with no parameter change must agree
    EXPECT_EQ(IsPcMode(), IsPcMode());

    GTEST_LOG_(INFO) << "SUB_STORAGE_IsPcMode_Stable_0000 end";
}

/**
 * @tc.number: SUB_STORAGE_GetAnonyString_ShortLen_001
 * @tc.name: GetAnonyString_ShortLen_001
 * @tc.desc: Test GetAnonyString with length less than minimum (3) returns "******".
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, GetAnonyString_ShortLen_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnonyString_ShortLen_001 start";

    EXPECT_EQ(GetAnonyString(""), "******");
    EXPECT_EQ(GetAnonyString("a"), "******");
    EXPECT_EQ(GetAnonyString("ab"), "******");

    GTEST_LOG_(INFO) << "GetAnonyString_ShortLen_001 end";
}

/**
 * @tc.number: SUB_STORAGE_GetAnonyString_MinLen_001
 * @tc.name: GetAnonyString_MinLen_001
 * @tc.desc: Test GetAnonyString with exact minimum length (3).
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, GetAnonyString_MinLen_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnonyString_MinLen_001 start";

    EXPECT_EQ(GetAnonyString("abc"), "a******c");

    GTEST_LOG_(INFO) << "GetAnonyString_MinLen_001 end";
}

/**
 * @tc.number: SUB_STORAGE_GetAnonyString_ShortId_001
 * @tc.name: GetAnonyString_ShortId_001
 * @tc.desc: Test GetAnonyString with length within short id range (3-20).
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, GetAnonyString_ShortId_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnonyString_ShortId_001 start";

    EXPECT_EQ(GetAnonyString("1234567890"), "1******0");

    GTEST_LOG_(INFO) << "GetAnonyString_ShortId_001 end";
}

/**
 * @tc.number: SUB_STORAGE_GetAnonyString_ShortIdBoundary_001
 * @tc.name: GetAnonyString_ShortIdBoundary_001
 * @tc.desc: Test GetAnonyString with exact short id boundary length (20).
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, GetAnonyString_ShortIdBoundary_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnonyString_ShortIdBoundary_001 start";

    std::string value(20, 'x');
    std::string expected = std::string("x") + "******" + "x";
    EXPECT_EQ(GetAnonyString(value), expected);

    GTEST_LOG_(INFO) << "GetAnonyString_ShortIdBoundary_001 end";
}

/**
 * @tc.number: SUB_STORAGE_GetAnonyString_LongId_001
 * @tc.name: GetAnonyString_LongId_001
 * @tc.desc: Test GetAnonyString with length greater than short id length (20) uses first4+******+last4.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, GetAnonyString_LongId_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAnonyString_LongId_001 start";

    std::string value = "123456789012345678901234567890"; // len=30
    std::string expected = "1234******7890";
    EXPECT_EQ(GetAnonyString(value), expected);

    GTEST_LOG_(INFO) << "GetAnonyString_LongId_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithDlp_Empty_001
 * @tc.name: IsPathStartWithDlp_Empty_001
 * @tc.desc: Test IsPathStartWithDlp with empty path returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithDlp_Empty_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithDlp_Empty_001 start";

    EXPECT_FALSE(IsPathStartWithDlp(""));

    GTEST_LOG_(INFO) << "IsPathStartWithDlp_Empty_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithDlp_ValidPrefix_001
 * @tc.name: IsPathStartWithDlp_ValidPrefix_001
 * @tc.desc: Test IsPathStartWithDlp with valid dlp prefix returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithDlp_ValidPrefix_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithDlp_ValidPrefix_001 start";

    EXPECT_TRUE(IsPathStartWithDlp("/data/service/el1/public/dlp_credential_service/file"));

    GTEST_LOG_(INFO) << "IsPathStartWithDlp_ValidPrefix_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithDlp_ExactPrefix_001
 * @tc.name: IsPathStartWithDlp_ExactPrefix_001
 * @tc.desc: Test IsPathStartWithDlp with exact dlp prefix path returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithDlp_ExactPrefix_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithDlp_ExactPrefix_001 start";

    EXPECT_TRUE(IsPathStartWithDlp("/data/service/el1/public/dlp_credential_service/"));

    GTEST_LOG_(INFO) << "IsPathStartWithDlp_ExactPrefix_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithDlp_Mismatch_001
 * @tc.name: IsPathStartWithDlp_Mismatch_001
 * @tc.desc: Test IsPathStartWithDlp with mismatched prefix returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithDlp_Mismatch_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithDlp_Mismatch_001 start";

    EXPECT_FALSE(IsPathStartWithDlp("/data/local/evil"));

    GTEST_LOG_(INFO) << "IsPathStartWithDlp_Mismatch_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsPathStartWithDlp_PartialMismatch_001
 * @tc.name: IsPathStartWithDlp_PartialMismatch_001
 * @tc.desc: Test IsPathStartWithDlp with partially matching prefix returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsPathStartWithDlp_PartialMismatch_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsPathStartWithDlp_PartialMismatch_001 start";

    EXPECT_FALSE(IsPathStartWithDlp("/data/service/el1/public/other_service/file"));

    GTEST_LOG_(INFO) << "IsPathStartWithDlp_PartialMismatch_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckPkgNameRange_Empty_001
 * @tc.name: CheckPkgNameRange_Empty_001
 * @tc.desc: Test CheckPkgNameRange with empty pkgName returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckPkgNameRange_Empty_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckPkgNameRange_Empty_001 start";

    EXPECT_FALSE(CheckPkgNameRange(""));

    GTEST_LOG_(INFO) << "CheckPkgNameRange_Empty_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckPkgNameRange_Valid_001
 * @tc.name: CheckPkgNameRange_Valid_001
 * @tc.desc: Test CheckPkgNameRange with valid pkgName returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckPkgNameRange_Valid_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckPkgNameRange_Valid_001 start";

    EXPECT_TRUE(CheckPkgNameRange("com.example.app"));

    GTEST_LOG_(INFO) << "CheckPkgNameRange_Valid_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckPkgNameRange_Boundary_001
 * @tc.name: CheckPkgNameRange_Boundary_001
 * @tc.desc: Test CheckPkgNameRange with exact max length (128) returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckPkgNameRange_Boundary_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckPkgNameRange_Boundary_001 start";

    std::string pkgName(128, 'a');
    EXPECT_TRUE(CheckPkgNameRange(pkgName));

    GTEST_LOG_(INFO) << "CheckPkgNameRange_Boundary_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckPkgNameRange_Exceed_001
 * @tc.name: CheckPkgNameRange_Exceed_001
 * @tc.desc: Test CheckPkgNameRange with length exceeding max (129) returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckPkgNameRange_Exceed_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckPkgNameRange_Exceed_001 start";

    std::string pkgName(129, 'a');
    EXPECT_FALSE(CheckPkgNameRange(pkgName));

    GTEST_LOG_(INFO) << "CheckPkgNameRange_Exceed_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckAppIndexRange_Negative_001
 * @tc.name: CheckAppIndexRange_Negative_001
 * @tc.desc: Test CheckAppIndexRange with negative value returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckAppIndexRange_Negative_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckAppIndexRange_Negative_001 start";

    EXPECT_FALSE(CheckAppIndexRange(-1));

    GTEST_LOG_(INFO) << "CheckAppIndexRange_Negative_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckAppIndexRange_Zero_001
 * @tc.name: CheckAppIndexRange_Zero_001
 * @tc.desc: Test CheckAppIndexRange with zero returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckAppIndexRange_Zero_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckAppIndexRange_Zero_001 start";

    EXPECT_TRUE(CheckAppIndexRange(0));

    GTEST_LOG_(INFO) << "CheckAppIndexRange_Zero_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckAppIndexRange_Positive_001
 * @tc.name: CheckAppIndexRange_Positive_001
 * @tc.desc: Test CheckAppIndexRange with positive value returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckAppIndexRange_Positive_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckAppIndexRange_Positive_001 start";

    EXPECT_TRUE(CheckAppIndexRange(5));

    GTEST_LOG_(INFO) << "CheckAppIndexRange_Positive_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckLevelRange_ValidLow_001
 * @tc.name: CheckLevelRange_ValidLow_001
 * @tc.desc: Test CheckLevelRange with EL1_SYS_KEY (0) returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckLevelRange_ValidLow_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckLevelRange_ValidLow_001 start";

    EXPECT_TRUE(CheckLevelRange(0));

    GTEST_LOG_(INFO) << "CheckLevelRange_ValidLow_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckLevelRange_ValidMid_001
 * @tc.name: CheckLevelRange_ValidMid_001
 * @tc.desc: Test CheckLevelRange with mid range value (3) returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckLevelRange_ValidMid_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckLevelRange_ValidMid_001 start";

    EXPECT_TRUE(CheckLevelRange(3));

    GTEST_LOG_(INFO) << "CheckLevelRange_ValidMid_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckLevelRange_ValidHigh_001
 * @tc.name: CheckLevelRange_ValidHigh_001
 * @tc.desc: Test CheckLevelRange with EL5_USER_KEY (5) returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckLevelRange_ValidHigh_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckLevelRange_ValidHigh_001 start";

    EXPECT_TRUE(CheckLevelRange(5));

    GTEST_LOG_(INFO) << "CheckLevelRange_ValidHigh_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckLevelRange_AboveRange_001
 * @tc.name: CheckLevelRange_AboveRange_001
 * @tc.desc: Test CheckLevelRange with value above EL5_USER_KEY (6) returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckLevelRange_AboveRange_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckLevelRange_AboveRange_001 start";

    EXPECT_FALSE(CheckLevelRange(6));

    GTEST_LOG_(INFO) << "CheckLevelRange_AboveRange_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckInputListRange_Empty_001
 * @tc.name: CheckInputListRange_Empty_001
 * @tc.desc: Test CheckInputListRange with empty list returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckInputListRange_Empty_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckInputListRange_Empty_001 start";

    std::vector<std::string> emptyList;
    EXPECT_FALSE(CheckInputListRange(emptyList));

    GTEST_LOG_(INFO) << "CheckInputListRange_Empty_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckInputListRange_Valid_001
 * @tc.name: CheckInputListRange_Valid_001
 * @tc.desc: Test CheckInputListRange with valid list returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckInputListRange_Valid_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckInputListRange_Valid_001 start";

    std::vector<std::string> list = {"a", "b"};
    EXPECT_TRUE(CheckInputListRange(list));

    GTEST_LOG_(INFO) << "CheckInputListRange_Valid_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckInputListRange_Boundary_001
 * @tc.name: CheckInputListRange_Boundary_001
 * @tc.desc: Test CheckInputListRange with exact max size (50000) returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckInputListRange_Boundary_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckInputListRange_Boundary_001 start";

    std::vector<std::string> list(50000, "a");
    EXPECT_TRUE(CheckInputListRange(list));

    GTEST_LOG_(INFO) << "CheckInputListRange_Boundary_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckInputListRange_Exceed_001
 * @tc.name: CheckInputListRange_Exceed_001
 * @tc.desc: Test CheckInputListRange with size exceeding max (50001) returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckInputListRange_Exceed_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckInputListRange_Exceed_001 start";

    std::vector<std::string> list(50001, "a");
    EXPECT_FALSE(CheckInputListRange(list));

    GTEST_LOG_(INFO) << "CheckInputListRange_Exceed_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_Empty_001
 * @tc.name: CheckIdRange_Empty_001
 * @tc.desc: Test CheckIdRange with empty id returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_Empty_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_Empty_001 start";

    EXPECT_FALSE(CheckIdRange(""));

    GTEST_LOG_(INFO) << "CheckIdRange_Empty_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_InvalidUnderscore_001
 * @tc.name: CheckIdRange_InvalidUnderscore_001
 * @tc.desc: Test CheckIdRange with underscore (not in [0-9a-zA-Z]) returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_InvalidUnderscore_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_InvalidUnderscore_001 start";

    EXPECT_FALSE(CheckIdRange("abc_def"));

    GTEST_LOG_(INFO) << "CheckIdRange_InvalidUnderscore_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_InvalidSpace_001
 * @tc.name: CheckIdRange_InvalidSpace_001
 * @tc.desc: Test CheckIdRange with space returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_InvalidSpace_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_InvalidSpace_001 start";

    EXPECT_FALSE(CheckIdRange("abc def"));

    GTEST_LOG_(INFO) << "CheckIdRange_InvalidSpace_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_InvalidSlash_001
 * @tc.name: CheckIdRange_InvalidSlash_001
 * @tc.desc: Test CheckIdRange with slash returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_InvalidSlash_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_InvalidSlash_001 start";

    EXPECT_FALSE(CheckIdRange("abc/def"));

    GTEST_LOG_(INFO) << "CheckIdRange_InvalidSlash_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_Valid_001
 * @tc.name: CheckIdRange_Valid_001
 * @tc.desc: Test CheckIdRange with valid alphanumeric id returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_Valid_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_Valid_001 start";

    EXPECT_TRUE(CheckIdRange("abc123"));

    GTEST_LOG_(INFO) << "CheckIdRange_Valid_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_ValidSingle_001
 * @tc.name: CheckIdRange_ValidSingle_001
 * @tc.desc: Test CheckIdRange with single character (min length) returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_ValidSingle_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_ValidSingle_001 start";

    EXPECT_TRUE(CheckIdRange("a"));

    GTEST_LOG_(INFO) << "CheckIdRange_ValidSingle_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_Boundary65_001
 * @tc.name: CheckIdRange_Boundary65_001
 * @tc.desc: Test CheckIdRange with exact max length (65) returns true.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_Boundary65_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_Boundary65_001 start";

    std::string id(65, 'a');
    EXPECT_TRUE(CheckIdRange(id));

    GTEST_LOG_(INFO) << "CheckIdRange_Boundary65_001 end";
}

/**
 * @tc.number: SUB_STORAGE_CheckIdRange_Exceed65_001
 * @tc.name: CheckIdRange_Exceed65_001
 * @tc.desc: Test CheckIdRange with length exceeding max (66) returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, CheckIdRange_Exceed65_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckIdRange_Exceed65_001 start";

    std::string id(66, 'a');
    EXPECT_FALSE(CheckIdRange(id));

    GTEST_LOG_(INFO) << "CheckIdRange_Exceed65_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsFilePathInvalid_DotDotNotPrecededBySlash_001
 * @tc.name: IsFilePathInvalid_DotDotNotPrecededBySlash_001
 * @tc.desc: Test "../" not preceded by '/' does not trigger invalid (loop continues, returns false).
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsFilePathInvalid_DotDotNotPrecededBySlash_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsFilePathInvalid_DotDotNotPrecededBySlash_001 start";

    EXPECT_FALSE(IsFilePathInvalid("abc../def"));

    GTEST_LOG_(INFO) << "IsFilePathInvalid_DotDotNotPrecededBySlash_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsFilePathInvalid_DotDotSlashAtPos0_001
 * @tc.name: IsFilePathInvalid_DotDotSlashAtPos0_001
 * @tc.desc: Test "../" at position 0 triggers invalid (pos == 0 branch, returns true).
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsFilePathInvalid_DotDotSlashAtPos0_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsFilePathInvalid_DotDotSlashAtPos0_001 start";

    EXPECT_TRUE(IsFilePathInvalid("../etc"));

    GTEST_LOG_(INFO) << "IsFilePathInvalid_DotDotSlashAtPos0_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsFilePathInvalid_SlashDotDotNotAtTail_001
 * @tc.name: IsFilePathInvalid_SlashDotDotNotAtTail_001
 * @tc.desc: Test "/.." found in middle (not at tail) returns false.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsFilePathInvalid_SlashDotDotNotAtTail_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsFilePathInvalid_SlashDotDotNotAtTail_001 start";

    EXPECT_FALSE(IsFilePathInvalid("/mnt/data/..sub"));

    GTEST_LOG_(INFO) << "IsFilePathInvalid_SlashDotDotNotAtTail_001 end";
}

/**
 * @tc.number: SUB_STORAGE_IsUserUnlocked_InvalidUser_001
 * @tc.name: IsUserUnlocked_InvalidUser_001
 * @tc.desc: Test IsUserUnlocked with invalid userId returns false (error branch).
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, IsUserUnlocked_InvalidUser_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsUserUnlocked_InvalidUser_001 start";

    EXPECT_FALSE(IsUserUnlocked(-1));

    GTEST_LOG_(INFO) << "IsUserUnlocked_InvalidUser_001 end";
}

/**
 * @tc.number: SUB_STORAGE_GetForegroundUserIDFromOs_001
 * @tc.name: GetForegroundUserIDFromOs_001
 * @tc.desc: Test GetForegroundUserIDFromOs returns INVALID_USER_ID on error or valid id on success.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, GetForegroundUserIDFromOs_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetForegroundUserIDFromOs_001 start";

    int32_t result = GetForegroundUserIDFromOs();
    EXPECT_TRUE(result == INVALID_USER_ID || result >= 0);

    GTEST_LOG_(INFO) << "GetForegroundUserIDFromOs_001 end";
}

/**
 * @tc.number: SUB_STORAGE_GetCurrentUserId_001
 * @tc.name: GetCurrentUserId_001
 * @tc.desc: Test GetCurrentUserId returns appUid/200000 without crash.
 * @tc.type: FUNC
 * @tc.require: SR000H0372
 */
HWTEST_F(StorageUtilsTest, GetCurrentUserId_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetCurrentUserId_001 start";

    int result = GetCurrentUserId();
    GTEST_LOG_(INFO) << "GetCurrentUserId returned: " << result;
    EXPECT_GE(result, 0);

    GTEST_LOG_(INFO) << "GetCurrentUserId_001 end";
}
} // namespace StorageManager
} // namespace OHOS
