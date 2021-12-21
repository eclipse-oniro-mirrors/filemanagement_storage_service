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

#include "utils/user_path.h"
#include "utils/errno.h"
#include "utils/file_utils.h"
#include <sys/types.h>
#include <unordered_map>

using namespace std;

namespace OHOS {
namespace StorageDaemon {
constexpr uid_t OID_ROOT = 0;

vector<DirInfo> g_el1DirVec = {
        {"/data/app/el1/%s", 0711, OID_ROOT, OID_ROOT},
        {"/data/service/el1/%s", 0711, OID_ROOT, OID_ROOT},
        {"/data/vendor/el1/%s", 0711, OID_ROOT, OID_ROOT}
};

vector<DirInfo> g_el2DirVec = {
        {"/data/app/el2/%s", 0711, OID_ROOT, OID_ROOT},
        {"/data/service/el2/%s", 0711, OID_ROOT, OID_ROOT},
        {"/data/vendor/el2/%s", 0711, OID_ROOT, OID_ROOT}
};

vector<DirInfo> g_hmdfsDirVec = {
        {"/data/service/el2/%s/hmdfs", 0711, OID_ROOT, OID_ROOT},
        {"/data/service/el2/%s/hmdfs/files", 0711, OID_ROOT, OID_ROOT},
        {"/storage/media/%s", 0711, OID_ROOT, OID_ROOT},
        {"/storage/media/%s/local", 0711, OID_ROOT, OID_ROOT}
};
}
}