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

#include "ipc/storage_daemon_proxy.h"
#include "utils/errno.h"

namespace OHOS {
namespace StorageDaemon {
StorageDaemonProxy::StorageDaemonProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IStorageDaemon>(impl)
{}

int32_t StorageDaemonProxy::Shutdown()
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        return E_IPC_ERROR;
    }

    return Remote()->SendRequest(SHUTDOWN, data, reply, option);
}

int32_t StorageDaemonProxy::Mount(string volId)
{

}

int32_t StorageDaemonProxy::UMount(string volId)
{

}

int32_t StorageDaemonProxy::Check(string volId)
{

}

int32_t StorageDaemonProxy::Format(string voldId)
{

}

int32_t StorageDaemonProxy::AddUser(int32_t userId)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        return E_IPC_ERROR;
    }

    if (!data.WriteInt32(userId)) {
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(ADD_USER, data, reply, option);
    if (err != E_OK) {
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}

int32_t StorageDaemonProxy::RemoveUser(int32_t userId)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        return E_IPC_ERROR;
    }

    if (!data.WriteInt32(userId)) {
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(REMOVE_USER, data, reply, option);
    if (err != E_OK) {
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}

int32_t StorageDaemonProxy::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        return E_IPC_ERROR;
    }

    if (!data.WriteInt32(userId)) {
        return E_IPC_ERROR;
    }
    if (!data.WriteUint32(flags)) {
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(PREPARE_USER_DIRS, data, reply, option);
    if (err != E_OK) {
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}

int32_t StorageDaemonProxy::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageServiceCallbackProxy::GetDescriptor())) {
        return E_IPC_ERROR;
    }

    if (!data.WriteInt32(userId)) {
        return E_IPC_ERROR;
    }
    if (!data.WriteUint32(flags)) {
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(DESTROY_USER_DIRS, data, reply, option);
    if (err != E_OK) {
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}

} // StorageDaemon
} // OHOS