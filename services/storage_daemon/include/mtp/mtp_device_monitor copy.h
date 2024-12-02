/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_STORAGE_DAEMON_MTP_DEVICE_MONITOR_H
#define OHOS_STORAGE_DAEMON_MTP_DEVICE_MONITOR_H

#include <memory>
#include <nocopyable.h>
#include <singleton.h>
#include <string>
#include <thread>
#include <vector>
#include "mtp/mtp_device_manager.h"

namespace OHOS {
namespace StorageDaemon {
class MtpDeviceMonitor : public NoCopyable  {
    DECLARE_DELAYED_SINGLETON(MtpDeviceMonitor);

#endif