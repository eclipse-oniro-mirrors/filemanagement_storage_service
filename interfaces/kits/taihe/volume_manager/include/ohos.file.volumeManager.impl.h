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

#ifndef OHOS_FILE_VOLUMEMANAGER_IMPL_H
#define OHOS_FILE_VOLUMEMANAGER_IMPL_H
#include "ohos.file.volumeManager.impl.hpp"
#include "ohos.file.volumeManager.proj.hpp"
#include "taihe/runtime.hpp"

#include "storage_manager_connect.h"
#include "storage_service_errno.h"

namespace ANI::volumeManager {
using namespace taihe;
using namespace ohos::file::volumeManager;

Volume MakeVolume(string_view a, string_view b);

Volume GetVolumeByUuidSync(string_view uuid);

array_view<Volume> GetAllVolumesSync();
} // namespace ANI::volumeManager
#endif // OHOS_FILE_VOLUMEMANAGER_IMPL_H