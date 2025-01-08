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
    // 历史错误码（为保证兼容，禁止修改或新增）
    E_OK = 0,
    E_ACTIVE_EL2_FAILED = 30,

    // 通用错误码 13600001 ~ 13600200
    E_PERMISSION_DENIED = STORAGE_SERVICE_SYS_CAP_TAG + 1,
    E_PARAMS_INVALID = STORAGE_SERVICE_SYS_CAP_TAG + 2,
    E_PARAMS_NULLPTR_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 3,
    E_NON_EXIST = STORAGE_SERVICE_SYS_CAP_TAG + 4,
    E_PREPARE_DIR = STORAGE_SERVICE_SYS_CAP_TAG + 5,
    E_DESTROY_DIR = STORAGE_SERVICE_SYS_CAP_TAG + 6,
    E_NOT_SUPPORT = STORAGE_SERVICE_SYS_CAP_TAG + 7,
    E_SYS_KERNEL_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 8,
    E_NO_CHILD = STORAGE_SERVICE_SYS_CAP_TAG + 9,
    E_WRITE_PARCEL_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 10,
    E_WRITE_REPLY_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 11,
    E_WRITE_DESCRIPTOR_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 12,
    E_SA_IS_NULLPTR = STORAGE_SERVICE_SYS_CAP_TAG + 13,
    E_REMOTE_IS_NULLPTR = STORAGE_SERVICE_SYS_CAP_TAG + 14,
    E_SERVICE_IS_NULLPTR = STORAGE_SERVICE_SYS_CAP_TAG + 15,
    E_MEMORY_OPERATION_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 16,

    // 密钥管理 13600201 ~ 13600700
    E_SET_POLICY = STORAGE_SERVICE_SYS_CAP_TAG + 201,
    E_USERID_RANGE = STORAGE_SERVICE_SYS_CAP_TAG + 202,
    E_KEY_TYPE_INVAL = STORAGE_SERVICE_SYS_CAP_TAG + 203,
    E_MIGRETE_ELX_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 204,
    E_GLOBAL_KEY_NULLPTR = STORAGE_SERVICE_SYS_CAP_TAG + 205,
    E_GLOBAL_KEY_INIT_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 206,
    E_GLOBAL_KEY_STORE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 207,
    E_GLOBAL_KEY_ACTIVE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 208,
    E_GLOBAL_KEY_UPDATE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 209,
    E_ELX_KEY_INIT_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 210,
    E_ELX_KEY_STORE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 211,
    E_ELX_KEY_ACTIVE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 212,
    E_ELX_KEY_UPDATE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 213,
    E_EL5_ADD_CLASS_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 214,
    E_EL5_ENCRYPT_CLASS_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 215,
    E_ELX_KEY_INACTIVE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 216,
    E_CLEAR_KEY_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 217,
    E_KEY_NOT_ACTIVED = STORAGE_SERVICE_SYS_CAP_TAG + 218,
    E_RESTORE_KEY_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 219,
    E_UNLOCK_SCREEN_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 220,
    E_UNLOCK_APP_KEY2_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 221,
    E_JSON_PARSE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 222,
    E_OPEN_JSON_FILE_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 223,
    E_FILE_VERIFY_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 224,
    E_HCF_OPERATION_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 225,

    // 用户管理 13600701 ~ 13601200
    E_USER_MOUNT_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 701,
    E_USER_UMOUNT_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 702,
    E_DIFF_UID_GID = STORAGE_SERVICE_SYS_CAP_TAG + 703,
    E_UMOUNT_CLOUD_FUSE = STORAGE_SERVICE_SYS_CAP_TAG + 704,
    E_UMOUNT_CLOUD = STORAGE_SERVICE_SYS_CAP_TAG + 705,
    E_UMOUNT_MEDIA_FUSE = STORAGE_SERVICE_SYS_CAP_TAG + 706,
    E_UMOUNT_SHAREFS = STORAGE_SERVICE_SYS_CAP_TAG + 707,
    E_UMOUNT_HMFS = STORAGE_SERVICE_SYS_CAP_TAG + 708,
    E_UMOUNT_HMDFS = STORAGE_SERVICE_SYS_CAP_TAG + 709,
    E_UMOUNT_LOCAL = STORAGE_SERVICE_SYS_CAP_TAG + 710,
    E_UMOUNT_PROC_MOUNTS_OPEN = STORAGE_SERVICE_SYS_CAP_TAG + 711,
    E_UMOUNT_ALL_PATH = STORAGE_SERVICE_SYS_CAP_TAG + 712,
    E_UMOUNT_PROC_OPEN = STORAGE_SERVICE_SYS_CAP_TAG + 713,
    E_UMOUNT_DETACH = STORAGE_SERVICE_SYS_CAP_TAG + 714,
    E_UMOUNT_NO_PROCESS_FIND = STORAGE_SERVICE_SYS_CAP_TAG + 715,
    E_UMOUNT_PROCESS_KILL = STORAGE_SERVICE_SYS_CAP_TAG + 716,
    E_UMOUNT_FIND_FD = STORAGE_SERVICE_SYS_CAP_TAG + 717,
    E_NOT_EMPTY_TO_UMOUNT = STORAGE_SERVICE_SYS_CAP_TAG + 718,

    // 空间统计 13601201 ~ 13601700
    E_BUNDLEMGR_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 1201,
    E_MEDIALIBRARY_ERROR = STORAGE_SERVICE_SYS_CAP_TAG + 1202,
    E_STORAGE_USAGE_NOT_ENOUGH = STORAGE_SERVICE_SYS_CAP_TAG + 1203,

    // 外卡管理 13601701 ~ 13602200
    E_VOL_STATE = STORAGE_SERVICE_SYS_CAP_TAG + 1701,
    E_VOL_MOUNT_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 1702,
    E_VOL_UMOUNT_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 1703,
    E_UMOUNT_BUSY = STORAGE_SERVICE_SYS_CAP_TAG + 1704,
    E_MTP_IS_MOUNTING = STORAGE_SERVICE_SYS_CAP_TAG + 1705,
    E_MTP_PREPARE_DIR_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 1706,
    E_MTP_MOUNT_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 1707,
    E_MTP_UMOUNT_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 1708,

    // 文件分享/分布式文件/备份扫描等 13602201 ~
    E_CREATE_SHARE_FILE_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 2201,
    E_DELETE_SHARE_FILE_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 2202,
    E_INIT_QUOTA_MOUNTS_FAILED = STORAGE_SERVICE_SYS_CAP_TAG + 2203,
    E_QUOTA_CTL_KERNEL_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 2204,
    E_STAT_VFS_KERNEL_ERR = STORAGE_SERVICE_SYS_CAP_TAG + 2205,
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