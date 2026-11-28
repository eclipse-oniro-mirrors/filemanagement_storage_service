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

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <deque>
#include <limits.h>
#include <vector>

#include <scsi/sg.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <securec.h>

// 直接包含实现文件，以覆盖匿名命名空间/静态函数（GetDiscInfoNextTracks / ReadBe32 / ProbeBdTrackInfo）。
#include "disk/disk_common_utils.cpp"

namespace OHOS {
namespace StorageDaemon {

namespace {
/* ---- mock 全局量，供 --wrap= 符号读取与控制 ---- */
struct SgIoResult {
    int ioctlRet = 0;          // ioctl 返回值（<0 表示失败）
    bool infoOk = true;        // true => SG_INFO_OK
    std::vector<uint8_t> data; // 写入 dxferp 的数据（可空）
};

std::deque<SgIoResult> g_sgScript;
SgIoResult g_sgDefault;
bool g_realpathFail = false;
bool g_fopenFail = false;
int g_filenoRet = 3;

std::vector<uint8_t> MakeReadCapacityResp(uint32_t lastLba, uint32_t blockSize)
{
    std::vector<uint8_t> d(8, 0);
    d[0] = (lastLba >> 24) & 0xff;
    d[1] = (lastLba >> 16) & 0xff;
    d[2] = (lastLba >> 8) & 0xff;
    d[3] = lastLba & 0xff;
    d[4] = (blockSize >> 24) & 0xff;
    d[5] = (blockSize >> 16) & 0xff;
    d[6] = (blockSize >> 8) & 0xff;
    d[7] = blockSize & 0xff;
    return d;
}

/* 构造 GET CONFIGURATION 响应：header(8B) + 若干 feature descriptor(4B + additionalLen) */
std::vector<uint8_t> MakeConfigResp(uint16_t currentProfile, const std::vector<std::pair<uint16_t, uint8_t>> &features)
{
    uint32_t descBytes = 0;
    for (const auto &f : features) {
        descBytes += 4 + f.second;
    }
    uint32_t dataLen = 4 + descBytes;
    std::vector<uint8_t> d(4 + dataLen, 0);
    d[0] = (dataLen >> 24) & 0xff;
    d[1] = (dataLen >> 16) & 0xff;
    d[2] = (dataLen >> 8) & 0xff;
    d[3] = dataLen & 0xff;
    d[6] = (currentProfile >> 8) & 0xff;
    d[7] = currentProfile & 0xff;
    size_t off = 8;
    for (const auto &f : features) {
        d[off] = (f.first >> 8) & 0xff;
        d[off + 1] = f.first & 0xff;
        d[off + 2] = 0;
        d[off + 3] = f.second;
        off += 4 + f.second;
    }
    return d;
}

std::vector<uint8_t> MakeTrackInfoResp(bool nwaValid, uint32_t nwa, uint32_t freeBlocks)
{
    std::vector<uint8_t> d(20, 0);
    if (nwaValid) {
        d[TRACK_INFO_NWA_VALID_OFFSET] = TRACK_INFO_NWA_VALID_MASK;
    }
    d[TRACK_INFO_NWA_OFFSET] = (nwa >> 24) & 0xff;
    d[TRACK_INFO_NWA_OFFSET + 1] = (nwa >> 16) & 0xff;
    d[TRACK_INFO_NWA_OFFSET + 2] = (nwa >> 8) & 0xff;
    d[TRACK_INFO_NWA_OFFSET + 3] = nwa & 0xff;
    d[TRACK_INFO_FREE_BLOCKS_OFFSET] = (freeBlocks >> 24) & 0xff;
    d[TRACK_INFO_FREE_BLOCKS_OFFSET + 1] = (freeBlocks >> 16) & 0xff;
    d[TRACK_INFO_FREE_BLOCKS_OFFSET + 2] = (freeBlocks >> 8) & 0xff;
    d[TRACK_INFO_FREE_BLOCKS_OFFSET + 3] = freeBlocks & 0xff;
    return d;
}

std::vector<uint8_t> MakeDiscInfoResp(uint8_t firstTrack, uint8_t lastTrack, uint8_t altLastTrack)
{
    std::vector<uint8_t> d(12, 0);
    d[DISC_INFO_FIRST_TRACK_IN_LAST_SESSION_OFFSET] = firstTrack;
    d[DISC_INFO_LAST_TRACK_IN_LAST_SESSION_OFFSET] = lastTrack;
    d[DISC_INFO_LAST_TRACK_ALT_OFFSET] = altLastTrack;
    return d;
}

bool IsMockDevicePath(const char *path)
{
    return path != nullptr && std::strncmp(path, "/dev/", 5) == 0;
}

FILE *g_fakeFile = reinterpret_cast<FILE *>(0x1);
} // namespace

extern "C" char *__real_realpath(const char *path, char *resolved);
extern "C" FILE *__real_fopen(const char *path, const char *mode);
extern "C" int __real_fileno(FILE *file);
extern "C" int __real_fclose(FILE *file);

extern "C" int __wrap_ioctl(int fd, unsigned long request, ...)
{
    (void)fd;
    SgIoResult res = g_sgScript.empty() ? g_sgDefault : g_sgScript.front();
    if (!g_sgScript.empty()) {
        g_sgScript.pop_front();
    }
    if (res.ioctlRet < 0) {
        return -1;
    }
    if (request == SG_IO) {
        va_list ap;
        va_start(ap, request);
        sg_io_hdr_t *hdr = va_arg(ap, sg_io_hdr_t *);
        va_end(ap);
        if (hdr != nullptr) {
            if (hdr->dxferp != nullptr && !res.data.empty()) {
                size_t n = std::min(static_cast<size_t>(hdr->dxfer_len), res.data.size());
                std::memcpy(hdr->dxferp, res.data.data(), n);
            }
            hdr->info = res.infoOk ? 0U : static_cast<unsigned int>(SG_INFO_CHECK);
            hdr->sb_len_wr = 0;
        }
    }
    return 0;
}

extern "C" char *__wrap_realpath(const char *path, char *resolved)
{
    if (!IsMockDevicePath(path)) {
        return __real_realpath(path, resolved);
    }
    if (g_realpathFail) {
        return nullptr;
    }
    if (resolved != nullptr) {
        std::strncpy(resolved, path, PATH_MAX - 1);
        resolved[PATH_MAX - 1] = '\0';
    }
    return resolved;
}

extern "C" FILE *__wrap_fopen(const char *path, const char *mode)
{
    if (!IsMockDevicePath(path)) {
        return __real_fopen(path, mode);
    }
    if (g_fopenFail) {
        return nullptr;
    }
    return g_fakeFile;
}

extern "C" int __wrap_fileno(FILE *file)
{
    if (file != g_fakeFile) {
        return __real_fileno(file);
    }
    return g_filenoRet;
}

extern "C" int __wrap_fclose(FILE *file)
{
    if (file != g_fakeFile) {
        return __real_fclose(file);
    }
    return 0;
}

using namespace testing;
using namespace testing::ext;

class DiskCommonUtilsTest : public testing::Test {
public:
    void SetUp() override
    {
        g_sgScript.clear();
        g_sgDefault = SgIoResult();
        g_realpathFail = false;
        g_fopenFail = false;
        g_filenoRet = 3;
    }
};

/* ---------------- ReadBe32 ---------------- */

HWTEST_F(DiskCommonUtilsTest, ReadBe32_Basic, TestSize.Level1)
{
    unsigned char buf[4] = {0x12, 0x34, 0x56, 0x78};
    EXPECT_EQ(ReadBe32(buf), 0x12345678U);
}

HWTEST_F(DiskCommonUtilsTest, ReadBe32_ZeroAndMax, TestSize.Level1)
{
    unsigned char zero[4] = {0, 0, 0, 0};
    EXPECT_EQ(ReadBe32(zero), 0U);

    unsigned char max[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    EXPECT_EQ(ReadBe32(max), 0xFFFFFFFFU);
}

/* ---------------- GetReadCapacitySize ---------------- */

HWTEST_F(DiskCommonUtilsTest, GetReadCapacitySize_Success, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeReadCapacityResp(0, ODD_LOGICAL_SECTOR_SIZE)});
    int64_t cap = 0;
    EXPECT_EQ(GetReadCapacitySize(3, cap), E_OK);
    EXPECT_EQ(cap, static_cast<int64_t>(ODD_LOGICAL_SECTOR_SIZE));
}

HWTEST_F(DiskCommonUtilsTest, GetReadCapacitySize_IoctlFail, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{-1, true, {}});
    int64_t cap = 0;
    EXPECT_EQ(GetReadCapacitySize(3, cap), E_ERR);
}

