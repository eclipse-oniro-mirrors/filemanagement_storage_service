/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef STORAGE_DAEMON_FSCRYPT_KEY_V1_EXT_MOCK_H
#define STORAGE_DAEMON_FSCRYPT_KEY_V1_EXT_MOCK_H

#include <gmock/gmock.h>
#include <memory>

#include "fscrypt_key_v1_ext.h"

namespace OHOS {
namespace StorageDaemon {
class IFscryptKeyV1Ext {
public:
    virtual ~IFscryptKeyV1Ext() = default;
    virtual bool LockUeceExt(bool &isFbeSupport) = 0;
    virtual bool LockUserScreenExt(uint32_t flag, uint32_t &elType) = 0;
    virtual bool ReadClassE(uint32_t status, std::unique_ptr<uint8_t[]> &classEBuffer, uint32_t length,
                            bool &isFbeSupport) = 0;
    virtual bool WriteClassE(uint32_t status, uint8_t *classEBuffer, uint32_t length) = 0;
    virtual bool ChangePinCodeClassE(uint32_t userId, bool &isFbeSupport) = 0;
    virtual bool AddClassE(bool &isNeedEncryptClassE, bool &isSupport, uint32_t status) = 0;
    virtual bool DeleteClassEPinCode(uint32_t userId) = 0;
    virtual bool GenerateAppkey(uint32_t userId, uint32_t appUid, std::unique_ptr<uint8_t[]> &keyId, uint32_t size) = 0;
    virtual bool UnlockUserScreenExt(uint32_t flag, uint8_t *iv, uint32_t size) = 0;
    virtual uint32_t SetElType() = 0;
public:
    static inline std::shared_ptr<IFscryptKeyV1Ext> fscryptKeyV1ExtMock = nullptr;
};

class FscryptKeyV1ExtMock : public IFscryptKeyV1Ext {
public:
    MOCK_METHOD1(LockUeceExt, bool(bool &));
    MOCK_METHOD2(LockUserScreenExt, bool(uint32_t, uint32_t &));
    MOCK_METHOD4(ReadClassE, bool(uint32_t, std::unique_ptr<uint8_t[]> &, uint32_t, bool &));
    MOCK_METHOD3(WriteClassE, bool(uint32_t, uint8_t *, uint32_t));
    MOCK_METHOD2(ChangePinCodeClassE, bool(uint32_t, bool &));
    MOCK_METHOD3(AddClassE, bool(bool &, bool &, uint32_t));
    MOCK_METHOD1(DeleteClassEPinCode, bool(uint32_t));
    MOCK_METHOD4(GenerateAppkey, bool(uint32_t, uint32_t, std::unique_ptr<uint8_t[]> &, uint32_t));
    MOCK_METHOD3(UnlockUserScreenExt, bool(uint32_t, uint8_t *, uint32_t));
    MOCK_METHOD0(SetElType, uint32_t());
};
}
}
#endif