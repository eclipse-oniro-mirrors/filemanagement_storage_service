/*
 * Copyright (C) 2023-2025 Huawei Device Co., Ltd.
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

#include "crypto/key_manager_ext.h"

#include "storage_service_errno.h"

namespace OHOS {
namespace StorageDaemon {

int KeyManagerExt::GenerateUserKeys(uint32_t user, uint32_t flags)
{
    return E_OK;
}

int KeyManagerExt::DeleteUserKeys(uint32_t user)
{
    return E_OK;
}

int KeyManagerExt::ActiveUserKey(uint32_t user,
                                 const std::vector<uint8_t> &token,
                                 const std::vector<uint8_t> &secret)
{
    return E_OK;
}

int KeyManagerExt::InActiveUserKey(uint32_t user)
{
    return E_OK;
}

KeyManagerExt::KeyManagerExt() {}

KeyManagerExt::~KeyManagerExt() {}

} // namespace StorageDaemon
} // namespace OHOS
