/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_STORAGE_DAEMON_USER_INFO_H
#define OHOS_STORAGE_DAEMON_USER_INFO_H

namespace OHOS {
namespace StorageDaemon {
enum UserState {
    USER_CREAT,
    USER_START,
    USER_STOP,
};

class UserInfo {
    int32_t userId;
    UserState state;

    UserInfo(int32_t id, UserState state)
    {
        this->userId = id;
        this->state = state;
    }

    UserInfo(const UserInfo& userInfo)
    {
        this->userId = userInfo.userId;
        this->state = userInfo.state;
    }
};
} // STORAGE_DAEMON
} // OHOS

#endif // OHOS_STORAGE_DAEMON_USER_INFO_H