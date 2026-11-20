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
#include <dlfcn.h>

#include <chrono>
#include <thread>

#include "disk_manager/disk_manager_lib_loader.h"
#include "disk_manager/disk/disk_utils.h"
#include "storage_service_errno.h"

using namespace testing::ext;
using namespace OHOS::StorageDaemon;

namespace {
int g_dlopenCallCount = 0;
int g_dlcloseCount = 0;
int g_dlopenFailTimes = 0;
bool g_dlopenOk = false;
bool g_dlsymOk = false;
bool g_getApiNull = false;
int g_fakeSoHandle = 0;
const char *g_lastApiCall = "";
int32_t g_stubRet = 0;
uint64_t g_stubU64 = 0;
dev_t g_stubDev = 0;
std::vector<OHOS::DiskManager::BlockInfo> g_stubBlocks;

constexpr int32_t SENTRY_I32 = 13579;
constexpr uint64_t SENTRY_U64 = 0xABCD1234ULL;
constexpr dev_t SENTRY_DEV = 4242;

void ResetStubState()
{
    g_lastApiCall = "";
    g_stubRet = 0;
    g_stubU64 = 0;
    g_stubDev = 0;
    g_stubBlocks.clear();
}

int32_t StubQueryUsbIsInUse(const std::string &, bool &isInUse)
{
    g_lastApiCall = "QueryUsbIsInUse";
    isInUse = true;
    return g_stubRet;
}

int32_t StubCreateBlockDeviceNode(const std::string &, uint32_t, int32_t, int32_t)
{
    g_lastApiCall = "CreateBlockDeviceNode";
    return g_stubRet;
}

int32_t StubDestroyBlockDeviceNode(const std::string &)
{
    g_lastApiCall = "DestroyBlockDeviceNode";
    return g_stubRet;
}

int32_t StubReadPartitionTable(const std::string &, std::string &output, int32_t &maxVolume)
{
    g_lastApiCall = "ReadPartitionTable";
    output = "stub";
    maxVolume = 15;
    return g_stubRet;
}

int32_t StubQueryCDStatus(const std::string &, int32_t &status)
{
    g_lastApiCall = "QueryCDStatus";
    status = 4;
    return g_stubRet;
}

int32_t StubPartition(const std::string &, const std::string &)
{
    g_lastApiCall = "Partition";
    return g_stubRet;
}

int32_t StubGetPartitionTableInfo(const std::string &, std::string &execRet)
{
    g_lastApiCall = "GetPartitionTableInfo";
    execRet = "stub";
    return g_stubRet;
}

int32_t StubCreatePartition(const std::string &, int32_t, int64_t, int64_t, const std::string &)
{
    g_lastApiCall = "CreatePartition";
    return g_stubRet;
}

int32_t StubDeletePartitionInfo(const std::string &, const std::string &, int32_t)
{
    g_lastApiCall = "DeletePartitionInfo";
    return g_stubRet;
}

int32_t StubFormatPartition(const std::string &, const std::string &, const std::string &,
                            const std::vector<std::string> &, bool)
{
    g_lastApiCall = "FormatPartition";
    return g_stubRet;
}

int32_t StubErase(const std::string &)
{
    g_lastApiCall = "Erase";
    return g_stubRet;
}

int32_t StubEject(const std::string &)
{
    g_lastApiCall = "Eject";
    return g_stubRet;
}

int32_t StubParseBurnOptions(const std::string &, BurnOptions &)
{
    g_lastApiCall = "ParseBurnOptions";
    return g_stubRet;
}

int32_t StubValidateBurnOptions(const BurnOptions &)
{
    g_lastApiCall = "ValidateBurnOptions";
    return g_stubRet;
}

int32_t StubGetVolumeOpProcess(const std::string &, int32_t &progressPct)
{
    g_lastApiCall = "GetVolumeOpProcess";
    progressPct = 50;
    return g_stubRet;
}

int32_t StubVerifyBurnData(const std::string &, int32_t)
{
    g_lastApiCall = "VerifyBurnData";
    return g_stubRet;
}

int32_t StubGetCapacity(const std::string &, int64_t &totalSize, int64_t &freeSize)
{
    g_lastApiCall = "GetCapacity";
    totalSize = 111;
    freeSize = 222;
    return g_stubRet;
}

uint64_t StubGetDiskPowerCycleCount(const std::string &)
{
    g_lastApiCall = "GetDiskPowerCycleCount";
    return g_stubU64;
}

uint64_t StubGetDiskPowerOnHours(const std::string &)
{
    g_lastApiCall = "GetDiskPowerOnHours";
    return g_stubU64;
}

int32_t StubMountVolume(const std::string &, const std::string &, const std::string &, unsigned long,
                        const std::string &)
{
    g_lastApiCall = "MountVolume";
    return g_stubRet;
}

int32_t StubUnmountVolume(const std::string &, const std::string &, bool)
{
    g_lastApiCall = "UnmountVolume";
    return g_stubRet;
}

int32_t StubFormatVolume(const std::string &, const std::string &, const std::string &, const std::string &,
                         int32_t)
{
    g_lastApiCall = "FormatVolume";
    return g_stubRet;
}

int32_t StubCheckVolume(const std::string &, const std::string &)
{
    g_lastApiCall = "CheckVolume";
    return g_stubRet;
}

int32_t StubRepairVolume(const std::string &, const std::string &)
{
    g_lastApiCall = "RepairVolume";
    return g_stubRet;
}

int32_t StubSetVolumeLabel(const std::string &, const std::string &, const std::string &)
{
    g_lastApiCall = "SetVolumeLabel";
    return g_stubRet;
}

int32_t StubCreateIsoImage(const std::string &, const std::string &, const std::string &, const std::string &)
{
    g_lastApiCall = "CreateIsoImage";
    return g_stubRet;
}

int32_t StubBurnDisk(const std::string &, const std::string &, const BurnOptions &)
{
    g_lastApiCall = "BurnDisk";
    return g_stubRet;
}

int32_t StubReadMetadata(const std::string &, std::string &uuid, std::string &type, std::string &label)
{
    g_lastApiCall = "ReadMetadata";
    uuid = "uuid";
    type = "vfat";
    label = "label";
    return g_stubRet;
}

int32_t StubMountFuseDevice(const std::string &, int &fuseFd)
{
    g_lastApiCall = "MountFuseDevice";
    fuseFd = 7;
    return g_stubRet;
}

std::vector<OHOS::DiskManager::BlockInfo> StubGetDataDisks()
{
    g_lastApiCall = "GetDataDisks";
    return g_stubBlocks;
}

std::vector<OHOS::DiskManager::BlockInfo> StubGetExternalDisks(const std::string &, const std::string &)
{
    g_lastApiCall = "GetExternalDisks";
    return g_stubBlocks;
}

uint64_t StubGetDiskSize(const std::string &)
{
    g_lastApiCall = "GetDiskSize";
    return g_stubU64;
}

int32_t StubCreateDmDevice(const std::string &, uint64_t, uint64_t, dev_t &dev)
{
    g_lastApiCall = "CreateDmDevice";
    dev = g_stubDev;
    return g_stubRet;
}
} // namespace

