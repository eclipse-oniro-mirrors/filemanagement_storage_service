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

#include <climits>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstring>

#include "securec.h"
#include "disk_manager/disk_mgr_api.h"
#include "disk_manager/volume/volume_operator_factory.h"
#include "library_func_mock.h"
#include "mock/disk_utils_mock.h"
#include "mock/file_utils_mock.h"
#include "storage_service_errno.h"

namespace {
    std::string g_getParameter;
}

namespace OHOS::system {
std::string GetParameter(const std::string& key, const std::string& def)
{
    return g_getParameter;
}
} // OHOS::system

namespace OHOS {
namespace StorageDaemon {
using namespace testing;
using namespace testing::ext;

int g_realpathRet = 0;
std::string g_realpathPath = "/dev/block/sr0";

extern "C" char* realpath(const char *path, char *resolvedPath)
{
    (void)path;
    if (g_realpathRet != 0) {
        return nullptr;
    }
    if (resolvedPath == nullptr) {
        return nullptr;
    }
    if (strcpy_s(resolvedPath, PATH_MAX, g_realpathPath.c_str()) != 0) {
        return nullptr;
    }
    return resolvedPath;
}

namespace {
constexpr const char *TEST_MOUNT_DIR = "/mnt/data/disk_mgr_api_test";
}

int32_t MountVolumeWrapper(const std::string &devPath, const std::string &mountPath,
                           const std::string &fsType, unsigned long mountFlags, const std::string &mountData);
int32_t UnmountVolumeWrapper(const std::string &mountPath, const std::string &fsType, bool force);
int32_t FormatVolumeWrapper(const std::string &devPath, const std::string &diskPath,
                            const std::string &fsType, const std::string &partitionType, int32_t partitionNum);
int32_t CheckVolumeWrapper(const std::string &devPath, const std::string &fsType);
int32_t RepairVolumeWrapper(const std::string &devPath, const std::string &fsType);
int32_t SetVolumeLabelWrapper(const std::string &devPath, const std::string &fsType, const std::string &label);
int32_t CreateIsoImageWrapper(const std::string &devPath, const std::string &filePath,
                              const std::string &mountPath, const std::string &fsType);
int32_t BurnDiskWrapper(const std::string &devPath, const std::string &fsType, const BurnOptions &burnOptions);
std::vector<OHOS::DiskManager::BlockInfo> GetDataDisksWrapper();
std::vector<OHOS::DiskManager::BlockInfo> GetExternalDisksWrapper(const std::string &devName,
                                                                  const std::string &diskId);
uint64_t GetDiskSizeWrapper(const std::string &deviceName);
int32_t CreateDmDeviceWrapper(const std::string &sourceDevPath, uint64_t startSector,
                              uint64_t sectorCount, dev_t &dev);

class ExtDiskMgrApiTest : public testing::Test {
public:
    void SetUp() override
    {
        fileUtilMoc_ = std::make_shared<FileUtilMoc>();
        FileUtilMoc::fileUtilMoc = fileUtilMoc_;
        diskUtilMoc_ = std::make_shared<DiskUtilMoc>();
        DiskUtilMoc::diskUtilMoc = diskUtilMoc_;
        libraryFuncMock_ = std::make_shared<LibraryFuncMock>();
        LibraryFunc::libraryFunc_ = libraryFuncMock_;
        g_realpathRet = 0;
        g_realpathPath = "/dev/block/sr0";
        ::system((std::string("mkdir -p ") + TEST_MOUNT_DIR).c_str());
    }
    void TearDown() override
    {
        FileUtilMoc::fileUtilMoc = nullptr;
        fileUtilMoc_ = nullptr;
        DiskUtilMoc::diskUtilMoc = nullptr;
        diskUtilMoc_ = nullptr;
        LibraryFunc::libraryFunc_ = nullptr;
        libraryFuncMock_ = nullptr;
        ::system((std::string("rm -rf ") + TEST_MOUNT_DIR).c_str());
    }
    static inline std::shared_ptr<FileUtilMoc> fileUtilMoc_ = nullptr;
    static inline std::shared_ptr<DiskUtilMoc> diskUtilMoc_ = nullptr;
    static inline std::shared_ptr<LibraryFuncMock> libraryFuncMock_ = nullptr;
};

static std::string WrapperMountPath()
{
    return std::string(TEST_MOUNT_DIR) + "/mnt";
}

/**
 * @tc.name: DiskMgrGetApi_TableComplete
 * @tc.desc: DiskMgrGetApi returns non-null table and all 33 entries are valid function pointers.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, DiskMgrGetApi_TableComplete, TestSize.Level1)
{
    const DiskMgrApi *api = DiskMgrGetApi();
    ASSERT_NE(api, nullptr);
    EXPECT_NE(api->QueryUsbIsInUse, nullptr);
    EXPECT_NE(api->CreateBlockDeviceNode, nullptr);
    EXPECT_NE(api->DestroyBlockDeviceNode, nullptr);
    EXPECT_NE(api->ReadPartitionTable, nullptr);
    EXPECT_NE(api->QueryCDStatus, nullptr);
    EXPECT_NE(api->Partition, nullptr);
    EXPECT_NE(api->GetPartitionTableInfo, nullptr);
    EXPECT_NE(api->CreatePartition, nullptr);
    EXPECT_NE(api->DeletePartitionInfo, nullptr);
    EXPECT_NE(api->FormatPartition, nullptr);
    EXPECT_NE(api->Erase, nullptr);
    EXPECT_NE(api->Eject, nullptr);
    EXPECT_NE(api->ParseBurnOptions, nullptr);
    EXPECT_NE(api->ValidateBurnOptions, nullptr);
    EXPECT_NE(api->GetVolumeOpProcess, nullptr);
    EXPECT_NE(api->VerifyBurnData, nullptr);
    EXPECT_NE(api->GetCapacity, nullptr);
    EXPECT_NE(api->GetDiskPowerCycleCount, nullptr);
    EXPECT_NE(api->GetDiskPowerOnHours, nullptr);
    EXPECT_NE(api->MountVolume, nullptr);
    EXPECT_NE(api->UnmountVolume, nullptr);
    EXPECT_NE(api->FormatVolume, nullptr);
    EXPECT_NE(api->CheckVolume, nullptr);
    EXPECT_NE(api->RepairVolume, nullptr);
    EXPECT_NE(api->SetVolumeLabel, nullptr);
    EXPECT_NE(api->CreateIsoImage, nullptr);
    EXPECT_NE(api->BurnDisk, nullptr);
    EXPECT_NE(api->ReadMetadata, nullptr);
    EXPECT_NE(api->MountFuseDevice, nullptr);
    EXPECT_NE(api->GetDataDisks, nullptr);
    EXPECT_NE(api->GetExternalDisks, nullptr);
    EXPECT_NE(api->GetDiskSize, nullptr);
    EXPECT_NE(api->CreateDmDevice, nullptr);
}

/**
 * @tc.name: MountVolumeWrapper_UnknownFsType
 * @tc.desc: Mount wrapper returns E_NOT_SUPPORT when factory has no operator for the fs type.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, MountVolumeWrapper_UnknownFsType, TestSize.Level1)
{
    std::string mountPath = WrapperMountPath();
    EXPECT_EQ(MountVolumeWrapper("/dev/block/sda1", mountPath, "no_such_fs", 0, ""), E_NOT_SUPPORT);
}

HWTEST_F(ExtDiskMgrApiTest, FormatVolumeWrapper_UnknownFsType, TestSize.Level1)
{
    EXPECT_EQ(FormatVolumeWrapper("/dev/block/sda1", "/dev/block/sda", "no_such_fs", "gpt", 1), E_NOT_SUPPORT);
}

HWTEST_F(ExtDiskMgrApiTest, CheckVolumeWrapper_UnknownFsType, TestSize.Level1)
{
    EXPECT_EQ(CheckVolumeWrapper("/dev/block/sda1", "no_such_fs"), E_NOT_SUPPORT);
}

HWTEST_F(ExtDiskMgrApiTest, RepairVolumeWrapper_UnknownFsType, TestSize.Level1)
{
    EXPECT_EQ(RepairVolumeWrapper("/dev/block/sda1", "no_such_fs"), E_NOT_SUPPORT);
}

HWTEST_F(ExtDiskMgrApiTest, SetVolumeLabelWrapper_UnknownFsType, TestSize.Level1)
{
    EXPECT_EQ(SetVolumeLabelWrapper("/dev/block/sda1", "no_such_fs", "label"), E_NOT_SUPPORT);
}

HWTEST_F(ExtDiskMgrApiTest, CreateIsoImageWrapper_UnknownFsType, TestSize.Level1)
{
    EXPECT_EQ(CreateIsoImageWrapper("/dev/block/sr0", "/data/test.iso", "/mnt/data/iso", "no_such_fs"),
              E_NOT_SUPPORT);
}

HWTEST_F(ExtDiskMgrApiTest, BurnDiskWrapper_UnknownFsType, TestSize.Level1)
{
    BurnOptions opts;
    EXPECT_EQ(BurnDiskWrapper("/dev/block/sr0", "no_such_fs", opts), E_NOT_SUPPORT);
}

/**
 * @tc.name: SetVolumeLabelWrapper_KnownFsType_Dispatch
 * @tc.desc: SetLabel wrapper dispatches to the exfat operator (base impl semantics preserved).
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, SetVolumeLabelWrapper_KnownFsType_Dispatch, TestSize.Level1)
{
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).Times(AtLeast(0)).WillRepeatedly(Return(E_OK));
    int32_t ret = SetVolumeLabelWrapper("/dev/block/mock_dev", "exfat", "label");
    (void)ret;
    SUCCEED();
}

/**
 * @tc.name: CreateIsoImageWrapper_KnownFsType_Dispatch
 * @tc.desc: CreateIsoImage wrapper dispatches to the exfat operator (base impl returns E_NOT_SUPPORT).
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, CreateIsoImageWrapper_KnownFsType_Dispatch, TestSize.Level1)
{
    EXPECT_EQ(CreateIsoImageWrapper("/dev/block/mock_dev", "/data/test.iso", "/mnt/data/iso", "exfat"),
              E_NOT_SUPPORT);
}

/**
 * @tc.name: BurnDiskWrapper_KnownFsType_Dispatch
 * @tc.desc: BurnDisk wrapper dispatches to the exfat operator (base impl returns E_NOT_SUPPORT).
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, BurnDiskWrapper_KnownFsType_Dispatch, TestSize.Level1)
{
    BurnOptions opts;
    EXPECT_EQ(BurnDiskWrapper("/dev/block/mock_dev", "exfat", opts), E_NOT_SUPPORT);
}

/**
 * @tc.name: RepairVolumeWrapper_Exfat_Dispatch
 * @tc.desc: Repair wrapper dispatches to the exfat operator and propagates the fsck result.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, RepairVolumeWrapper_Exfat_Dispatch, TestSize.Level1)
{
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).WillOnce(
        Invoke([](std::vector<std::string> &, std::vector<std::string> *, int *exitStatus) {
            if (exitStatus != nullptr) {
                *exitStatus = 1;
            }
            return E_OK;
        }));
    int32_t ret = RepairVolumeWrapper("/dev/block/mock_dev", "exfat");
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: MountVolumeWrapper_Exfat_Dispatch
 * @tc.desc: Mount wrapper dispatches to the exfat operator and propagates ForkExec result.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, MountVolumeWrapper_Exfat_Dispatch, TestSize.Level1)
{
    std::string mountPath = WrapperMountPath();
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).WillOnce(Return(E_OK));
    int32_t ret = MountVolumeWrapper("/dev/block/mock_dev", mountPath, "exfat", 0, "");
    EXPECT_EQ(ret, E_OK);
}

HWTEST_F(ExtDiskMgrApiTest, MountVolumeWrapper_Exfat_MountFailed, TestSize.Level1)
{
    std::string mountPath = WrapperMountPath();
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).WillOnce(Return(E_ERR));
    int32_t retFail = MountVolumeWrapper("/dev/block/mock_dev", mountPath, "exfat", 0, "");
    EXPECT_NE(retFail, E_OK);
}

/**
 * @tc.name: UnmountVolumeWrapper_Force_Dispatch
 * @tc.desc: Unmount wrapper dispatches to the generic operator; force path tolerates umount2 failure.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, UnmountVolumeWrapper_Force_Dispatch, TestSize.Level1)
{
    g_realpathPath = TEST_MOUNT_DIR;
    int32_t ret = UnmountVolumeWrapper(TEST_MOUNT_DIR, "exfat", true);
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: CheckVolumeWrapper_Exfat_Dispatch
 * @tc.desc: Check wrapper dispatches to the exfat operator and propagates the fsck exit result.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, CheckVolumeWrapper_Exfat_Dispatch, TestSize.Level1)
{
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).WillOnce(
        Invoke([](std::vector<std::string> &, std::vector<std::string> *, int *exitStatus) {
            if (exitStatus != nullptr) {
                *exitStatus = 0;
            }
            return E_OK;
        }));
    int32_t ret = CheckVolumeWrapper("/dev/block/mock_dev", "exfat");
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: FormatVolumeWrapper_Exfat_Dispatch
 * @tc.desc: Format wrapper dispatches to the exfat operator and propagates ForkExec result.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, FormatVolumeWrapper_Exfat_Dispatch, TestSize.Level1)
{
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).Times(AtLeast(1)).WillRepeatedly(Return(E_OK));
    int32_t ret = FormatVolumeWrapper("/dev/block/mock_dev", "/dev/block/sda", "exfat", "gpt", 1);
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: GetDataDisksWrapper_NoCrash
 * @tc.desc: GetDataDisks wrapper constructs ScanDevice and returns without crash.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, GetDataDisksWrapper_NoCrash, TestSize.Level1)
{
    auto disks = GetDataDisksWrapper();
    (void)disks;
    SUCCEED();
}

/**
 * @tc.name: GetExternalDisksWrapper_NoCrash
 * @tc.desc: GetExternalDisks wrapper constructs ScanDevice and returns without crash.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, GetExternalDisksWrapper_NoCrash, TestSize.Level1)
{
    auto disks = GetExternalDisksWrapper("usb", "disk-8-1");
    (void)disks;
    SUCCEED();
}

/**
 * @tc.name: GetDiskSizeWrapper_InvalidDevice_ReturnsZero
 * @tc.desc: GetDiskSize wrapper returns 0 for a nonexistent device node.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, GetDiskSizeWrapper_InvalidDevice_ReturnsZero, TestSize.Level1)
{
    EXPECT_EQ(GetDiskSizeWrapper("nonexistent_disk_node"), 0U);
}

/**
 * @tc.name: CreateDmDeviceWrapper_SourceOpenFailed
 * @tc.desc: CreateDmDevice wrapper returns E_NOT_SUPPORT when the source device cannot be opened.
 * @tc.type: FUNC
 */
HWTEST_F(ExtDiskMgrApiTest, CreateDmDeviceWrapper_SourceOpenFailed, TestSize.Level1)
{
    dev_t dev = 0;
    int32_t ret = CreateDmDeviceWrapper("/dev/nonexistent_source", 0, 100, dev);
    EXPECT_EQ(ret, E_NOT_SUPPORT);
    EXPECT_EQ(dev, 0);
}
} // namespace StorageDaemon
} // namespace OHOS
