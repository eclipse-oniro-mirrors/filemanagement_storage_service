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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock/file_utils_mock.h"
#include "storage_service_errno.h"
#include "disk_manager/disk/disk_common_utils.h"

namespace OHOS {
namespace StorageDaemon {
using namespace testing;
using namespace testing::ext;

class BlkidFormTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<FileUtilMoc> fileUtilMoc_ = nullptr;
};

void BlkidFormTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase Start";
}

void BlkidFormTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase Start";
}

void BlkidFormTest::SetUp(void)
{
    fileUtilMoc_ = std::make_shared<FileUtilMoc>();
    FileUtilMoc::fileUtilMoc = fileUtilMoc_;
}

void BlkidFormTest::TearDown(void)
{
    if (fileUtilMoc_) {
        FileUtilMoc::fileUtilMoc = nullptr;
        fileUtilMoc_ = nullptr;
    }
}

/**
 * @tc.name: BlkidFormTest_BuildBlkidCmd_Legacy_001
 * @tc.desc: Old implementation form, command identical to current one byte by byte.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_BuildBlkidCmd_Legacy_001, TestSize.Level1)
{
    std::vector<std::string> expected = {
        "blkid", "-n", "mdraid", "-s", "TYPE", "-o", "value", "/dev/block/sda1"
    };
    EXPECT_EQ(BuildBlkidCmd(false, "TYPE", "/dev/block/sda1"), expected);
}

/**
 * @tc.name: BlkidFormTest_BuildBlkidCmd_UtilLinux_001
 * @tc.desc: Util-linux form command, -p enables -u noraid to exclude raid usage.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_BuildBlkidCmd_UtilLinux_001, TestSize.Level1)
{
    std::vector<std::string> expected = {
        "blkid", "-p", "-u", "noraid", "-s", "TYPE", "-o", "value", "/dev/block/sda1"
    };
    EXPECT_EQ(BuildBlkidCmd(true, "TYPE", "/dev/block/sda1"), expected);
}

/**
 * @tc.name: BlkidFormTest_HasUtilLinuxKeyword_Match_001
 * @tc.desc: Output line containing util-linux keyword returns true.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_HasUtilLinuxKeyword_Match_001, TestSize.Level1)
{
    std::vector<std::string> output = { "blkid from util-linux 2.42.2  (libblkid 2.42.2, 16-Jun-2026)" };
    EXPECT_TRUE(HasUtilLinuxKeyword(output));
}

/**
 * @tc.name: BlkidFormTest_HasUtilLinuxKeyword_NoMatch_001
 * @tc.desc: Output line without util-linux keyword returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_HasUtilLinuxKeyword_NoMatch_001, TestSize.Level1)
{
    std::vector<std::string> output = { "blkid 1.47.0 (18-May-2026)" };
    EXPECT_FALSE(HasUtilLinuxKeyword(output));
}

/**
 * @tc.name: BlkidFormTest_HasUtilLinuxKeyword_Empty_001
 * @tc.desc: Empty probe output returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_HasUtilLinuxKeyword_Empty_001, TestSize.Level1)
{
    std::vector<std::string> output;
    EXPECT_FALSE(HasUtilLinuxKeyword(output));
}

/**
 * @tc.name: BlkidFormTest_GetBlkidData_EmptyPath_001
 * @tc.desc: Empty devPath returns empty string without probing.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_GetBlkidData_EmptyPath_001, TestSize.Level1)
{
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).Times(0);
    EXPECT_EQ(GetBlkidData("", "TYPE"), "");
}

/**
 * @tc.name: BlkidFormTest_GetBlkidData_DashPath_001
 * @tc.desc: devPath starting with dash returns empty string without probing.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_GetBlkidData_DashPath_001, TestSize.Level1)
{
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _)).Times(0);
    EXPECT_EQ(GetBlkidData("-evil", "TYPE"), "");
}

/**
 * @tc.name: BlkidFormTest_GetBlkidData_DetectUtilLinuxThenQuery_001
 * @tc.desc: End to end, first call probes blkid -v; output contains "util-linux"
 *           keyword so query uses util-linux form and return value passes through.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BlkidFormTest, BlkidFormTest_GetBlkidData_DetectUtilLinuxThenQuery_001, TestSize.Level1)
{
    std::vector<std::string> probeCmd;
    std::vector<std::string> queryCmd;
    std::vector<std::string> versionOut = { "blkid from util-linux 2.42.2  (libblkid 2.42.2, 16-Jun-2026)" };
    std::vector<std::string> typeOut = { "ext4" };
    EXPECT_CALL(*fileUtilMoc_, ForkExec(_, _, _))
        .WillOnce(DoAll(SaveArg<0>(&probeCmd), SetArgPointee<1>(versionOut), Return(E_OK)))
        .WillOnce(DoAll(SaveArg<0>(&queryCmd), SetArgPointee<1>(typeOut), Return(E_OK)));
    EXPECT_EQ(GetBlkidData("/dev/block/sda1", "TYPE"), "ext4");
    EXPECT_EQ(probeCmd, (std::vector<std::string>{ "blkid", "-v" }));
    EXPECT_EQ(queryCmd, (std::vector<std::string>{
        "blkid", "-p", "-u", "noraid", "-s", "TYPE", "-o", "value", "/dev/block/sda1" }));
}
} // namespace StorageDaemon
} // namespace OHOS