HWTEST_F(DiskCommonUtilsTest, GetReadCapacitySize_SgInfoNotOk, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, false, {}});
    int64_t cap = 0;
    EXPECT_EQ(GetReadCapacitySize(3, cap), E_ERR);
}

HWTEST_F(DiskCommonUtilsTest, GetReadCapacitySize_Overflow, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeReadCapacityResp(0xFFFFFFFFU, 0xFFFFFFFFU)});
    int64_t cap = 0;
    EXPECT_EQ(GetReadCapacitySize(3, cap), E_ERR);
}

/* ---------------- GetDiscInfoNextTracks ---------------- */

HWTEST_F(DiskCommonUtilsTest, GetDiscInfoNextTracks_Success, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeDiscInfoResp(1, 4, 3)});
    uint8_t nextTracks[DISC_INFO_MAX_NEXT_TRACKS] = {0};
    size_t count = 0;
    EXPECT_EQ(GetDiscInfoNextTracks(3, nextTracks, DISC_INFO_MAX_NEXT_TRACKS, count), E_OK);
    EXPECT_EQ(count, 4U);
    EXPECT_EQ(nextTracks[0], 4);
    EXPECT_EQ(nextTracks[1], 1);
    EXPECT_EQ(nextTracks[2], 3);
    EXPECT_EQ(nextTracks[3], 5);
}

