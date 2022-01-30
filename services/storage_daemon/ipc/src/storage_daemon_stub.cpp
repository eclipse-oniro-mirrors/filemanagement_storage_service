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

#include "ipc/storage_daemon_stub.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"

namespace OHOS {
namespace StorageDaemon {
int32_t StorageDaemonStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return E_PERMISSION_DENIED;
    }

    LOGI("recv remote request code %{public}d", code);
    int err = E_OK;
    switch (code) {
        case SHUTDOWN:
            err = HandleShutdown();
            break;
        case PREPARE_USER_DIRS:
            err = HandlePrepareUserDirs(data, reply);
            break;
        case DESTROY_USER_DIRS:
            err = HandleDestroyUserDirs(data, reply);
            break;
        case START_USER:
            err = HandleStartUser(data,  reply);
            break;
        case STOP_USER:
            err = HandleStopUser(data, reply);
            break;
        case INIT_GLOBAL_KEY:
            err = HandleInitGlobalKey(data, reply);
            break;
        case INIT_GLOBAL_USER_KEYS:
            err = HandleInitGlobalUserKeys(data, reply);
            break;
        case CREATE_USER_KEYS:
            err = HandleGenerateUserKeys(data, reply);
            break;
        case DELETE_USER_KEYS:
            err = HandleDeleteUserKeys(data, reply);
            break;
        case UPDATE_USER_AUTH:
            err = HandleUpdateUserAuth(data, reply);
            break;
        case ACTIVE_USER_KEY:
            err = HandleActiveUserKey(data, reply);
            break;
        case INACTIVE_USER_KEY:
            err = HandleInactiveUserKey(data, reply);
            break;
        default: {
            LOGI(" use IPCObjectStub default OnRemoteRequest");
            err = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return err;
}

int32_t StorageDaemonStub::HandleShutdown()
{
    Shutdown();
    return E_OK;
}

int32_t StorageDaemonStub::HandleMount(MessageParcel &data, MessageParcel &reply)
{
    return E_OK;
}

int32_t StorageDaemonStub::HandleUMount(MessageParcel &data, MessageParcel &reply)
{
    return E_OK;
}

int32_t StorageDaemonStub::HandleCheck(MessageParcel &data, MessageParcel &reply)
{
    return E_OK;
}

int32_t StorageDaemonStub::HandleFormat(MessageParcel &data, MessageParcel &reply)
{
    return E_OK;
}

int32_t StorageDaemonStub::HandlePrepareUserDirs(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = data.ReadInt32();
    uint32_t flags = data.ReadUint32();

    int err = PrepareUserDirs(userId, flags);
    if (!reply.WriteUint32(err)) {
        return  E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleDestroyUserDirs(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = data.ReadInt32();
    uint32_t flags = data.ReadUint32();

    int err = DestroyUserDirs(userId, flags);
    if (!reply.WriteUint32(err)) {
        return  E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleStartUser(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = data.ReadInt32();

    int32_t err = StartUser(userId);
    if (!reply.WriteInt32(err)) {
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleStopUser(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = data.ReadInt32();

    int32_t err = StopUser(userId);
    if (!reply.WriteInt32(err)) {
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleInitGlobalKey(MessageParcel &data, MessageParcel &reply)
{
    int err = InitGlobalKey();
    if (!reply.WriteInt32(err)){
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleInitGlobalUserKeys(MessageParcel &data, MessageParcel &reply)
{
    int err = InitGlobalUserKeys();
    if (!reply.WriteInt32(err)){
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleGenerateUserKeys(MessageParcel &data, MessageParcel &reply)
{
    uint32_t userId = data.ReadUint32();
    uint32_t flags = data.ReadUint32();
    int err = GenerateUserKeys(userId, flags);
    if (!reply.WriteInt32(err)) {
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleDeleteUserKeys(MessageParcel &data, MessageParcel &reply)
{
    uint32_t userId = data.ReadUint32();
    int err = DeleteUserKeys(userId);
    if (!reply.WriteInt32(err)) {
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleUpdateUserAuth(MessageParcel &data, MessageParcel &reply)
{
    uint32_t userId = data.ReadUint32();
    std::string auth = "";
    std::string secret = "";
    int err = UpdateUserAuth(userId, auth, secret);
    if (!reply.WriteInt32(err)) {
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleActiveUserKey(MessageParcel &data, MessageParcel &reply)
{
    uint32_t userId = data.ReadUint32();
    std::string auth = "";
    std::string secret = "";
    int err = ActiveUserKey(userId, auth, secret);
    if (!reply.WriteInt32(err)) {
        return E_IPC_ERROR;
    }

    return E_OK;
}

int32_t StorageDaemonStub::HandleInactiveUserKey(MessageParcel &data, MessageParcel &reply)
{
    uint32_t userId = data.ReadUint32();
    int err = InactiveUserKey(userId);
    if (!reply.WriteInt32(err)) {
        return E_IPC_ERROR;
    }

    return E_OK;
}
} // StorageDaemon
} // OHOS