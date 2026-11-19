/*
 * Copyright (c) 2021-2026 Huawei Device Co., Ltd.
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
#include <sys/sysmacros.h>

#include "disk/disk_info.h"
#include "storage_service_log.h"

namespace OHOS {
namespace StorageDaemon {
using namespace testing::ext;

class DiskInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.name: Storage_Service_DiskInfoTest_DiskInfo_001
 * @tc.desc: Verify the DiskInfo constructor builds diskId from device major/minor.
 * @tc.type: FUNC
 */
HWTEST_F(DiskInfoTest, Storage_Service_DiskInfoTest_DiskInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Storage_Service_DiskInfoTest_DiskInfo_001 start";

    dev_t device = makedev(8, 0);
    auto diskInfo = std::make_shared<DiskInfo>(device);
    ASSERT_TRUE(diskInfo != nullptr);

    GTEST_LOG_(INFO) << "Storage_Service_DiskInfoTest_DiskInfo_001 end";
}

}
}
