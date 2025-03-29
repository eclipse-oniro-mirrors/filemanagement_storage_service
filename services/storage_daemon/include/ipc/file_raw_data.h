/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_STORAGE_DAEMON_FILE_RAW_DATA_H
#define OHOS_STORAGE_DAEMON_FILE_RAW_DATA_H

#include "securec.h"
#include "storage_service_errno.h"
#include <cstring>
namespace OHOS {
namespace StorageDaemon {

constexpr uint32_t MAX_IPC_RAW_DATA_SIZE = 128 * 1024 * 1024; // 128MB

class FileRawData {
public:
    uint32_t size;
    const void *data;
    bool mallocFlag; // true: malloc, false: not malloc
    FileRawData() : size(0), data(nullptr), mallocFlag(false) {};
    FileRawData(uint32_t size, const void *data);
    int32_t RawDataCpy(const void *rawData);
    ~FileRawData();
};
} // namespace StorageDaemon
} // namespace OHOS
#endif // OHOS_STORAGE_DAEMON_FILE_RAW_DATA_H