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

#ifndef OHOS_STORAGE_MANAGER_NOTIFICATION_H
#define OHOS_STORAGE_MANAGER_NOTIFICATION_H

#include "volume_external.h"

namespace OHOS {
namespace StorageManager {
class Notification {
public:
    static Notification &GetInstance()
    {
        static Notification instance;
        return instance;
    }
    void NotifyVolumeChange(VolumeState notifyCode, std::shared_ptr<VolumeExternal> volume);
private:
    Notification();
    ~Notification();
    Notification(const Notification &) = delete;
    Notification &operator=(const Notification &) = delete;
    Notification(const Notification &&) = delete;
    Notification &operator=(const Notification &&) = delete;
};
} // StorageManager
} // OHOS

#endif // OHOS_STORAGE_MANAGER_NOTIFICATION_H