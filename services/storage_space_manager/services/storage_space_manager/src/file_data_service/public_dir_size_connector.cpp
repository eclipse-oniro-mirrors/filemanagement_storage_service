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

#include "file_data_service/public_dir_size_connector.h"

#include <dlfcn.h>

#include "storage_space_manager_errno.h"
#include "storage_space_manager_hilog.h"

namespace OHOS {
namespace StorageSpaceManager {
namespace {
constexpr const char *LIB_EXT_ADAPTER_NAME = "libstorage_service_ext_adapter.z.so";
constexpr const char *FUNC_QUERY_PUBLIC_DIR_SIZE = "QueryPublicDirSizeViaAdapter";
} // namespace

PublicDirSizeConnector::QueryPublicDirSizeFunc PublicDirSizeConnector::LoadQueryFunc()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (queryFunc_ != nullptr) {
        return queryFunc_;
    }
    if (loadFailed_) {
        return nullptr;
    }
    // Prefer reusing an already-loaded so (RTLD_NOLOAD) to avoid duplicate mapping
    adapterHandle_ = dlopen(LIB_EXT_ADAPTER_NAME, RTLD_NOW | RTLD_NODELETE | RTLD_NOLOAD);
    if (adapterHandle_ == nullptr) {
        adapterHandle_ = dlopen(LIB_EXT_ADAPTER_NAME, RTLD_NOW | RTLD_NODELETE);
    }
    if (adapterHandle_ == nullptr) {
        LOGE("load %{public}s failed, dlerror=%{public}s", LIB_EXT_ADAPTER_NAME, dlerror());
        loadFailed_ = true;
        return nullptr;
    }
    dlerror();
    queryFunc_ = reinterpret_cast<QueryPublicDirSizeFunc>(dlsym(adapterHandle_, FUNC_QUERY_PUBLIC_DIR_SIZE));
    if (dlerror() != nullptr || queryFunc_ == nullptr) {
        LOGE("dlsym %{public}s failed", FUNC_QUERY_PUBLIC_DIR_SIZE);
        dlclose(adapterHandle_);
        adapterHandle_ = nullptr;
        queryFunc_ = nullptr;
        loadFailed_ = true;
        return nullptr;
    }
    return queryFunc_;
}

int32_t PublicDirSizeConnector::QueryPublicDirSize(int32_t userId, int64_t &publicDirSize)
{
    LOGI("QueryPublicDirSize begin, userId=%{public}d", userId);
    publicDirSize = 0;
    QueryPublicDirSizeFunc func = LoadQueryFunc();
    if (func == nullptr) {
        LOGE("QueryPublicDirSize unavailable, userId=%{public}d", userId);
        return E_FAIL;
    }
    return func(userId, publicDirSize);
}
} // namespace StorageSpaceManager
} // namespace OHOS