extern "C" const OHOS::StorageDaemon::DiskMgrApi *DiskMgrGetApi(void)
{
    if (g_getApiNull) {
        return nullptr;
    }
    static const OHOS::StorageDaemon::DiskMgrApi api = {
        StubQueryUsbIsInUse,
        StubCreateBlockDeviceNode,
        StubDestroyBlockDeviceNode,
        StubReadPartitionTable,
        StubQueryCDStatus,
        StubPartition,
        StubGetPartitionTableInfo,
        StubCreatePartition,
        StubDeletePartitionInfo,
        StubFormatPartition,
        StubErase,
        StubEject,
        StubParseBurnOptions,
        StubValidateBurnOptions,
        StubGetVolumeOpProcess,
        StubVerifyBurnData,
        StubGetCapacity,
        StubGetDiskPowerCycleCount,
        StubGetDiskPowerOnHours,
        StubMountVolume,
        StubUnmountVolume,
        StubFormatVolume,
        StubCheckVolume,
        StubRepairVolume,
        StubSetVolumeLabel,
        StubCreateIsoImage,
        StubBurnDisk,
        StubReadMetadata,
        StubMountFuseDevice,
        StubGetDataDisks,
        StubGetExternalDisks,
        StubGetDiskSize,
        StubCreateDmDevice,
    };
    return &api;
}

