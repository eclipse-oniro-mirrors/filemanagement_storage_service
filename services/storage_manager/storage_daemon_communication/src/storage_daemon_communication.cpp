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
#include "storage_daemon_communication/storage_daemon_communication.h"
#include <system_ability_definition.h>
#include <iservice_registry.h>
#include "utils/storage_manager_log.h"
#include "utils/storage_manager_errno.h"
#include "ipc/istorage_daemon.h"
#include "ipc/storage_daemon.h"
#include "ipc/storage_daemon_proxy.h"

namespace OHOS {
namespace StorageManager {

StorageDaemonCommunication::StorageDaemonCommunication() 
{
    LOGI("DEBUG StorageDaemonCommunication constructer");
    storageDaemon_ = nullptr;
}

StorageDaemonCommunication::~StorageDaemonCommunication() 
{
    LOGI("DEBUG ~StorageDaemonCommunication destructer ~");
}

int32_t StorageDaemonCommunication::Connect() 
{
    if (storageDaemon_ != nullptr) {
        return E_OK;
    }
    int32_t err = 0;
    LOGI("StorageDaemonCommunication::Connect start");
    if (storageDaemon_ == nullptr) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            LOGE("StorageDaemonCommunication::Connect samgr nullptr");
            return E_IPC_ERROR;
        }
        auto object = sam->GetSystemAbility(STORAGE_MANAGER_DAEMON_ID); 
        if (object == nullptr) {
            LOGE("StorageDaemonCommunication::Connect object nullptr");
            return E_IPC_ERROR;
        }
        storageDaemon_ = iface_cast<OHOS::StorageDaemon::IStorageDaemon>(object);
        if (storageDaemon_ == nullptr) {
            LOGE("StorageDaemonCommunication::Connect service nullptr");
            return E_IPC_ERROR;
        }
    }
    LOGI("StorageDaemonCommunication::Connect end");
    return err;
}

int32_t StorageDaemonCommunication::OnUserCreate(int32_t userId) 
{
    LOGI("StorageDaemonCommunication::OnUserCreate start");
    
    if (Connect() != E_OK) {
        LOGE("StorageDaemonCommunication::OnUserCreate failed");
        return E_IPC_ERROR;
    } else {
        int err = storageDaemon_->AddUser(userId);
        if (err != E_OK) {
            LOGE("StorageDaemonCommunication::OnUserCreate call StorageDaemon AddUser failed");
        }
        return err;
    }
}

int32_t StorageDaemonCommunication::OnUserDelete(int32_t userId) 
{
    LOGI("StorageDaemonCommunication::OnUserDelete start");
    if (Connect() != E_OK) {
        LOGE("StorageDaemonCommunication::OnUserDelete failed");
        return E_IPC_ERROR;
    } else {
        int err = storageDaemon_->RemoveUser(userId);
        if (err != E_OK) {
            LOGE("StorageDaemonCommunication::OnUserDelete call StorageDaemon RemoveUser failed");
        }
        return err;
    }
}
} // StorageManager
} // OHOS