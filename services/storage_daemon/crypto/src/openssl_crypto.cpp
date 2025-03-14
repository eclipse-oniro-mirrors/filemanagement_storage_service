/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "huks_master.h"
#include "openssl_crypto.h"
#include "openssl/err.h"
#include <openssl/sha.h>
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "openssl/evp.h"

namespace OHOS {
namespace StorageDaemon {
int32_t OpensslCrypto::AESDecrypt(const KeyBlob &preKey, KeyContext &keyContext_, KeyBlob &plainText)
{
    LOGI("enter");
    KeyBlob shield = HashWithPrefix(preKey, keyContext_.secDiscard, AES_256_HASH_RANDOM_SIZE);
    if (keyContext_.rndEnc.size < GCM_NONCE_BYTES + GCM_MAC_BYTES) {
        LOGE("GCM cipherText too small: %{public}u ", keyContext_.rndEnc.size);
        return E_KEY_SIZE_ERROR;
    }
    auto ctx = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>(
        EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (!ctx) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_CIPHER_CTX_NEW;
    }
    if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_gcm(), NULL, reinterpret_cast<const uint8_t*>(shield.data.get()),
                           reinterpret_cast<const uint8_t*>(keyContext_.rndEnc.data.get())) !=
                           OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_DECRYPT_INIT_EX;
    }
    plainText = KeyBlob(keyContext_.rndEnc.size - GCM_NONCE_BYTES - GCM_MAC_BYTES);
    int outlen;
    if (EVP_DecryptUpdate(ctx.get(), reinterpret_cast<uint8_t*>(plainText.data.get()), &outlen,
                          reinterpret_cast<const uint8_t*>(keyContext_.rndEnc.data.get() + GCM_NONCE_BYTES),
                          plainText.size) != OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_DECRYPT_UPDATE;
    }
    if (static_cast<int>(plainText.size) != outlen) {
        LOGE("GCM plainText length should be %{private}u, was %{public}d", plainText.size, outlen);
        return E_DECRYPT_UPDATE_LEN;
    }
    if (EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_TAG, GCM_MAC_BYTES,
                            const_cast<void*>(reinterpret_cast<const void*>(
                            keyContext_.rndEnc.data.get() + GCM_NONCE_BYTES + plainText.size))) !=
                            OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_CIPHER_CTX_CTRL;
    }
    if (EVP_DecryptFinal_ex(ctx.get(), reinterpret_cast<uint8_t*>(plainText.data.get() + plainText.size),
                            &outlen) != OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_DECRYPT_FINAL_EX;
    }
    if (outlen != 0) {
        LOGE("GCM EncryptFinal should be 0, was %{public}d ", outlen);
        return E_DECRYPT_FINAL_EX_LEN;
    }
    LOGI("Enhance decrypt key success");
    return E_OK;
}

int32_t OpensslCrypto::AESEncrypt(const KeyBlob &preKey, const KeyBlob &plainText, KeyContext &keyContext_)
{
    LOGI("enter");
    KeyBlob shield = HashWithPrefix(preKey, keyContext_.secDiscard, AES_256_HASH_RANDOM_SIZE);
    auto ctx = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>(
        EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (!ctx) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_CIPHER_CTX_NEW;
    }
    keyContext_.rndEnc = HuksMaster::GenerateRandomKey(GCM_NONCE_BYTES + plainText.size + GCM_MAC_BYTES);
    if (EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_gcm(), NULL,
                           reinterpret_cast<const uint8_t*>(shield.data.get()),
                           reinterpret_cast<const uint8_t*>(keyContext_.rndEnc.data.get())) !=
                           OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_ENCRYPT_INIT_EX;
    }
    int outlen;
    if (EVP_EncryptUpdate(ctx.get(), reinterpret_cast<uint8_t*>(keyContext_.rndEnc.data.get() + GCM_NONCE_BYTES),
                          &outlen, reinterpret_cast<const uint8_t*>(plainText.data.get()), plainText.size) !=
                          OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_ENCRYPT_UPDATE;
    }
    if (static_cast<int>(plainText.size) != outlen) {
        LOGE("GCM cipherText length should be %{private}d, was %{public}u", plainText.size, outlen);
        return E_ENCRYPT_UPDATE_LEN;
    }
    if (EVP_EncryptFinal_ex(ctx.get(),
                            reinterpret_cast<uint8_t*>(keyContext_.rndEnc.data.get() +
                            GCM_NONCE_BYTES + plainText.size), &outlen) != OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_ENCRYPT_FINAL_EX;
    }
    if (outlen != 0) {
        LOGE("GCM EncryptFinal should be 0 , was %{public}u", outlen);
        return E_ENCRYPT_FINAL_EX_LEN;
    }
    if (EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_GET_TAG, GCM_MAC_BYTES,
                            reinterpret_cast<uint8_t*> (keyContext_.rndEnc.data.get() +
                            GCM_NONCE_BYTES + plainText.size)) != OPENSSL_SUCCESS_FLAG) {
        LOGE("Openssl error: %{public}lu ", ERR_get_error());
        return E_CIPHER_CTX_CTRL;
    }
    LOGI("Enhance encrypt key success");
    return E_OK;
}

KeyBlob OpensslCrypto::HashWithPrefix(const KeyBlob &prefix, const KeyBlob &payload, uint32_t length)
{
    KeyBlob res(SHA512_DIGEST_LENGTH);
    SHA512_CTX c;
    SHA512_Init(&c);
    SHA512_Update(&c, prefix.data.get(), prefix.size);
    if (!payload.IsEmpty()) {
        SHA512_Update(&c, payload.data.get(), payload.size);
    }
    SHA512_Final(res.data.get(), &c);

    res.size = length;
    return res;
}

} // namespace StorageDaemon
} // namespace OHOS