extern "C" {
void *__real_dlopen(const char *filename, int flag);
void *__wrap_dlopen(const char *filename, int flag)
{
    (void)filename;
    (void)flag;
    g_dlopenCallCount++;
    if (g_dlopenFailTimes > 0) {
        g_dlopenFailTimes--;
        return nullptr;
    }
    return g_dlopenOk ? &g_fakeSoHandle : nullptr;
}

void *__real_dlsym(void *handle, const char *symbol);
void *__wrap_dlsym(void *handle, const char *symbol)
{
    (void)handle;
    (void)symbol;
    return g_dlsymOk ? reinterpret_cast<void *>(&DiskMgrGetApi) : nullptr;
}

// 32 位 ARM（musl _REDIR_TIME64）下 dlsym 符号被 dlfcn.h 重定向为 __dlsym_time64，
// --wrap=dlsym 不生效，须额外包装重定向后的符号；64 位构建下无引用、无害。
void *__real___dlsym_time64(void *handle, const char *symbol);
void *__wrap___dlsym_time64(void *handle, const char *symbol)
{
    (void)handle;
    (void)symbol;
    return g_dlsymOk ? reinterpret_cast<void *>(&DiskMgrGetApi) : nullptr;
}

int __real_dlclose(void *handle);
int __wrap_dlclose(void *handle)
{
    (void)handle;
    g_dlcloseCount++;
    return 0;
}
}

class DiskManagerLibLoaderTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp()
    {
        g_dlopenCallCount = 0;
    }
    void TearDown()
    {
        ResetStubState();
    }
};

HWTEST_F(DiskManagerLibLoaderTest, QueryUsbIsInUse_DlopenFailed, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "QueryUsbIsInUse_DlopenFailed start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    bool isInUse = false;
    int32_t ret = loader.QueryUsbIsInUse("/dev/sda1", isInUse);
    EXPECT_EQ(ret, OHOS::E_NOT_SUPPORT);
    GTEST_LOG_(INFO) << "QueryUsbIsInUse_DlopenFailed end";
}

HWTEST_F(DiskManagerLibLoaderTest, ReadPartitionTable_DlopenFailed, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ReadPartitionTable_DlopenFailed start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    std::string output;
    int32_t maxVolume = 0;
    int32_t ret = loader.ReadPartitionTable("/dev/sda", output, maxVolume);
    EXPECT_EQ(ret, OHOS::E_NOT_SUPPORT);
    GTEST_LOG_(INFO) << "ReadPartitionTable_DlopenFailed end";
}

