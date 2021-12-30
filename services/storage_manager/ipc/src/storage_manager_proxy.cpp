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

#include "ipc/storage_manager_proxy.h"
#include "utils/storage_manager_errno.h"
#include "utils/storage_manager_log.h"

namespace OHOS {
namespace StorageManager {

int32_t StorageManagerProxy::OnUserCreate(int32_t userId, uint32_t flags)
{   
    LOGI("StorageManagerProxy::OnUserCreate, userId:%{public}d", userId);
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageManagerProxy::GetDescriptor())) {
        LOGE("StorageManagerProxy::OnUserCreate, WriteInterfaceToken failed");
        return E_IPC_ERROR;
    }
    if (!data.WriteInt32(userId)) {
        LOGE("StorageManagerProxy::OnUserCreate, WriteInt32 failed");
        return E_IPC_ERROR;
    }
    if (!data.WriteUint32(flags)) {
        LOGE("StorageManagerProxy::OnUserCreate, WriteUint32 failed");
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(ON_USER_CREATE, data, reply, option);
    if (err != E_OK) {
        LOGE("StorageManagerProxy::OnUserCreate, SendRequest failed");
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}
    
int32_t StorageManagerProxy::OnUserDelete(int32_t userId, uint32_t flags)
{
    LOGI("StorageManagerProxy::OnUserDelete, userId:%{public}d", userId);
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageManagerProxy::GetDescriptor())) {
        LOGE("StorageManagerProxy::OnUserDelete, WriteInterfaceToken failed");
        return E_IPC_ERROR;
    }
    if (!data.WriteInt32(userId)) {
        LOGE("StorageManagerProxy::OnUserDelete, WriteInt32 failed");
        return E_IPC_ERROR;
    }
    if (!data.WriteUint32(flags)) {
        LOGE("StorageManagerProxy::OnUserCreate, WriteUint32 failed");
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(ON_USER_DELETE, data, reply, option);
    if (err != E_OK) {
        LOGE("StorageManagerProxy::OnUserDelete, SendRequest failed");
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}

int32_t StorageManagerProxy::PrepareUserStart(int32_t userId)
{
    LOGI("StorageManagerProxy::PrepareUserStart, userId:%{public}d", userId);
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageManagerProxy::GetDescriptor())) {
        LOGE("StorageManagerProxy::PrepareUserStart, WriteInterfaceToken failed");
        return E_IPC_ERROR;
    }
    if (!data.WriteInt32(userId)) {
        LOGE("StorageManagerProxy::PrepareUserStart, WriteInt32 failed");
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(PREPARE_USER_START, data, reply, option);
    if (err != E_OK) {
        LOGE("StorageManagerProxy::PrepareUserStart, SendRequest failed");
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}

int32_t StorageManagerProxy::PrepareUserStop(int32_t userId)
{
    LOGI("StorageManagerProxy::PrepareUserStop, userId:%{public}d", userId);
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(StorageManagerProxy::GetDescriptor())) {
        LOGE("StorageManagerProxy::PrepareUserStop, WriteInterfaceToken failed");
        return E_IPC_ERROR;
    }
    if (!data.WriteInt32(userId)) {
        LOGE("StorageManagerProxy::PrepareUserStop, WriteInt32 failed");
        return E_IPC_ERROR;
    }
    int err = Remote()->SendRequest(PREPARE_USER_STOP, data, reply, option);
    if (err != E_OK) {
        LOGE("StorageManagerProxy::PrepareUserStop, SendRequest failed");
        return E_IPC_ERROR;
    }
    return reply.ReadUint32();
}
} // StorageManager
} // OHOS

