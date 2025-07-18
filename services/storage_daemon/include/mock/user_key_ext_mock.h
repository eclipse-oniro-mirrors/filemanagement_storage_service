/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef STORAGE_DAEMON_USER_KEY_EXT_MOCK_H
#define STORAGE_DAEMON_USER_KEY_EXT_MOCK_H

#include <gmock/gmock.h>
#include <memory>
#include <vector>

#include "key_manager_ext.h"

namespace OHOS {
namespace StorageDaemon {
class UserkeyExtMoc : public UserkeyExtInterface {
public:
    MOCK_METHOD2(GenerateUserKey, int32_t(int32_t userId, const std::vector<uint8_t>& keyInfo));
    MOCK_METHOD3(ActiveUserKey, int32_t(int32_t userId, const std::vector<uint8_t>& keyInfo,
        const std::vector<uint8_t>& token));
    MOCK_METHOD1(InactiveUserKey, int32_t(int32_t userId));
    MOCK_METHOD1(DeleteUserKey, int32_t(int32_t userId));
    MOCK_METHOD1(SetFilePathPolicy, int32_t(int32_t userId));
};
}
}
#endif