HWTEST_F(DiskManagerLibLoaderTest, GetCapacity_DlopenFailed, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetCapacity_DlopenFailed start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    int64_t totalSize = 0;
    int64_t freeSize = 0;
    int32_t ret = loader.GetCapacity("/dev/sda", totalSize, freeSize);
    EXPECT_EQ(ret, OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(totalSize, 0);
    EXPECT_EQ(freeSize, 0);
    GTEST_LOG_(INFO) << "GetCapacity_DlopenFailed end";
}

HWTEST_F(DiskManagerLibLoaderTest, ParseBurnOptions_DlopenFailed, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ParseBurnOptions_DlopenFailed start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    BurnOptions opts;
    int32_t ret = loader.ParseBurnOptions("test", opts);
    EXPECT_EQ(ret, OHOS::E_NOT_SUPPORT);
    GTEST_LOG_(INFO) << "ParseBurnOptions_DlopenFailed end";
}

HWTEST_F(DiskManagerLibLoaderTest, RetryCooldown_NoHotRetry, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RetryCooldown_NoHotRetry start";
    g_dlopenCallCount = 0;
    auto &loader = DiskManagerLibLoader::GetInstance();
    bool isInUse = false;
    loader.QueryUsbIsInUse("/dev/sda1", isInUse);
    int32_t firstCount = g_dlopenCallCount;
    std::string output;
    int32_t maxVolume = 0;
    loader.ReadPartitionTable("/dev/sda", output, maxVolume);
    int64_t total = 0, free = 0;
    loader.GetCapacity("/dev/sda", total, free);
    BurnOptions opts;
    loader.ParseBurnOptions("test", opts);
    EXPECT_EQ(g_dlopenCallCount, firstCount);
    GTEST_LOG_(INFO) << "RetryCooldown_NoHotRetry end";
}

HWTEST_F(DiskManagerLibLoaderTest, AllWrappers_NotLoaded_ReturnNotSupport, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AllWrappers_NotLoaded_ReturnNotSupport start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    bool isInUse = false;
    std::string output;
    int32_t maxVolume = 0;
    int32_t status = 0;
    std::string execRet;
    BurnOptions opts;
    int32_t progressPct = 0;
    int64_t totalSize = 0;
    int64_t freeSize = 0;
    int fuseFd = 0;
    dev_t dev = 0;
    std::string uuid;
    std::string type;
    std::string label;
    std::vector<std::string> cmd;
    std::vector<OHOS::DiskManager::BlockInfo> blocks;

    EXPECT_EQ(loader.QueryUsbIsInUse("/dev/sda1", isInUse), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.CreateBlockDeviceNode("/dev/sda", 0660, 8, 1), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.DestroyBlockDeviceNode("/dev/sda"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.ReadPartitionTable("/dev/sda", output, maxVolume), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.QueryCDStatus("/dev/sr0", status), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.Partition("/dev/sda", "gpt"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.GetPartitionTableInfo("/dev/sda", execRet), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.CreatePartition("/dev/sda", 1, 2048, 4096, "0700"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.DeletePartitionInfo("/dev/sda", "disk-8-1", 1), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.FormatPartition("/dev/sda", "vfat", "vol", cmd, true), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.Erase("/dev/sda"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.Eject("sr0"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.ParseBurnOptions("opt", opts), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.ValidateBurnOptions(opts), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.GetVolumeOpProcess("vol-1", progressPct), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.VerifyBurnData("/dev/sr0", 1), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.GetCapacity("/dev/sr0", totalSize, freeSize), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.GetDiskPowerCycleCount("sda"), 0U);
    EXPECT_EQ(loader.GetDiskPowerOnHours("sda"), 0U);
    EXPECT_EQ(loader.MountVolume("/dev/sda1", "/mnt/usb", "vfat", 0, ""), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.UnmountVolume("/mnt/usb", "vfat", false), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.FormatVolume("/dev/sda1", "/dev/sda", "vfat", "gpt", 1), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.CheckVolume("/dev/sda1", "exfat"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.RepairVolume("/dev/sda1", "exfat"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.SetVolumeLabel("/dev/sda1", "exfat", "label"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.CreateIsoImage("/dev/sr0", "/data/iso", "/mnt/cd", "iso9660"), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.BurnDisk("/dev/sr0", "udf", opts), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.ReadMetadata("/dev/sda1", uuid, type, label), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(loader.MountFuseDevice("/mnt/fuse", fuseFd), OHOS::E_NOT_SUPPORT);
    EXPECT_TRUE(loader.GetDataDisks().empty());
    EXPECT_TRUE(loader.GetExternalDisks("usb", "disk-8-1").empty());
    EXPECT_EQ(loader.GetDiskSize("sda"), 0U);
    EXPECT_EQ(loader.CreateDmDevice("/dev/sda", 0, 100, dev), OHOS::E_NOT_SUPPORT);
    EXPECT_EQ(dev, 0);
    GTEST_LOG_(INFO) << "AllWrappers_NotLoaded_ReturnNotSupport end";
}

HWTEST_F(DiskManagerLibLoaderTest, RetryAfterCooldown_SelfHeal, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "RetryAfterCooldown_SelfHeal start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    bool isInUse = false;
    loader.QueryUsbIsInUse("/dev/sda1", isInUse);
    g_dlopenCallCount = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    int32_t ret = loader.QueryUsbIsInUse("/dev/sda1", isInUse);
    EXPECT_EQ(ret, OHOS::E_NOT_SUPPORT);
    EXPECT_GE(g_dlopenCallCount, 1);
    GTEST_LOG_(INFO) << "RetryAfterCooldown_SelfHeal end";
}

HWTEST_F(DiskManagerLibLoaderTest, DlsymFailed_DeinitRollback, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "DlsymFailed_DeinitRollback start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    g_dlopenOk = true;
    g_dlsymOk = false;
    int dlcloseBefore = g_dlcloseCount;
    bool isInUse = false;
    int32_t ret = loader.QueryUsbIsInUse("/dev/sda1", isInUse);
    EXPECT_EQ(ret, OHOS::E_NOT_SUPPORT);
    EXPECT_GE(g_dlcloseCount - dlcloseBefore, 1);
    g_dlopenOk = false;
    g_dlsymOk = false;
    GTEST_LOG_(INFO) << "DlsymFailed_DeinitRollback end";
}

HWTEST_F(DiskManagerLibLoaderTest, GetApiNull_DeinitRollback, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "GetApiNull_DeinitRollback start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    g_dlopenOk = true;
    g_dlsymOk = true;
    g_getApiNull = true;
    int dlcloseBefore = g_dlcloseCount;
    bool isInUse = false;
    int32_t ret = loader.QueryUsbIsInUse("/dev/sda1", isInUse);
    EXPECT_EQ(ret, OHOS::E_NOT_SUPPORT);
    EXPECT_GE(g_dlcloseCount - dlcloseBefore, 1);
    g_dlopenOk = false;
    g_dlsymOk = false;
    g_getApiNull = false;
    GTEST_LOG_(INFO) << "GetApiNull_DeinitRollback end";
}

HWTEST_F(DiskManagerLibLoaderTest, InitRetry_FailTwiceThenSucceed, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "InitRetry_FailTwiceThenSucceed start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    g_dlopenOk = true;
    g_dlsymOk = true;
    g_dlopenFailTimes = 2;
    g_stubRet = OHOS::E_OK;
    int dlopenBefore = g_dlopenCallCount;
    bool isInUse = false;
    int32_t ret = loader.QueryUsbIsInUse("/dev/sda1", isInUse);
    EXPECT_EQ(ret, OHOS::E_OK);
    EXPECT_EQ(g_dlopenCallCount - dlopenBefore, 3);
    EXPECT_STREQ(g_lastApiCall, "QueryUsbIsInUse");
    EXPECT_TRUE(isInUse);
    GTEST_LOG_(INFO) << "InitRetry_FailTwiceThenSucceed end";
}

HWTEST_F(DiskManagerLibLoaderTest, EnsureLoaded_FastPath_NoReload, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EnsureLoaded_FastPath_NoReload start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    g_stubRet = OHOS::E_OK;
    int dlopenBefore = g_dlopenCallCount;
    bool isInUse = false;
    EXPECT_EQ(loader.QueryUsbIsInUse("/dev/sda1", isInUse), OHOS::E_OK);
    std::string output;
    int32_t maxVolume = 0;
    EXPECT_EQ(loader.ReadPartitionTable("/dev/sda", output, maxVolume), OHOS::E_OK);
    EXPECT_EQ(g_dlopenCallCount - dlopenBefore, 0);
    EXPECT_TRUE(loader.EnsureLoaded());
    GTEST_LOG_(INFO) << "EnsureLoaded_FastPath_NoReload end";
}

HWTEST_F(DiskManagerLibLoaderTest, AllWrappers_DispatchToApiTable, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AllWrappers_DispatchToApiTable start";
    auto &loader = DiskManagerLibLoader::GetInstance();
    g_stubRet = SENTRY_I32;
    g_stubU64 = SENTRY_U64;
    g_stubDev = SENTRY_DEV;
    OHOS::DiskManager::BlockInfo info;
    g_stubBlocks.push_back(info);

    bool isInUse = false;
    std::string output;
    int32_t maxVolume = 0;
    int32_t status = 0;
    std::string execRet;
    BurnOptions opts;
    int32_t progressPct = 0;
    int64_t totalSize = 0;
    int64_t freeSize = 0;
    int fuseFd = 0;
    dev_t dev = 0;
    std::string uuid;
    std::string type;
    std::string label;
    std::vector<std::string> cmd;

    EXPECT_EQ(loader.QueryUsbIsInUse("/dev/sda1", isInUse), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "QueryUsbIsInUse");
    EXPECT_EQ(loader.CreateBlockDeviceNode("/dev/sda", 0660, 8, 1), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "CreateBlockDeviceNode");
    EXPECT_EQ(loader.DestroyBlockDeviceNode("/dev/sda"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "DestroyBlockDeviceNode");
    EXPECT_EQ(loader.ReadPartitionTable("/dev/sda", output, maxVolume), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "ReadPartitionTable");
    EXPECT_EQ(output, "stub");
    EXPECT_EQ(maxVolume, 15);
    EXPECT_EQ(loader.QueryCDStatus("/dev/sr0", status), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "QueryCDStatus");
    EXPECT_EQ(status, 4);
    EXPECT_EQ(loader.Partition("/dev/sda", "gpt"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "Partition");
    EXPECT_EQ(loader.GetPartitionTableInfo("/dev/sda", execRet), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "GetPartitionTableInfo");
    EXPECT_EQ(execRet, "stub");
    EXPECT_EQ(loader.CreatePartition("/dev/sda", 1, 2048, 4096, "0700"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "CreatePartition");
    EXPECT_EQ(loader.DeletePartitionInfo("/dev/sda", "disk-8-1", 1), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "DeletePartitionInfo");
    EXPECT_EQ(loader.FormatPartition("/dev/sda", "vfat", "vol", cmd, true), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "FormatPartition");
    EXPECT_EQ(loader.Erase("/dev/sda"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "Erase");
    EXPECT_EQ(loader.Eject("sr0"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "Eject");
    EXPECT_EQ(loader.ParseBurnOptions("opt", opts), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "ParseBurnOptions");
    EXPECT_EQ(loader.ValidateBurnOptions(opts), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "ValidateBurnOptions");
    EXPECT_EQ(loader.GetVolumeOpProcess("vol-1", progressPct), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "GetVolumeOpProcess");
    EXPECT_EQ(progressPct, 50);
    EXPECT_EQ(loader.VerifyBurnData("/dev/sr0", 1), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "VerifyBurnData");
    EXPECT_EQ(loader.GetCapacity("/dev/sr0", totalSize, freeSize), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "GetCapacity");
    EXPECT_EQ(totalSize, 111);
    EXPECT_EQ(freeSize, 222);
    EXPECT_EQ(loader.GetDiskPowerCycleCount("sda"), SENTRY_U64);
    EXPECT_STREQ(g_lastApiCall, "GetDiskPowerCycleCount");
    EXPECT_EQ(loader.GetDiskPowerOnHours("sda"), SENTRY_U64);
    EXPECT_STREQ(g_lastApiCall, "GetDiskPowerOnHours");
    EXPECT_EQ(loader.MountVolume("/dev/sda1", "/mnt/usb", "vfat", 0, ""), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "MountVolume");
    EXPECT_EQ(loader.UnmountVolume("/mnt/usb", "vfat", false), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "UnmountVolume");
    EXPECT_EQ(loader.FormatVolume("/dev/sda1", "/dev/sda", "vfat", "gpt", 1), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "FormatVolume");
    EXPECT_EQ(loader.CheckVolume("/dev/sda1", "exfat"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "CheckVolume");
    EXPECT_EQ(loader.RepairVolume("/dev/sda1", "exfat"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "RepairVolume");
    EXPECT_EQ(loader.SetVolumeLabel("/dev/sda1", "exfat", "label"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "SetVolumeLabel");
    EXPECT_EQ(loader.CreateIsoImage("/dev/sr0", "/data/iso", "/mnt/cd", "iso9660"), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "CreateIsoImage");
    EXPECT_EQ(loader.BurnDisk("/dev/sr0", "udf", opts), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "BurnDisk");
    EXPECT_EQ(loader.ReadMetadata("/dev/sda1", uuid, type, label), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "ReadMetadata");
    EXPECT_EQ(loader.MountFuseDevice("/mnt/fuse", fuseFd), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "MountFuseDevice");
    EXPECT_EQ(fuseFd, 7);
    EXPECT_EQ(loader.GetDataDisks().size(), 1U);
    EXPECT_STREQ(g_lastApiCall, "GetDataDisks");
    EXPECT_EQ(loader.GetExternalDisks("usb", "disk-8-1").size(), 1U);
    EXPECT_STREQ(g_lastApiCall, "GetExternalDisks");
    EXPECT_EQ(loader.GetDiskSize("sda"), SENTRY_U64);
    EXPECT_STREQ(g_lastApiCall, "GetDiskSize");
    EXPECT_EQ(loader.CreateDmDevice("/dev/sda", 0, 100, dev), SENTRY_I32);
    EXPECT_STREQ(g_lastApiCall, "CreateDmDevice");
    EXPECT_EQ(dev, SENTRY_DEV);
    GTEST_LOG_(INFO) << "AllWrappers_DispatchToApiTable end";
}
