/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMANAGEMENT_PUBLIC_DIR_SIZE_CONNECTOR_H
#define OHOS_FILEMANAGEMENT_PUBLIC_DIR_SIZE_CONNECTOR_H

#include <cstdint>
#include <mutex>

#include <nocopyable.h>

namespace OHOS {
namespace StorageSpaceManager {

class PublicDirSizeConnector : public NoCopyable {
public:
    static PublicDirSizeConnector &GetInstance()
    {
        static PublicDirSizeConnector instance;
        return instance;
    }

    /**
     * @brief Query the public directory size of the given user.
     * @param userId User id (values < 0 are invalid).
     * @param publicDirSize Output: public directory size in bytes.
     * @return E_OK on success;
     */
    int32_t QueryPublicDirSize(int32_t userId, int64_t &publicDirSize);

private:
    PublicDirSizeConnector() = default;
    ~PublicDirSizeConnector() = default;

    using QueryPublicDirSizeFunc = int32_t (*)(int32_t userId, int64_t &publicDirSize);
    QueryPublicDirSizeFunc LoadQueryFunc();

    std::mutex mutex_;
    void *adapterHandle_ = nullptr;
    QueryPublicDirSizeFunc queryFunc_ = nullptr;
    bool loadFailed_ = false;
};
} // namespace StorageSpaceManager
} // namespace OHOS

#endif // OHOS_FILEMANAGEMENT_PUBLIC_DIR_SIZE_CONNECTOR_H
