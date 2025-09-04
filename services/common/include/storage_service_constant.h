/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#ifndef STORAGE_SERVICE_CONSTANTS_H
#define STORAGE_SERVICE_CONSTANTS_H

#include <map>
#include <sys/types.h>

namespace OHOS {
namespace StorageService {
const int START_USER_ID = 0;
const int MAX_USER_ID = 10738; // user id range is (0, 10737]
const int START_APP_CLONE_USER_ID = 219;
const int MAX_APP_CLONE_USER_ID = 239;
const int ZERO_USER = 0;
const int UID_FILE_MANAGER = 1006;
const int32_t USER_ID_BASE = 200000;
const int32_t APP_UID = 20000000;
const int32_t ZERO_USER_MIN_UID = 20000;
const int32_t ZERO_USER_MAX_UID = 65535;
const int MAX_APP_INDEX = 5;
const int PRIORITY_LEVEL = -20;
const int ANCO_SA_UID = 7558;
}

namespace StorageDaemon {
constexpr uint32_t GLOBAL_USER_ID = 0;
constexpr uint32_t USER_ID_SIZE_VALUE = 16;
constexpr char FILE_SEPARATOR_CHAR = '/';
constexpr const char *ANCO_DIR = "/data/virt_service/rgm_hmos/anco_hmos_data/";
constexpr const char *ANCO_MEDIA_PATH = "/data/virt_service/rgm_hmos/anco_hmos_data/media/0";
constexpr const char *SERVICE_DIR_PATH = "/data/service/";

const inline std::string EL1 = "el1";
const inline std::string EL2 = "el2";
const inline std::string EL3 = "el3";
const inline std::string EL4 = "el4";
const inline std::string EL5 = "el5";

enum KeyType {
    EL1_KEY = 1,
    EL2_KEY = 2,
    EL3_KEY = 3,
    EL4_KEY = 4,
    EL5_KEY = 5,
};

static std::map<std::string, KeyType> EL_DIR_MAP = {
    {EL1, EL1_KEY},
    {EL2, EL2_KEY},
    {EL3, EL3_KEY},
    {EL4, EL4_KEY},
    {EL5, EL5_KEY},
};

enum QuotaIdType {
    USRID,
    GRPID,
    PRJID
};

enum IStorageDaemonEnum {
    CRYPTO_FLAG_EL1 = 1,
    CRYPTO_FLAG_EL2 = 2,
    CRYPTO_FLAG_EL3 = 4,
    CRYPTO_FLAG_EL4 = 8,
    CRYPTO_FLAG_EL5 = 16,
};
}
}

#endif // STORAGE_SERVICE_CONSTANTS_H