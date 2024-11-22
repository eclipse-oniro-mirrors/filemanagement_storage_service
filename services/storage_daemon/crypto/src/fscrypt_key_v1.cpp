/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "fscrypt_key_v1.h"

#include <fcntl.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <cstdio>
#include <dirent.h>

#include "file_ex.h"
#include "key_backup.h"
#include "libfscrypt/key_control.h"
#include "storage_service_log.h"
#include "utils/file_utils.h"

namespace OHOS {
namespace StorageDaemon {
static const std::string CRYPTO_NAME_PREFIXES[] = {"ext4", "f2fs", "fscrypt"};

bool FscryptKeyV1::ActiveKey(uint32_t flag, const std::string &mnt)
{
    uint32_t elType;
    (void)mnt;
    LOGI("enter");
    if (!GenerateKeyDesc()) {
        keyInfo_.key.Clear();
        LOGE("GenerateKeyDesc failed");
        return false;
    }
    LOGE("ActiveKey key is empty: %{public}u", keyInfo_.key.IsEmpty());
    if (!fscryptV1Ext.ActiveKeyExt(flag, keyInfo_.key.data.get(), keyInfo_.key.size, elType)) {
        keyInfo_.key.Clear();
        LOGE("fscryptV1Ext ActiveKeyExtfailed");
        return false;
    }
    if (elType == TYPE_EL3 || elType == TYPE_EL4) {
        uint32_t sdpClass;
        if (elType == TYPE_EL3) {
            sdpClass = FSCRYPT_SDP_SECE_CLASS;
        } else {
            sdpClass = FSCRYPT_SDP_ECE_CLASS;
        }
        if (!InstallEceSeceKeyToKeyring(sdpClass)) {
            keyInfo_.key.Clear();
            LOGE("InstallEceSeceKeyToKeyring failed");
            return false;
        }
    } else {
        if (!InstallKeyToKeyring()) {
            keyInfo_.key.Clear();
            LOGE("InstallKeyToKeyring failed");
            return false;
        }
    }
    keyInfo_.key.Clear();
    LOGI("success");
    return true;
}

bool FscryptKeyV1::GenerateAppkey(uint32_t userId, uint32_t hashId, std::string &keyDesc)
{
    KeyBlob appKey(FBEX_KEYID_SIZE);
    if (!fscryptV1Ext.GenerateAppkey(userId, hashId, appKey.data, appKey.size)) {
        LOGE("fscryptV1Ext GenerateAppkey failed");
        return false;
    }
    // The ioctl does not support EL5, return empty character string
    if (appKey.data.get() == nullptr) {
        LOGE("appKey.data.get() is nullptr");
        keyDesc = "";
        return true;
    }
    if (!GenerateAppKeyDesc(appKey)) {
        LOGE("GenerateAppKeyDesc failed");
        return false;
    }
    if (!InstallKeyForAppKeyToKeyring(appKey)) {
        LOGE("InstallKeyForAppKeyToKeyring failed");
        return false;
    }
    appKey.Clear();
    keyDesc = keyInfo_.keyDesc.ToString();
    keyInfo_.keyDesc.Clear();
    LOGI("success");
    return true;
}

bool FscryptKeyV1::InstallKeyForAppKeyToKeyring(KeyBlob &appKey)
{
    LOGI("InstallKeyForAppKeyToKeyring enter");
    EncryptAsdpKey fskey;
    fskey.size = appKey.size;
    fskey.version = 0;
    auto err = memcpy_s(fskey.raw, FSCRYPT_MAX_KEY_SIZE, appKey.data.get(), appKey.size);
    if (err != EOK) {
        LOGE("memcpy failed ret %{public}d", err);
        return false;
    }
    key_serial_t krid = KeyCtrlSearch(KEY_SPEC_SESSION_KEYRING, "keyring", "fscrypt", 0);
    if (krid < 0) {
        LOGI("no session keyring for fscrypt");
        krid = KeyCtrlAddKey("keyring", "fscrypt", KEY_SPEC_SESSION_KEYRING);
        if (krid < 0) {
            LOGE("failed to add session keyring");
            return false;
        }
    }
    for (auto prefix : CRYPTO_NAME_PREFIXES) {
        std::string keyref = prefix + ":" + keyInfo_.keyDesc.ToString();
        key_serial_t ks =
            KeyCtrlAddAppAsdpKey("logon", keyref.c_str(), &fskey, krid);
        if (ks < 0) {
            // Addkey failed, need to process the error
            LOGE("Failed to AddKey, errno %{public}d", errno);
        }
    }
    LOGI("success");
    return true;
}

bool FscryptKeyV1::DeleteAppkey(const std::string KeyId)
{
    LOGI("DeleteAppkey enter");
    if (!UninstallKeyForAppKeyToKeyring(KeyId)) {
        LOGE("FscryptKeyV1 Delete Appkey2 failed");
        return false;
    }
    LOGI("success");
    return true;
}

bool FscryptKeyV1::UninstallKeyForAppKeyToKeyring(const std::string keyId)
{
    LOGI("UninstallKeyForAppKeyToKeyring enter");
    if (keyId.length() == 0) {
        LOGE("keyId is null, does not need to be installed?");
        return false;
    }
    key_serial_t krid = KeyCtrlSearch(KEY_SPEC_SESSION_KEYRING, "keyring", "fscrypt", 0);
    if (krid == -1) {
        LOGE("Error searching session keyring for fscrypt-provisioning key for fscrypt");
        return false;
    }
    for (auto prefix : CRYPTO_NAME_PREFIXES) {
        std::string keyref = prefix + ":" + keyId;
        key_serial_t ks = KeyCtrlSearch(krid, "logon", keyref.c_str(), 0);
        if (KeyCtrlUnlink(ks, krid) != 0) {
            LOGE("Failed to unlink key !");
        }
    }
    LOGI("success");
    return true;
}

bool FscryptKeyV1::UnlockUserScreen(uint32_t flag, uint32_t sdpClass, const std::string &mnt)
{
    (void)mnt;
    LOGI("enter");
    if (!GenerateKeyDesc()) {
        keyInfo_.key.Clear();
        LOGE("GenerateKeyDesc failed");
        return false;
    }
    LOGI("keyInfo empty: %{public}u:", keyInfo_.key.IsEmpty());
    if (!fscryptV1Ext.UnlockUserScreenExt(flag, keyInfo_.key.data.get(), keyInfo_.key.size)) {
        keyInfo_.key.Clear();
        LOGE("fscryptV1Ext UnlockUserScreenExtfailed");
        return false;
    }
    if (sdpClass == FSCRYPT_SDP_ECE_CLASS) {
        if (!InstallEceSeceKeyToKeyring(sdpClass)) {
            keyInfo_.key.Clear();
            LOGE("UnlockUserScreen InstallKeyToKeyring failed");
            return false;
        }
    }
    keyInfo_.key.Clear();
    LOGI("success");
    return true;
}

bool FscryptKeyV1::AddClassE(bool &isNeedEncryptClassE, bool &isSupport, uint32_t status)
{
    LOGI("AddClassE enter");
    if (!fscryptV1Ext.AddClassE(isNeedEncryptClassE, isSupport, status)) {
        LOGE("fscryptV1Ext AddClassE failed");
        return false;
    }
    LOGI("AddClassE finish");
    return true;
}

bool FscryptKeyV1::DeleteClassEPinCode(uint32_t userId)
{
    LOGI("DeleteClassE enter");
    if (!fscryptV1Ext.DeleteClassEPinCode(userId)) {
        LOGE("fscryptV1Ext DeleteClassE failed");
        return false;
    }
    LOGI("DeleteClassE finish");
    return true;
}

bool FscryptKeyV1::ChangePinCodeClassE(bool &isFbeSupport, uint32_t userId)
{
    LOGI("ChangePinCodeClassE enter, userId: %{public}d", userId);
    if (!fscryptV1Ext.ChangePinCodeClassE(userId, isFbeSupport)) {
        LOGE("fscryptV1Ext ChangePinCodeClassE failed");
        return false;
    }
    LOGI("ChangePinCodeClassE finish");
    return true;
}

bool FscryptKeyV1::DoDecryptClassE(const UserAuth &auth, KeyBlob &eSecretFBE, KeyBlob &decryptedKey,
                                   bool needSyncCandidate)
{
    LOGI("enter");
    auto candidate = GetCandidateDir();
    if (candidate.empty()) {
        // no candidate dir, just restore from the latest
        return KeyBackup::GetInstance().TryRestoreUeceKey(shared_from_this(), auth, eSecretFBE, decryptedKey) == 0;
    }

    if (DecryptKeyBlob(auth, candidate, eSecretFBE, decryptedKey)) {
        // update the latest with the candidate
        UpdateKey("", needSyncCandidate);
        return true;
    }

    LOGE("DoRestoreKey with %{public}s failed", candidate.c_str());
    // try to restore from other versions
    std::vector<std::string> files;
    GetSubDirs(dir_, files);
    std::sort(files.begin(), files.end(), [&](const std::string &a, const std::string &b) {
        if (a.length() != b.length() ||
            a.length() < PATH_KEY_VERSION.length() ||
            b.length() < PATH_KEY_VERSION.length()) {
            return a.length() > b.length();
        }
        // make sure a.length() >= PATH_KEY_VERSION.length() && b.length() >= PATH_KEY_VERSION.length()
        return std::stoi(a.substr(PATH_KEY_VERSION.size() - 1)) > std::stoi(b.substr(PATH_KEY_VERSION.size() - 1));
    });
    for (const auto &it: files) {
        if (it != candidate) {
            if (DecryptKeyBlob(auth, dir_ + "/" + it, eSecretFBE, decryptedKey)) {
                UpdateKey(it, needSyncCandidate);
                return true;
            }
        }
    }
    return false;
}

bool FscryptKeyV1::DecryptClassE(const UserAuth &auth, bool &isSupport, bool &eBufferStatue,
                                 uint32_t user, bool needSyncCandidate)
{
    LOGI("enter");
    KeyBlob eSecretFBE(AES_256_HASH_RANDOM_SIZE + GCM_MAC_BYTES + GCM_NONCE_BYTES);
    bool isFbeSupport = true;
    if (!fscryptV1Ext.ReadClassE(USER_UNLOCK, eSecretFBE.data, eSecretFBE.size, isFbeSupport)) {
        LOGE("fscryptV1Ext ReadClassE failed");
        return false;
    }
    if ((auth.token.IsEmpty() && auth.secret.IsEmpty()) || eSecretFBE.IsEmpty()) {
        LOGE("Token and secret is invalid, do not deal.");
        eBufferStatue = eSecretFBE.IsEmpty();
        eSecretFBE.Clear();
        return true;
    }
    if (!isFbeSupport) {
        LOGE("fbe not support uece, skip!");
        isSupport = false;
        return true;
    }
    LOGI("Decrypt keyPath is %{public}s", (dir_ + PATH_LATEST).c_str());
    KeyBlob decryptedKey(AES_256_HASH_RANDOM_SIZE);
    if (!DoDecryptClassE(auth, eSecretFBE, decryptedKey, needSyncCandidate)) {
        LOGE("DecryptKeyBlob Decrypt failed");
        eSecretFBE.Clear();
        return false;
    }
    eSecretFBE.Clear();
    LOGI("Decrypt end!");
    if (!fscryptV1Ext.WriteClassE(USER_UNLOCK, decryptedKey.data.get(), decryptedKey.size)) {
        LOGE("fscryptV1Ext WriteClassE failed");
        return false;
    }
    decryptedKey.Clear();
    LOGI("finish");
    return true;
}

bool FscryptKeyV1::EncryptClassE(const UserAuth &auth, bool &isSupport, uint32_t user, uint32_t status)
{
    LOGI("enter");
    KeyBlob eSecretFBE(AES_256_HASH_RANDOM_SIZE);
    bool isFbeSupport = true;
    if (!fscryptV1Ext.ReadClassE(status, eSecretFBE.data, eSecretFBE.size, isFbeSupport)) {
        LOGE("fscryptV1Ext ReadClassE failed");
        return false;
    }
    if (!isFbeSupport) {
        LOGE("fbe not support E type, skip!");
        isSupport = false;
        return true;
    }
    KeyBlob encryptedKey(AES_256_HASH_RANDOM_SIZE + GCM_MAC_BYTES + GCM_NONCE_BYTES);
    if (!EncryptKeyBlob(auth, dir_ + PATH_LATEST, eSecretFBE, encryptedKey)) {
        LOGE("EncryptKeyBlob Decrypt failed");
        eSecretFBE.Clear();
        return false;
    }
    eSecretFBE.Clear();
    if (!RenameKeyPath(dir_ + PATH_LATEST)) {
        LOGE("RenameKeyPath failed");
        return false;
    }
    LOGI("encrypt end");
    if (!fscryptV1Ext.WriteClassE(status, encryptedKey.data.get(), encryptedKey.size)) {
        LOGE("fscryptV1Ext WriteClassE failed");
        return false;
    }
    encryptedKey.Clear();
    LOGI("finish");
    return true;
}

bool FscryptKeyV1::InstallKeyToKeyring()
{
    fscrypt_key fskey;
    fskey.mode = FS_ENCRYPTION_MODE_AES_256_XTS;
    fskey.size = keyInfo_.key.size;
    auto err = memcpy_s(fskey.raw, FS_MAX_KEY_SIZE, keyInfo_.key.data.get(), keyInfo_.key.size);
    if (err != EOK) {
        LOGE("memcpy failed ret %{public}d", err);
        return false;
    }

    key_serial_t krid = KeyCtrlSearch(KEY_SPEC_SESSION_KEYRING, "keyring", "fscrypt", 0);
    if (krid == -1) {
        LOGI("no session keyring for fscrypt");
        krid = KeyCtrlAddKey("keyring", "fscrypt", KEY_SPEC_SESSION_KEYRING);
        if (krid == -1) {
            LOGE("failed to add session keyring");
            return false;
        }
    }
    for (auto prefix : CRYPTO_NAME_PREFIXES) {
        std::string keyref = prefix + ":" + keyInfo_.keyDesc.ToString();
        LOGI("InstallKeyToKeyring: keyref: %{public}s", keyref.c_str());
        LOGI("InstallKeyToKeyring: keyref length: %{public}zu", keyref.length());
        key_serial_t ks =
            KeyCtrlAddKeyEx("logon", keyref.c_str(), &fskey, krid);
        if (ks == -1) {
            // Addkey failed, need to process the error
            LOGE("Failed to AddKey into keyring, errno %{public}d", errno);
        }
    }
    if (!SaveKeyBlob(keyInfo_.keyDesc, dir_ + PATH_KEYDESC)) {
        return false;
    }
    keyInfo_.key.Clear();
    LOGI("success");
    return true;
}

bool FscryptKeyV1::InstallEceSeceKeyToKeyring(uint32_t sdpClass)
{
    EncryptionKeySdp fskey;
    if (keyInfo_.key.size != sizeof(fskey.raw)) {
        LOGE("Wrong key size is %{public}d", keyInfo_.key.size);
        return false;
    }
    fskey.mode = EXT4_ENCRYPTION_MODE_AES_256_XTS;
    auto err = memcpy_s(fskey.raw, sizeof(fskey.raw), keyInfo_.key.data.get(), keyInfo_.key.size);
    if (err != EOK) {
        LOGE("memcpy failed ret %{public}d", err);
        return false;
    }
    fskey.size = EXT4_AES_256_XTS_KEY_SIZE_TO_KEYRING;
    fskey.sdpClass = sdpClass;
    fskey.version = 0;
    key_serial_t krid = KeyCtrlSearch(KEY_SPEC_SESSION_KEYRING, "keyring", "fscrypt", 0);
    if (krid == -1) {
        LOGI("no session keyring for fscrypt");
        krid = KeyCtrlAddKey("keyring", "fscrypt", KEY_SPEC_SESSION_KEYRING);
        if (krid == -1) {
            LOGE("failed to add session keyring");
            return false;
        }
    }
    for (auto prefix : CRYPTO_NAME_PREFIXES) {
        std::string keyref = prefix + ":" + keyInfo_.keyDesc.ToString();
        key_serial_t ks =
                KeyCtrlAddKeySdp("logon", keyref.c_str(), &fskey, krid);
        if (ks == -1) {
            // Addkey failed, need to process the error
            LOGE("Failed to AddKey into keyring, errno %{public}d", errno);
        }
    }
    if (!SaveKeyBlob(keyInfo_.keyDesc, dir_ + PATH_KEYDESC)) {
        return false;
    }
    LOGI("success");
    return true;
}

bool FscryptKeyV1::InactiveKey(uint32_t flag, const std::string &mnt)
{
    (void)mnt;
    LOGI("enter");
    bool ret = true;

    if (!keyInfo_.keyDesc.IsEmpty() && !UninstallKeyToKeyring()) {
        LOGE("UninstallKeyToKeyring failed");
        ret = false;
    }
    if (!fscryptV1Ext.InactiveKeyExt(flag)) {
        LOGE("fscryptV1Ext InactiveKeyExt failed");
        ret = false;
    }
    DropCachesIfNeed();
    LOGI("finish");
    return ret;
}

void FscryptKeyV1::DropCachesIfNeed()
{
    DIR *dir = opendir(MNT_DATA.c_str());
    if (dir == nullptr) {
        sync();
    }
    int fd = dirfd(dir);
    if (fd < 0 || syncfs(fd)) {
        sync();
    }
    LOGI("drop cache start.");
    if (!SaveStringToFile("/proc/sys/vm/drop_caches", "2")) {
        LOGE("Failed to drop cache during key eviction");
    }
    (void)closedir(dir);
    LOGI("drop cache success");
}

bool FscryptKeyV1::LockUserScreen(uint32_t flag, uint32_t sdpClass, const std::string &mnt)
{
    uint32_t elType;
    (void)mnt;
    LOGI("enter");
    bool ret = true;
    if (!fscryptV1Ext.LockUserScreenExt(flag, elType)) {
        LOGE("fscryptV1Ext InactiveKeyExt failed");
        ret = false;
    }
    if (elType == TYPE_EL4) {
        if (!UninstallKeyToKeyring()) {
            LOGE("UninstallKeyToKeyring failed");
            ret = false;
        }
    }
    LOGI("finish");
    return ret;
}

bool FscryptKeyV1::LockUece(bool &isFbeSupport)
{
    LOGI("enter");
    bool ret = true;
    if (!fscryptV1Ext.LockUeceExt(isFbeSupport)) {
        LOGE("fscryptV1Ext InactiveKeyExt failed");
        ret = false;
    }
    LOGI("finish");
    return ret;
}

bool FscryptKeyV1::UninstallKeyToKeyring()
{
    if (keyInfo_.keyDesc.IsEmpty() && !LoadKeyBlob(keyInfo_.keyDesc, dir_ + PATH_KEYDESC)) {
        LOGE("Load keyDesc failed !");
        return false;
    }
    if (keyInfo_.keyDesc.IsEmpty()) {
        DropCachesIfNeed();
        LOGE("keyDesc is null, key not installed?");
        return false;
    }

    key_serial_t krid = KeyCtrlSearch(KEY_SPEC_SESSION_KEYRING, "keyring", "fscrypt", 0);
    if (krid == -1) {
        LOGE("Error searching session keyring for fscrypt-provisioning key for fscrypt");
        return false;
    }
    for (auto prefix : CRYPTO_NAME_PREFIXES) {
        std::string keyref = prefix + ":" + keyInfo_.keyDesc.ToString();
        key_serial_t ks = KeyCtrlSearch(krid, "logon", keyref.c_str(), 0);
        if (KeyCtrlUnlink(ks, krid) != 0) {
            LOGE("Failed to unlink key !");
        }
    }

    LOGI("success");
    return true;
}

bool FscryptKeyV1::GenerateKeyDesc()
{
    if (keyInfo_.key.IsEmpty()) {
        LOGE("key is empty");
        return false;
    }
    SHA512_CTX c;

    SHA512_Init(&c);
    SHA512_Update(&c, keyInfo_.key.data.get(), keyInfo_.key.size);
    uint8_t keyRef1[SHA512_DIGEST_LENGTH] = { 0 };
    SHA512_Final(keyRef1, &c);

    SHA512_Init(&c);
    SHA512_Update(&c, keyRef1, SHA512_DIGEST_LENGTH);
    uint8_t keyRef2[SHA512_DIGEST_LENGTH] = { 0 };
    SHA512_Final(keyRef2, &c);

    static_assert(SHA512_DIGEST_LENGTH >= CRYPTO_KEY_DESC_SIZE, "Hash too short for descriptor");
    keyInfo_.keyDesc.Alloc(CRYPTO_KEY_DESC_SIZE);
    auto err = memcpy_s(keyInfo_.keyDesc.data.get(), keyInfo_.keyDesc.size, keyRef2, CRYPTO_KEY_DESC_SIZE);
    if (err != EOK) {
        LOGE("memcpy failed ret %{public}d", err);
        return false;
    }
    return true;
}

bool FscryptKeyV1::GenerateAppKeyDesc(KeyBlob appKey)
{
    if (appKey.IsEmpty()) {
        LOGE("key is empty");
        return false;
    }
    SHA512_CTX c;
    SHA512_Init(&c);
    SHA512_Update(&c, appKey.data.get(), appKey.size - 1);
    uint8_t keyRef1[SHA512_DIGEST_LENGTH] = { 0 };
    SHA512_Final(keyRef1, &c);

    SHA512_Init(&c);
    SHA512_Update(&c, keyRef1, SHA512_DIGEST_LENGTH);
    uint8_t keyRef2[SHA512_DIGEST_LENGTH] = { 0 };
    SHA512_Final(keyRef2, &c);

    static_assert(SHA512_DIGEST_LENGTH >= CRYPTO_KEY_DESC_SIZE, "Hash too short for descriptor");
    keyInfo_.keyDesc.Alloc(CRYPTO_KEY_DESC_SIZE);
    auto err = memcpy_s(keyInfo_.keyDesc.data.get(), keyInfo_.keyDesc.size, keyRef2, CRYPTO_KEY_DESC_SIZE);
    if (err != EOK) {
        LOGE("memcpy failed ret %{public}d", err);
        return false;
    }
    LOGE("GenerateAppKeyDesc keyDesc : %{private}s", keyInfo_.keyDesc.ToString().c_str());
    return true;
}
} // namespace StorageDaemon
} // namespace OHOS
