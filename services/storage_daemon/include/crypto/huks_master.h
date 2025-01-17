/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#ifndef STORAGE_DAEMON_CRYPTO_HUKS_MASTER_H
#define STORAGE_DAEMON_CRYPTO_HUKS_MASTER_H

#include "key_blob.h"

#include "huks_hdi.h"

namespace OHOS {
namespace StorageDaemon {
using HkmHdiHandle_t = void *;
using HkmHalDevice_t = HuksHdi *;
using HkmHalCreateHandle = HuksHdi *(*)(void);
using HkmHalDestroyHandle = void (*)(HuksHdi *);

class HuksMaster {
public:
    static HuksMaster &GetInstance()
    {
        static HuksMaster instance;
        return instance;
    }

    /* key operations */
    static KeyBlob GenerateRandomKey(uint32_t keyLen);
    int32_t GenerateKey(const UserAuth &auth, KeyBlob &keyOut);
    int32_t EncryptKey(KeyContext &ctx, const UserAuth &auth, const KeyInfo &key, bool isNeedNewNonce);
    int32_t EncryptKeyEx(const UserAuth &auth, const KeyBlob &rnd, KeyContext &ctx);
    int32_t DecryptKey(KeyContext &ctx, const UserAuth &auth, KeyInfo &key, bool isNeedNewNonce);
    int32_t DecryptKeyEx(KeyContext &ctx, const UserAuth &auth, KeyBlob &rnd);
    bool UpgradeKey(KeyContext &ctx);
private:
    HuksMaster();
    ~HuksMaster();
    HuksMaster(const HuksMaster &) = delete;
    HuksMaster &operator=(const HuksMaster &) = delete;

    /* huks hal interface */
    bool HdiCreate();
    void HdiDestroy();
    int HdiModuleInit();
    int HdiModuleDestroy();
    int HdiGenerateKey(const HksBlob &keyAlias, const HksParamSet *paramSetIn,
                       HksBlob &keyOut);
    int HdiAccessInit(const HksBlob &key, const HksParamSet *paramSet, HksBlob &handle, HksBlob &token);
    int HdiAccessUpdate(const HksBlob &handle, const HksParamSet *paramSet,
                        const HksBlob &inData, struct HksBlob &outData);
    int HdiAccessFinish(const HksBlob &handle, const HksParamSet *paramSet,
                        const HksBlob &inData, HksBlob &outData);
    int HuksHalTripleStage(HksParamSet *paramSet1, const HksParamSet *paramSet2,
                            const KeyBlob &keyIn, KeyBlob &keyOut);
    int HdiAccessUpgradeKey(const HksBlob &oldKey, const HksParamSet *paramSet, struct HksBlob &newKey);

    HkmHdiHandle_t hdiHandle_ = nullptr;
    HkmHalDevice_t halDevice_ = nullptr;
};
} // namespace StorageDaemon
} // namespace OHOS

#endif // STORAGE_DAEMON_CRYPTO_HUKS_MASTER_H
