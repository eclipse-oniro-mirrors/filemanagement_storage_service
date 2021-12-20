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

#ifndef OHOS_STORAGE_DAEMON_ISTORAGE_DAEMON_H
#define OHOS_STORAGE_DAEMON_ISTORAGE_DAEMON_H

#include "iremote_broker.h"
#include <string>

namespace OHOS {
namespace StorageDaemon {
class IStorageDaemon : public IRemoteBroker {
public:
    enum {
        SHUTDOWN = 1,

        MOUNT,
        UMOUNT,
        CHECK,
        FORMAT,

        ADD_USER,
        REMOVE_USER,
        PREPARE_USER_DIRS,
        DESTROY_USER_DIRS,
    };

    enum {
        CRYPTO_FLAG_EL1 = 1,
        CRYPTO_FLAG_EL2,
    };

    virtual int32_t Shutdown() = 0;

    virtual int32_t Mount(std::string volId, uint32_t flags) = 0;
    virtual int32_t UMount(std::string volId) = 0;
    virtual int32_t Check(std::string volId) = 0;
    virtual int32_t Format(std::string voldId) = 0;

    virtual int32_t AddUser(int32_t userId) = 0;
    virtual int32_t RemoveUser(int32_t userId) = 0;
    virtual int32_t PrepareUserDirs(int32_t userId, uint32_t flags) = 0;
    virtual int32_t DestroyUserDirs(int32_t userId, uint32_t flags) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.StorageDaemon");
};
} // STORAGE_DAEMON
} // OHOS

#endif // OHOS_STORAGE_DAEMON_ISTORAGE_DAEMON_H
