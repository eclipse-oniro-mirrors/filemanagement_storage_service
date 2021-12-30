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

#include "ipc/storage_manager_stub.h"
#include "utils/storage_manager_errno.h" 
#include "utils/storage_manager_log.h"

namespace OHOS {
namespace StorageManager {

int32_t StorageManagerStub::OnRemoteRequest(uint32_t code, 
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return E_PERMISSION_DENIED;
    }
    
    int err = 0;
    switch (code) {
        case ON_USER_CREATE: 
            HandleOnUserCreate(data, reply); 
            break;
        case ON_USER_DELETE: 
            HandleOnUserDelete(data, reply); 
            break;
        default: {
            LOGI(" use IPCObjectStub default OnRemoteRequest");
            err = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        } 
    }
    return err;
}

int32_t StorageManagerStub::HandleOnUserCreate(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = data.ReadInt32();
    uint32_t flags = data.ReadUint32();
    LOGI("StorageManagerStub::HandleOnUserCreate, userId:%{public}d", userId);
    int err = OnUserCreate(userId, flags);
    if (!reply.WriteUint32(err)) {
        LOGE("StorageManagerStub::HandleOnUserCreate call OnUserCreate failed");
        return  E_IPC_ERROR;
    }
    return E_OK;
}

int32_t StorageManagerStub::HandleOnUserDelete(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = data.ReadInt32();
    uint32_t flags = data.ReadUint32();
    LOGI("StorageManagerStub::HandleOnUserDelete, userId:%{public}d", userId);
    int err = OnUserDelete(userId, flags);
    if (!reply.WriteUint32(err)) {
        LOGE("StorageManagerStub::HandleOnUserDelete call OnUserDelete failed");
        return E_IPC_ERROR;
    }
    return E_OK;
}
} // StorageManager
} // OHOS