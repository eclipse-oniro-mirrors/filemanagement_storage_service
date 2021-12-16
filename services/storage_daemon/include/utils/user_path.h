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
#ifndef STORAGE_DAEMON_UTILS_USER_PATH_H
#define STORAGE_DAEMON_UTILS_USER_PATH_H
#include <string>

namespace OHOS {
namespace StorageDaemon {

struct DirInfo {
        mode_t mode;
        uid_t uid;
        gid_t gid;
};

extern std::unordered_map<std::string, struct DirInfo> g_el1DirMap;
extern std::unordered_map<std::string, struct DirInfo> g_el2DirMap;

// top level userdata path
const std::string DATA = "/data/";
// app/service/vendor path
const std::string DATA_APP = DATA + "app/";
const std::string DATA_SERVICE = DATA + "service/";
const std::string DATA_VENDOR = DATA + "vendor/";

// app data path
const std::string DATA_APP_EL1 = DATA_APP + "el1/";
const std::string DATA_APP_EL2 = DATA_APP + "el2/";

// service data path
const std::string DATA_SERVICE_EL1 = DATA_SERVICE + "el1/";
const std::string DATA_SERVICE_EL2 = DATA_SERVICE + "el2/";

// vendor data path
const std::string DATA_VENDOR_EL1 = DATA_VENDOR + "el1/";
const std::string DATA_VENDOR_EL2 = DATA_VENDOR + "el2/";

// mount path
const std::string HMDFS_FILES = "/hmdfs/files/";
const std::string STORAGE_MEDIA = "/storage/media/";
const std::string LOCAL = "/local/";
}
}
#endif // STORAGE_DAEMON_UTILS_USER_PATH_H