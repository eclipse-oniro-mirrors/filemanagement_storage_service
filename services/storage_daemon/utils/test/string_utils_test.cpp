/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "string_utils.h"

#include <gtest/gtest.h>
#include <tuple>
#include <climits>
#include <cstdio>
#include <fstream>

#include "directory_ex.h"
#include "storage_service_constant.h"
#include "storage_service_constants.h"

namespace OHOS {
namespace StorageDaemon {
namespace Test {
using namespace testing;
using namespace testing::ext;
class StringUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
};

namespace {
const std::string WRITE_FILE_SYNC_TEST_PATH = "/data/service/string_utils_write_test.txt";
const std::string WRITE_FILE_SYNC_INVALID_PATH = "/data/service/not_exist_dir/string_utils_write_test.txt";
}

/**
 * @tc.name: UserPathResolverTest_ConvertStringToInt_001
 * @tc.desc: Verify the ConvertStringToInt.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StringUtilsTest, UserPathResolverTest_ConvertStringToInt_001, TestSize.Level1)
{
    std::map <std::string, std::pair<bool, int64_t>> testMap{
        {"12345", {true, 12345}},
        {"", {false, 0}},
        {"abc", {false, 0}},
        {"123abc", {false, 0}},
        {"9223372036854775807", {true, LLONG_MAX}},
        {"-9223372036854775808", {true, LLONG_MIN}},
        {"9223372036854775808", {false, 0}},
        {"-9223372036854775809", {false, 0}},
        {" 12345", {true, 12345}},
    };
    for (const auto &test : testMap) {
        std::cout << "test case: " << test.first << std::endl;
        int64_t value = 0;
        EXPECT_EQ(ConvertStringToInt(test.first, value, BASE_DECIMAL), test.second.first);
        EXPECT_EQ(value, test.second.second);
    }
}

/**
 * @tc.name: UserPathResolverTest_ParseKeyValuePairs_001
 * @tc.desc: Verify the ParseKeyValuePairs.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StringUtilsTest, UserPathResolverTest_ParseKeyValuePairs_001, TestSize.Level1)
{
    std::map <std::string, std::unordered_map<std::string, std::string>> testMap {
        {"key1=value1,key2=value2", {{"key1", "value1"}, {"key2", "value2"}}},
        {"key1,key2", {{"key1", ""}, {"key2", ""}}},
        {"", {}},
        {"key=value", {{"key", "value"}}},
        {"key1=,key2=value2", {{"key1", ""}, {"key2", "value2"}}},
        {"=value1,key2=value2", {{"key2", "value2"}}},
        {",,key2=value2", {{"key2", "value2"}}},
        {"key=value1,key=value2", {{"key", "value2"}}},
        {"=,key2=", {{"key2", ""}}},
        {" key1 = value1 , key2 = value2 ", {{" key1 ", " value1 "}, {" key2 ", " value2 "}}},
        {"key1=value1!@#$%^&*(),key2=value2", {{"key1", "value1!@#$%^&*()"}, {"key2", "value2"}}}
    };
    for (const auto &test : testMap) {
        std::cout << "test case: " << test.first << std::endl;
        auto ret = ParseKeyValuePairs(test.first, ',');
        EXPECT_EQ(ret, test.second);
    }
}

/**
 * @tc.name: UserPathResolverTest_ReplaceAndCount_001
 * @tc.desc: Verify the ReplaceAndCount.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StringUtilsTest, UserPathResolverTest_ReplaceAndCount_001, TestSize.Level1)
{
    std::map <std::tuple<std::string, std::string, std::string>, std::pair<std::string, int32_t>> testMap {
        {{"hello world, hello universe", "hello", "hi"}, {"hi world, hi universe", 2}},
        {{"", "hello", "hi"}, {"", 0}},
        {{"hello world", "", "hi"}, {"hello world", 0}},
        {{"hello world, hello universe", "hello", ""}, {" world,  universe", 2}},
        {{"hello world", "universe", "hi"}, {"hello world", 0}},
        {{"hello world", "hello", "hello"}, {"hello world", 1}},
        {{"hello world, hello", "hello", "hi"}, {"hi world, hi", 2}},
        {{"hello", "hello", "hi"}, {"hi", 1}},
        {{"hellohello", "hello", "hi"}, {"hihi", 2}},
        {{"hello@world, hello@universe", "@world", "@galaxy"}, {"hello@galaxy, hello@universe", 1}},
        {{"hello world, hello universe", "hello", "hellohello"}, {"hellohello world, hellohello universe", 2}},
    };
    for (const auto &test : testMap) {
        auto str = std::get<0>(test.first);
        std::cout << "test case: " << str << std::endl;
        auto count = ReplaceAndCount(str, std::get<1>(test.first), std::get<2>(test.first));
        EXPECT_EQ(count, test.second.second);
        EXPECT_EQ(str, test.second.first);
    }
}

/**
 * @tc.name: StringUtilsTest_ConvertStringToInt32_001
 * @tc.desc: Verify the ConvertStringToInt32.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_ConvertStringToInt32_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_ConvertStringToInt32_001 start";
    int32_t value = 0;
    std::string context;
    bool ret = ConvertStringToInt32(context, value);
    ASSERT_FALSE(ret);

    context = "test";
    ret = ConvertStringToInt32(context, value);
    ASSERT_FALSE(ret);

    context = "9999999999";
    ret = ConvertStringToInt32(context, value);
    ASSERT_FALSE(ret);

    context = "20000000";
    ret = ConvertStringToInt32(context, value);
    ASSERT_TRUE(ret);
    GTEST_LOG_(INFO) << "StringUtilsTest_ConvertStringToInt32_001 end";
}

/**
 * @tc.name: StringUtilsTest_WriteFileSync_001
 * @tc.desc: Verify WriteFileSync success and failure branches.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_WriteFileSync_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_WriteFileSync_001 start";
    std::string errMsg;
    const std::string content = "write file sync content";
    const uint8_t *data = reinterpret_cast<const uint8_t *>(content.c_str());

    EXPECT_FALSE(WriteFileSync(WRITE_FILE_SYNC_INVALID_PATH.c_str(), data, content.size(), errMsg));
    EXPECT_FALSE(errMsg.empty());

    std::ofstream out(WRITE_FILE_SYNC_TEST_PATH, std::ios::trunc);
    ASSERT_TRUE(out.is_open());
    out.close();

    errMsg.clear();
    EXPECT_TRUE(WriteFileSync(WRITE_FILE_SYNC_TEST_PATH.c_str(), data, content.size(), errMsg));
    EXPECT_EQ(std::remove(WRITE_FILE_SYNC_TEST_PATH.c_str()), 0);
    GTEST_LOG_(INFO) << "StringUtilsTest_WriteFileSync_001 end";
}

/**
 * @tc.name: StringUtilsTest_SaveStringToFileSync_001
 * @tc.desc: Verify SaveStringToFileSync branches for invalid input, write fail and write success.
 * @tc.type: FUNC
 * @tc.require: AR000H09L6
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_SaveStringToFileSync_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_SaveStringToFileSync_001 start";
    std::string errMsg;
    const std::string content = "save string to file sync content";

    EXPECT_FALSE(SaveStringToFileSync("", content, errMsg));
    EXPECT_FALSE(SaveStringToFileSync(WRITE_FILE_SYNC_TEST_PATH, "", errMsg));
    EXPECT_FALSE(SaveStringToFileSync(WRITE_FILE_SYNC_INVALID_PATH, content, errMsg));

    EXPECT_TRUE(SaveStringToFileSync(WRITE_FILE_SYNC_TEST_PATH, content, errMsg));
    EXPECT_EQ(std::remove(WRITE_FILE_SYNC_TEST_PATH.c_str()), 0);
    GTEST_LOG_(INFO) << "StringUtilsTest_SaveStringToFileSync_001 end";
}

/**
 * @tc.name: StringUtilsTest_CheckLevelRange_001
 * @tc.desc: Verify CheckLevelRange success and failure branches.
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_CheckLevelRange_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckLevelRange_001 start";
    EXPECT_TRUE(CheckLevelRange(StorageService::EL1_SYS_KEY));
    EXPECT_TRUE(CheckLevelRange(StorageService::EL4_USER_KEY));
    EXPECT_TRUE(CheckLevelRange((StorageService::EL1_SYS_KEY + StorageService::EL4_USER_KEY) / 2));

    EXPECT_FALSE(CheckLevelRange(StorageService::EL1_SYS_KEY - 1));
    EXPECT_FALSE(CheckLevelRange(StorageService::EL5_USER_KEY + 1));
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckLevelRange_001 end";
}

/**
 * @tc.name: StringUtilsTest_CheckInputListRange_001
 * @tc.desc: Verify CheckInputListRange success and failure branches.
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_CheckInputListRange_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckInputListRange_001 start";
    std::vector<std::string> validList = {"item1", "item2"};
    EXPECT_TRUE(CheckInputListRange(validList));
    EXPECT_TRUE(CheckInputListRange({"singleItem"}));

    std::vector<std::string> emptyList;
    EXPECT_FALSE(CheckInputListRange(emptyList));
    
    std::vector<std::string> largeList(50001, "item");
    EXPECT_FALSE(CheckInputListRange(largeList));
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckInputListRange_001 end";
}

/**
 * @tc.name: StringUtilsTest_CheckLocalIdListRange_001
 * @tc.desc: Verify CheckLocalIdListRange success and failure branches.
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_CheckLocalIdListRange_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckLocalIdListRange_001 start";
    std::vector<int32_t> validList = {1, 2, 3};
    EXPECT_TRUE(CheckLocalIdListRange(validList));
    EXPECT_TRUE(CheckLocalIdListRange({100}));

    std::vector<int32_t> emptyList;
    EXPECT_FALSE(CheckLocalIdListRange(emptyList));
    
    std::vector<int32_t> largeList(101, 1);
    EXPECT_FALSE(CheckLocalIdListRange(largeList));
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckLocalIdListRange_001 end";
}

/**
 * @tc.name: StringUtilsTest_CheckIdRange_001
 * @tc.desc: Verify CheckIdRange success and failure branches.
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_CheckIdRange_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckIdRange_001 start";
    EXPECT_TRUE(CheckIdRange("abc123"));
    EXPECT_TRUE(CheckIdRange("ABC123"));
    EXPECT_TRUE(CheckIdRange("123456"));
    EXPECT_TRUE(CheckIdRange("a1b2c3d4e5f6g7h8i9j0"));
    EXPECT_TRUE(CheckIdRange("A1B2C3D4E5F6G7H8I9J0"));
    EXPECT_TRUE(CheckIdRange("aBcDeFgHiJkLmNoPqRsTuVwXyZ0123456"));

    EXPECT_FALSE(CheckIdRange(""));
    EXPECT_FALSE(CheckIdRange("abc@123"));
    EXPECT_FALSE(CheckIdRange("abc 123"));
    EXPECT_FALSE(CheckIdRange("abc123!"));

    std::string longId(66, 'a');
    EXPECT_FALSE(CheckIdRange(longId));
    GTEST_LOG_(INFO) << "StringUtilsTest_CheckIdRange_001 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_001
 * @tc.desc: Verify IsUdmfClipPathValid returns true for valid udmf_fuse path.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_001 start";
    int32_t userId = 100;
    std::string path = "/mnt/data/100/udmf_fuse/test_valid";
    OHOS::ForceCreateDirectory(path);
    EXPECT_TRUE(IsUdmfClipPathValid(userId, path));
    OHOS::ForceRemoveDirectory(path);
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_001 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_002
 * @tc.desc: Verify IsUdmfClipPathValid returns true for valid clipboard_fuse path.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_002 start";
    int32_t userId = 100;
    std::string path = "/mnt/data/100/clipboard_fuse/test_valid";
    OHOS::ForceCreateDirectory(path);
    EXPECT_TRUE(IsUdmfClipPathValid(userId, path));
    OHOS::ForceRemoveDirectory(path);
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_002 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_003
 * @tc.desc: Verify IsUdmfClipPathValid returns false for mismatched prefix.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_003 start";
    int32_t userId = 100;
    std::string path = "/mnt/data/100/cloud_disk_fuse/test_mismatch";
    OHOS::ForceCreateDirectory(path);
    EXPECT_FALSE(IsUdmfClipPathValid(userId, path));
    OHOS::ForceRemoveDirectory(path);
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_003 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_004
 * @tc.desc: Verify IsUdmfClipPathValid returns false when path does not exist.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_004 start";
    int32_t userId = 100;
    std::string path = "/mnt/data/100/udmf_fuse/nonexistent_path_12345";
    EXPECT_FALSE(IsUdmfClipPathValid(userId, path));
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_004 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_005
 * @tc.desc: Verify IsUdmfClipPathValid returns false when userId in path does not match.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_005 start";
    int32_t userId = 100;
    std::string path = "/mnt/data/101/udmf_fuse/test_uid_mismatch";
    OHOS::ForceCreateDirectory(path);
    EXPECT_FALSE(IsUdmfClipPathValid(userId, path));
    OHOS::ForceRemoveDirectory(path);
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_005 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_006
 * @tc.desc: Verify IsUdmfClipPathValid returns false when realpath differs from dstPath (extra slash).
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_006 start";
    int32_t userId = 100;
    std::string canonicalPath = "/mnt/data/100/udmf_fuse/test_realpath_diff";
    OHOS::ForceCreateDirectory(canonicalPath);
    // Add an extra slash so realpath() resolves to a different string than dstPath
    std::string path = "/mnt/data/100/udmf_fuse//test_realpath_diff";
    EXPECT_FALSE(IsUdmfClipPathValid(userId, path));
    OHOS::ForceRemoveDirectory(canonicalPath);
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_006 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_007
 * @tc.desc: Verify IsUdmfClipPathValid returns false when realpath differs from dstPath (dot segment).
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_007 start";
    int32_t userId = 100;
    std::string canonicalPath = "/mnt/data/100/clipboard_fuse/test_dot_segment";
    OHOS::ForceCreateDirectory(canonicalPath);
    // Use a "." segment so realpath() resolves to a different string than dstPath
    std::string path = "/mnt/data/100/clipboard_fuse/./test_dot_segment";
    EXPECT_FALSE(IsUdmfClipPathValid(userId, path));
    OHOS::ForceRemoveDirectory(canonicalPath);
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_007 end";
}

/**
 * @tc.name: StringUtilsTest_IsUdmfClipPathValid_008
 * @tc.desc: Verify IsUdmfClipPathValid returns false for empty path.
 * @tc.type: FUNC
 * @tc.require: 20260807938859
 */
HWTEST_F(StringUtilsTest, StringUtilsTest_IsUdmfClipPathValid_008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_008 start";
    int32_t userId = 100;
    EXPECT_FALSE(IsUdmfClipPathValid(userId, ""));
    GTEST_LOG_(INFO) << "StringUtilsTest_IsUdmfClipPathValid_008 end";
}
} // Test
} // STORAGE_DAEMON
} // OHOS

