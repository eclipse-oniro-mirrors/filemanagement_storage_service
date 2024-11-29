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
#ifndef STORAGE_SERVICE_UTILS_ERRNO_H
#define STORAGE_SERVICE_UTILS_ERRNO_H

#include <sys/types.h>
#include <unordered_map>

namespace OHOS {
constexpr int32_t E_ERR = -1;
constexpr int32_t STORAGE_SERVICE_SYS_CAP_TAG = 13600000;

enum ErrNo {
    E_OK = 0,
    E_PERMISSION_DENIED = 1,    // no permission
    E_WRITE_DESCRIPTOR_ERR = 2, // ipc error
    E_EXIST = 3,                // exist
    E_WRONG_TYPE = 4,           // wrong file type
    E_USER_STATE = 5,           // wrong user state
    E_NON_EXIST = 6,            // no such item
    E_PREPARE_DIR = 7,          // failed to prepare dir
    E_DESTROY_DIR = 8,          // failed to destroy dir
    E_MOUNT,                    // mount error
    E_UMOUNT,                   // umount error
    E_SET_POLICY,               // set policy error
    E_USERID_RANGE,             // id out or range
    E_VOL_STATE,                // volume state check fail
    E_WAIT,                     // wait
    E_NOT_SUPPORT,              // not support
    E_SYS_CALL,                 // syscall error
    E_NO_CHILD,                 // child not exist
    E_WRITE_PARCEL_ERR,
    E_WRITE_REPLY_ERR,
    E_SA_IS_NULLPTR,
    E_REMOTE_IS_NULLPTR,
    E_SERVICE_IS_NULLPTR,
    E_DEATH_RECIPIENT_IS_NULLPTR,
    E_BUNDLEMGR_ERROR,
    E_MEDIALIBRARY_ERROR,
    E_SYS_ERR,
    E_KEY_TYPE_INVAL,
    E_DIFF_UID_GID,
    E_PARAMS_INVAL,
    E_ACTIVE_EL2_FAILED,
    E_CLEAR_KEY_FAILED,
    E_KEY_NOT_ACTIVED,
    E_RESTORE_KEY_FAILED,
    E_UNLOCK_SCREEN_FAILED,
    E_UNLOCK_APP_KEY2_FAILED,
    E_MTP_IS_MOUNTING,
    E_MTP_PREPARE_DIR_ERR,
    E_MTP_MOUNT_FAILED,
    E_MTP_UMOUNT_FAILED,
    E_MIGRETE_ELX_FAILED,
    
    E_GLOBAL_KEY_NULLPTR = 200,
    E_GLOBAL_KEY_INIT_ERROR,
    E_GLOBAL_KEY_STORE_ERROR,
    E_GLOBAL_KEY_ACTIVE_ERROR,
    E_GLOBAL_KEY_UPDATE_ERROR,
    E_ELX_KEY_INIT_ERROR,
    E_ELX_KEY_STORE_ERROR,
    E_ELX_KEY_ACTIVE_ERROR,
    E_ELX_KEY_UPDATE_ERROR,
    E_EL5_ADD_CLASS_ERROR,
    E_EL5_ENCRYPT_CLASS_ERROR,
    E_ELX_KEY_INACTIVE_ERROR,
};

enum JsErrCode {
    E_PERMISSION = 201,
    E_PERMISSION_SYS = 202,
    E_PARAMS = 401,
    E_DEVICENOTSUPPORT = 801,
    E_OSNOTSUPPORT = 901,
    E_IPCSS = STORAGE_SERVICE_SYS_CAP_TAG + 1,
    E_SUPPORTEDFS,
    E_MOUNT_ERR,
    E_UNMOUNT,
    E_VOLUMESTATE,
    E_PREPARE,
    E_DELETE,
    E_NOOBJECT,
    E_OUTOFRANGE,
};

}
#endif // STORAGE_SERVICE_UTILS_ERRNO_H