HWTEST_F(DiskCommonUtilsTest, GetDiscInfoNextTracks_NoCandidate, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeDiscInfoResp(0, 0, 0)});
    uint8_t nextTracks[DISC_INFO_MAX_NEXT_TRACKS] = {0};
    size_t count = 0;
    EXPECT_EQ(GetDiscInfoNextTracks(3, nextTracks, DISC_INFO_MAX_NEXT_TRACKS, count), E_ERR);
    EXPECT_EQ(count, 0U);
}

HWTEST_F(DiskCommonUtilsTest, GetDiscInfoNextTracks_IoctlFail, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{-1, true, {}});
    uint8_t nextTracks[DISC_INFO_MAX_NEXT_TRACKS] = {0};
    size_t count = 0;
    EXPECT_EQ(GetDiscInfoNextTracks(3, nextTracks, DISC_INFO_MAX_NEXT_TRACKS, count), E_ERR);
}

/* ---------------- ProbeBdTrackInfo ---------------- */

HWTEST_F(DiskCommonUtilsTest, ProbeBdTrackInfo_ReadFail, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{-1, true, {}});
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(ProbeBdTrackInfo(3, 1, TRACK_INFO_INVISIBLE_TRACK, false, 0, total, used), 0);
}

HWTEST_F(DiskCommonUtilsTest, ProbeBdTrackInfo_NwaInvalid, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeTrackInfoResp(false, 10, 90)});
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(ProbeBdTrackInfo(3, 1, TRACK_INFO_INVISIBLE_TRACK, false, 0, total, used), 0);
}

HWTEST_F(DiskCommonUtilsTest, ProbeBdTrackInfo_FreeBlocksZero, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeTrackInfoResp(true, 10, 0)});
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(ProbeBdTrackInfo(3, 1, TRACK_INFO_INVISIBLE_TRACK, false, 0, total, used), 0);
}

HWTEST_F(DiskCommonUtilsTest, ProbeBdTrackInfo_Success, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeTrackInfoResp(true, 10, 90)});
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(ProbeBdTrackInfo(3, 1, TRACK_INFO_INVISIBLE_TRACK, false, 0, total, used), 1);
    EXPECT_EQ(total, static_cast<int64_t>(100 * ODD_LOGICAL_SECTOR_SIZE));
    EXPECT_EQ(used, static_cast<int64_t>(10 * ODD_LOGICAL_SECTOR_SIZE));
}

HWTEST_F(DiskCommonUtilsTest, ProbeBdTrackInfo_ClampToReadCapacity, TestSize.Level1)
{
    int64_t rcCapacity = 1000000;
    g_sgScript.push_back(SgIoResult{0, true, MakeTrackInfoResp(true, 10, 90)});
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(ProbeBdTrackInfo(3, 1, TRACK_INFO_INVISIBLE_TRACK, true, rcCapacity, total, used), 1);
    EXPECT_EQ(total, rcCapacity);
    EXPECT_EQ(used, -1);
}

/* ---------------- GetOpticalDriveType ---------------- */

HWTEST_F(DiskCommonUtilsTest, GetOpticalDriveType_ReadConfigFail, TestSize.Level1)
{
    g_realpathFail = true;
    EXPECT_EQ(GetOpticalDriveType("/dev/sg1"), "");
}

HWTEST_F(DiskCommonUtilsTest, GetOpticalDriveType_FopenFail, TestSize.Level1)
{
    g_fopenFail = true;
    EXPECT_EQ(GetOpticalDriveType("/dev/sg1"), "");
}

