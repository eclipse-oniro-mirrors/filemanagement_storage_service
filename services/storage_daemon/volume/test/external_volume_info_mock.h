/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef EXTERNAL_VOLUME_INFO_MOCK_H
#define EXTERNAL_VOLUME_INFO_MOCK_H

#include "gmock/gmock.h"
#include "volume/external_volume_info.h"

namespace OHOS {
namespace StorageDaemon {
class ExternalVolumeInfoMock : public ExternalVolumeInfo {
public:
    ExternalVolumeInfoMock() {}
    virtual ~ExternalVolumeInfoMock() {}

    MOCK_METHOD1(DoCreate, int32_t(dev_t));
    MOCK_METHOD0(DoDestroy, int32_t());
    MOCK_METHOD2(DoMount, int32_t(std::string, uint32_t));
    MOCK_METHOD2(DoUMount, int32_t(std::string, bool));
    MOCK_METHOD0(DoCheck, int32_t());
    MOCK_METHOD1(DoFormat, int32_t(std::string));
};
} // namespace StorageDaemon
} // namespace OHOS
#endif // EXTERNAL_VOLUME_INFO_MOCK_H