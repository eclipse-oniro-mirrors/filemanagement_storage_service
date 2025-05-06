/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_FILE_KEYMANAGER_IMPL_H
#define OHOS_FILE_KEYMANAGER_IMPL_H
#include "ohos.file.keyManager.proj.hpp"
#include "ohos.file.keyManager.impl.hpp"
#include "taihe/runtime.hpp"

#include "storage_manager_connect.h"
#include "storage_service_errno.h"


namespace ANI::keyManager {
using namespace taihe;
void DeactivateUserKey(int64_t userId);
}
#endif // OHOS_FILE_KEYMANAGER_IMPL_H