HWTEST_F(DiskCommonUtilsTest, GetOpticalDriveType_BdRe, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeConfigResp(0, {{0x0042, 0}})});
    EXPECT_EQ(GetOpticalDriveType("/dev/sg1"), "BD-RE");
}

HWTEST_F(DiskCommonUtilsTest, GetOpticalDriveType_DvdRw, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeConfigResp(0, {{0x002D, 0}})});
    EXPECT_EQ(GetOpticalDriveType("/dev/sg1"), "DVD RW");
}

HWTEST_F(DiskCommonUtilsTest, GetOpticalDriveType_ProfileFallback, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeConfigResp(0x0008, {})});
    EXPECT_EQ(GetOpticalDriveType("/dev/sg1"), "CD-ROM");
}

HWTEST_F(DiskCommonUtilsTest, GetOpticalDriveType_NoMatch, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeConfigResp(0, {})});
    EXPECT_EQ(GetOpticalDriveType("/dev/sg1"), "");
}

/* ---------------- GetBdTrackCapacity ---------------- */

HWTEST_F(DiskCommonUtilsTest, GetBdTrackCapacity_ReadCapacityFail, TestSize.Level1)
{
    // rc 失败 -> 不可见轨道探测失败 -> disc info 失败 -> LBA 探测失败 -> rcValid=false -> E_ERR
    g_sgScript.push_back(SgIoResult{-1, true, {}}); // GetReadCapacitySize
    g_sgScript.push_back(SgIoResult{-1, true, {}}); // ProbeBdTrackInfo(invisible)
    g_sgScript.push_back(SgIoResult{-1, true, {}}); // GetDiscInfoNextTracks
    g_sgScript.push_back(SgIoResult{-1, true, {}}); // ProbeBdTrackInfo(LBA)
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(GetBdTrackCapacity(3, total, used), E_ERR);
}

HWTEST_F(DiskCommonUtilsTest, GetBdTrackCapacity_InvisibleTrackHit, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeReadCapacityResp(0, ODD_LOGICAL_SECTOR_SIZE)}); // rc=2048
    g_sgScript.push_back(SgIoResult{0, true, MakeTrackInfoResp(true, 10, 90)});                  // invisible hit
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(GetBdTrackCapacity(3, total, used), E_OK);
    EXPECT_EQ(total, static_cast<int64_t>(100 * ODD_LOGICAL_SECTOR_SIZE));
    EXPECT_EQ(used, static_cast<int64_t>(10 * ODD_LOGICAL_SECTOR_SIZE));
}

HWTEST_F(DiskCommonUtilsTest, GetBdTrackCapacity_DiscInfoTrackHit, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeReadCapacityResp(0, ODD_LOGICAL_SECTOR_SIZE)}); // rc=2048
    g_sgScript.push_back(SgIoResult{-1, true, {}});                                              // invisible 失败
    g_sgScript.push_back(SgIoResult{0, true, MakeDiscInfoResp(1, 4, 3)});                        // next tracks
    g_sgScript.push_back(SgIoResult{0, true, MakeTrackInfoResp(true, 5, 15)});                   // track=4 命中
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(GetBdTrackCapacity(3, total, used), E_OK);
    EXPECT_EQ(total, static_cast<int64_t>(20 * ODD_LOGICAL_SECTOR_SIZE));
    EXPECT_EQ(used, static_cast<int64_t>(5 * ODD_LOGICAL_SECTOR_SIZE));
}

HWTEST_F(DiskCommonUtilsTest, GetBdTrackCapacity_FallbackToReadCapacity, TestSize.Level1)
{
    g_sgScript.push_back(SgIoResult{0, true, MakeReadCapacityResp(99, ODD_LOGICAL_SECTOR_SIZE)}); // rc=100*2048
    g_sgScript.push_back(SgIoResult{-1, true, {}});                                               // invisible 失败
    g_sgScript.push_back(SgIoResult{-1, true, {}});                                               // disc info 失败
    g_sgScript.push_back(SgIoResult{-1, true, {}});                                               // LBA 失败
    int64_t total = 0;
    int64_t used = 0;
    EXPECT_EQ(GetBdTrackCapacity(3, total, used), E_OK);
    EXPECT_EQ(total, static_cast<int64_t>(100 * ODD_LOGICAL_SECTOR_SIZE));
    EXPECT_EQ(used, -1);
}

} // namespace StorageDaemon
} // namespace OHOS