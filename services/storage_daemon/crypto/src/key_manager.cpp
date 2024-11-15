/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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

#include "key_manager.h"

#include <fcntl.h>
#include <filesystem>
#include <string>

#include "base_key.h"
#include "directory_ex.h"
#include "file_ex.h"
#include "fscrypt_key_v1.h"
#include "fscrypt_key_v2.h"
#include "iam_client.h"
#include "libfscrypt/fscrypt_control.h"
#include "libfscrypt/key_control.h"
#include "parameter.h"
#include "storage_service_constant.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "user/mount_manager.h"
#include "user/user_manager.h"
#ifdef EL5_FILEKEY_MANAGER
#include "el5_filekey_manager_kit.h"
#endif
#ifdef EL5_FILEKEY_MANAGER
using namespace OHOS::Security::AccessToken;
#endif

namespace OHOS {
namespace StorageDaemon {
const UserAuth NULL_KEY_AUTH = {};
const std::string DEFAULT_NEED_RESTORE_VERSION = "1";
constexpr const char *UECE_PATH = "/dev/fbex_uece";

std::shared_ptr<BaseKey> KeyManager::GetBaseKey(const std::string& dir)
{
    uint8_t versionFromPolicy = GetFscryptVersionFromPolicy();
    uint8_t kernelSupportVersion = KeyCtrlGetFscryptVersion(MNT_DATA.c_str());
    if (kernelSupportVersion == FSCRYPT_INVALID) {
        LOGE("kernel not support fscrypt");
        return nullptr;
    }
    if ((versionFromPolicy == kernelSupportVersion) && (kernelSupportVersion == FSCRYPT_V2)) {
        return std::dynamic_pointer_cast<BaseKey>(std::make_shared<FscryptKeyV2>(dir));
    }
    if (versionFromPolicy != kernelSupportVersion) {
        LOGE("version from policy %{public}u not same as version from kernel %{public}u", versionFromPolicy,
             kernelSupportVersion);
    }
    return std::dynamic_pointer_cast<BaseKey>(std::make_shared<FscryptKeyV1>(dir));
}

int KeyManager::GenerateAndInstallDeviceKey(const std::string &dir)
{
    LOGI("enter");
    globalEl1Key_ = GetBaseKey(dir);
    if (globalEl1Key_ == nullptr) {
        return -EOPNOTSUPP;
    }

    if (globalEl1Key_->InitKey(true) == false) {
        globalEl1Key_ = nullptr;
        LOGE("global security key init failed");
        return -EFAULT;
    }

    if (globalEl1Key_->StoreKey(NULL_KEY_AUTH) == false) {
        globalEl1Key_->ClearKey();
        globalEl1Key_ = nullptr;
        LOGE("global security key store failed");
        return -EFAULT;
    }

    if (globalEl1Key_->ActiveKey(FIRST_CREATE_KEY) == false) {
        globalEl1Key_->ClearKey();
        globalEl1Key_ = nullptr;
        LOGE("global security key active failed");
        return -EFAULT;
    }

    (void)globalEl1Key_->UpdateKey();
    hasGlobalDeviceKey_ = true;
    LOGI("key create success");
    return 0;
}

int KeyManager::RestoreDeviceKey(const std::string &dir)
{
    LOGI("enter");
    if (globalEl1Key_ != nullptr) {
        LOGI("device key has existed");
        return 0;
    }

    globalEl1Key_ = GetBaseKey(dir);
    if (globalEl1Key_ == nullptr) {
        return -EOPNOTSUPP;
    }

    if (globalEl1Key_->InitKey(false) == false) {
        globalEl1Key_ = nullptr;
        LOGE("global security key init failed");
        return -EFAULT;
    }

    if (globalEl1Key_->RestoreKey(NULL_KEY_AUTH) == false) {
        globalEl1Key_ = nullptr;
        LOGE("global security key restore failed");
        return -EFAULT;
    }

    if (globalEl1Key_->ActiveKey(RETRIEVE_KEY) == false) {
        globalEl1Key_ = nullptr;
        LOGE("global security key active failed");
        return -EFAULT;
    }
    hasGlobalDeviceKey_ = true;
    LOGI("key restore success");

    return 0;
}

int KeyManager::InitGlobalDeviceKey(void)
{
    LOGI("enter");
    int ret = InitFscryptPolicy();
    if (ret < 0) {
        LOGE("fscrypt init failed, fscrypt will not be enabled");
        return ret;
    }

    std::lock_guard<std::mutex> lock(keyMutex_);
    if (hasGlobalDeviceKey_ || globalEl1Key_ != nullptr) {
        LOGI("glabal device el1 have existed");
        return 0;
    }

    ret = MkDir(STORAGE_DAEMON_DIR, S_IRWXU); // para.0700: root only
    if (ret && errno != EEXIST) {
        LOGE("create storage daemon dir error");
        return ret;
    }
    std::error_code errCode;
    if (std::filesystem::exists(DEVICE_EL1_DIR, errCode) && !std::filesystem::is_empty(DEVICE_EL1_DIR)) {
        UpgradeKeys({{0, DEVICE_EL1_DIR}});
        return RestoreDeviceKey(DEVICE_EL1_DIR);
    }
    ret = MkDir(DEVICE_EL1_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("create device el1 key dir = (/data/service/el0/storage_daemon/sd) error");
        return ret;
    }

    return GenerateAndInstallDeviceKey(DEVICE_EL1_DIR);
}

int KeyManager::GenerateAndInstallUserKey(uint32_t userId, const std::string &dir, const UserAuth &auth, KeyType type)
{
    LOGI("enter");
    if (HasElkey(userId, type)) {
        LOGI("The user %{public}u el %{public}u have existed", userId, type);
        return 0;
    }
    auto elKey = GetBaseKey(dir);
    if (elKey == nullptr) {
        return -EOPNOTSUPP;
    }
    if (type == EL5_KEY) {
        return GenerateAndInstallEl5Key(userId, dir, auth);
    }
    if (elKey->InitKey(true) == false) {
        LOGE("user security key init failed");
        return -EFAULT;
    }
    if (elKey->StoreKey(auth) == false) {
        elKey->ClearKey();
        LOGE("user security key store failed");
        return -EFAULT;
    }
    if (elKey->ActiveKey(FIRST_CREATE_KEY) == false) {
        elKey->ClearKey();
        LOGE("user security key active failed");
        return -EFAULT;
    }
    (void)elKey->UpdateKey();
    if (type == EL1_KEY) {
        userEl1Key_[userId] = elKey;
    } else if (type == EL2_KEY) {
        userEl2Key_[userId] = elKey;
    } else if (type == EL3_KEY) {
        userEl3Key_[userId] = elKey;
    } else if (type == EL4_KEY) {
        userEl4Key_[userId] = elKey;
    }
    LOGI("key create success");
    return 0;
}

int KeyManager::GenerateAndInstallEl5Key(uint32_t userId, const std::string &dir, const UserAuth &auth)
{
    LOGI("enter");
    auto elKey = GetBaseKey(dir);
    if (elKey == nullptr) {
        return -EOPNOTSUPP;
    }
    bool isNeedEncryptClassE = true;
    saveESecretStatus[userId] = true;
    if (elKey->AddClassE(isNeedEncryptClassE, saveESecretStatus[userId], FIRST_CREATE_KEY) == false) {
        elKey->ClearKey();
        LOGE("user %{public}u el5 create error", userId);
        return -EFAULT;
    }
    std::string keyDir = GetKeyDirByUserAndType(userId, EL5_KEY);
    if (keyDir == "") {
        return E_KEY_TYPE_INVAL;
    }
    if (!saveESecretStatus[userId]) {
        OHOS::ForceRemoveDirectory(keyDir);
    }
    saveESecretStatus[userId] = (!auth.secret.IsEmpty() && !auth.token.IsEmpty());
    if (isNeedEncryptClassE) {
        if ((!auth.secret.IsEmpty() && !auth.token.IsEmpty()) &&
            !elKey->EncryptClassE(auth, saveESecretStatus[userId], userId, USER_ADD_AUTH)) {
            elKey->ClearKey();
            LOGE("user %{public}u el5 create error", userId);
            return -EFAULT;
        }
    } else {
        bool eBufferStatue = false;
        if (!elKey->DecryptClassE(auth, saveESecretStatus[userId], eBufferStatue, userId, USER_UNLOCK)) {
            LOGE("user %{public}u decrypt error", userId);
        }
    }
    userEl5Key_[userId] = elKey;
    return 0;
}

int KeyManager::RestoreUserKey(uint32_t userId, const std::string &dir, const UserAuth &auth, KeyType type)
{
    LOGI("enter");
    if (HasElkey(userId, type)) {
        LOGI("The user %{public}u el %{public}u have existed", userId, type);
        return 0;
    }

    auto elKey = GetBaseKey(dir);
    if (elKey == nullptr) {
        return -EOPNOTSUPP;
    }

    if (elKey->InitKey(false) == false) {
        LOGE("user security key init failed");
        return -EFAULT;
    }

    if (elKey->RestoreKey(auth) == false) {
        LOGE("user security key restore failed");
        return -EFAULT;
    }

    if (elKey->ActiveKey(RETRIEVE_KEY) == false) {
        LOGE("user security key active failed");
        return -EFAULT;
    }

    if (type == EL1_KEY) {
        userEl1Key_[userId] = elKey;
    } else if (type == EL2_KEY) {
        userEl2Key_[userId] = elKey;
    } else if (type == EL3_KEY) {
        userEl3Key_[userId] = elKey;
    } else if (type == EL4_KEY) {
        userEl4Key_[userId] = elKey;
    } else if (type == EL5_KEY) {
        userEl5Key_[userId] = elKey;
    }
    LOGI("key restore success");

    return 0;
}

bool KeyManager::HasElkey(uint32_t userId, KeyType type)
{
    LOGI("enter");
    switch (type) {
        case EL1_KEY:
            if (userEl1Key_.find(userId) != userEl1Key_.end()) {
                LOGI("user el1 key has existed");
                return true;
            }
            break;
        case EL2_KEY:
            if (userEl2Key_.find(userId) != userEl2Key_.end()) {
                LOGI("user el2 key has existed");
                return true;
            }
            break;
        case EL3_KEY:
            if (userEl3Key_.find(userId) != userEl3Key_.end()) {
                LOGI("user el3 key has existed");
                return true;
            }
            break;
        case EL4_KEY:
            if (userEl4Key_.find(userId) != userEl4Key_.end()) {
                LOGI("user el4 key has existed");
                return true;
            }
            break;
        case EL5_KEY:
            if (userEl5Key_.find(userId) != userEl5Key_.end()) {
                LOGI("user el5 key has existed");
                return true;
            }
            break;
        default:
            LOGE("key type error");
            break;
    }

    return false;
}

bool KeyManager::IsNeedClearKeyFile(std::string file)
{
    LOGI("enter:");
    if (!std::filesystem::exists(file)) {
        LOGE("file not exist, file is %{private}s", file.c_str());
        return false;
    }

    std::string version;
    if (!OHOS::LoadStringFromFile(file, version)) {
        LOGE("LoadStringFromFile return fail, file is %{private}s", file.c_str());
        return false;
    }

    if (version != DEFAULT_NEED_RESTORE_VERSION) {
        LOGE("need to clear, file is %{private}s, version is %{public}s.", file.c_str(), version.c_str());
        return true;
    }
    LOGE("no need to clear, file is %{private}s, version is %{public}s", file.c_str(), version.c_str());
    return false;
}

void KeyManager::ProcUpgradeKey(const std::vector<FileList> &dirInfo)
{
    LOGI("enter:");
    for (const auto &it : dirInfo) {
        std::string needRestorePath = it.path + "/latest/need_restore";
        if (IsNeedClearKeyFile(needRestorePath)) {
            bool ret = RmDirRecurse(it.path);
            if (!ret) {
                LOGE("remove key dir fail, result is %{public}d, dir %{private}s", ret, it.path.c_str());
            }
        }
    }
}

int KeyManager::LoadAllUsersEl1Key(void)
{
    LOGI("enter");
    std::vector<FileList> dirInfo;
    ReadDigitDir(USER_EL2_DIR, dirInfo);
    UpgradeKeys(dirInfo);
    dirInfo.clear();
    ReadDigitDir(USER_EL1_DIR, dirInfo);
    UpgradeKeys(dirInfo);
    for (auto &item : dirInfo) {
        if (RestoreUserKey(item.userId, item.path, NULL_KEY_AUTH, EL1_KEY) != 0) {
            LOGE("user %{public}u el1 key restore error", item.userId);
        }
    }

    /* only for el3/el4 upgrade scene */
    dirInfo.clear();
    ReadDigitDir(USER_EL3_DIR, dirInfo);
    ProcUpgradeKey(dirInfo);
    dirInfo.clear();
    ReadDigitDir(USER_EL4_DIR, dirInfo);
    ProcUpgradeKey(dirInfo);
    dirInfo.clear();
    ReadDigitDir(USER_EL5_DIR, dirInfo);
    ProcUpgradeKey(dirInfo);
    return 0;
}

int KeyManager::InitUserElkeyStorageDir(void)
{
    int ret = MkDir(SERVICE_STORAGE_DAEMON_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("make service storage daemon dir error");
        return ret;
    }

    ret = MkDir(FSCRYPT_EL_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("make service storage daemon dir error");
        return ret;
    }

    ret = MkDir(USER_EL1_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("make el1 storage dir error");
        return ret;
    }
    ret = MkDir(USER_EL2_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("make el2 storage dir error");
        return ret;
    }
    // 0700 means create el3 permissions
    ret = MkDir(USER_EL3_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("make el3 storage dir error");
        return ret;
    }
    // 0700 means create el4 permissions
    ret = MkDir(USER_EL4_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("make el4 storage dir error");
        return ret;
    }
    // 0700 means create el5 permissions
    ret = MkDir(USER_EL5_DIR, S_IRWXU);
    if (ret && errno != EEXIST) {
        LOGE("make el5 storage dir error");
        return ret;
    }
    return 0;
}

int KeyManager::InitGlobalUserKeys(void)
{
    LOGI("enter");
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(keyMutex_);
    int ret = InitUserElkeyStorageDir();
    if (ret) {
        LOGE("Init user el storage dir failed");
        return ret;
    }

    std::string globalUserEl1Path = USER_EL1_DIR + "/" + std::to_string(GLOBAL_USER_ID);
    if (IsDir(globalUserEl1Path)) {
        ret = RestoreUserKey(GLOBAL_USER_ID, globalUserEl1Path, NULL_KEY_AUTH, EL1_KEY);
        if (ret != 0) {
            LOGE("Restore el1 failed");
            return ret;
        }
    } else {
        ret = GenerateAndInstallUserKey(GLOBAL_USER_ID, globalUserEl1Path, NULL_KEY_AUTH, EL1_KEY);
        if (ret != 0) {
            LOGE("Generate el1 failed");
            return ret;
        }
    }

    ret = LoadAllUsersEl1Key();
    if (ret) {
        LOGE("Load all users el1 failed");
        return ret;
    }
    LOGI("Init global user key success");

    return 0;
}

int KeyManager::GenerateUserKeys(unsigned int user, uint32_t flags)
{
    LOGI("start, user:%{public}u", user);
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }
    if ((!IsDir(USER_EL1_DIR)) || (!IsDir(USER_EL2_DIR)) || (!IsDir(USER_EL3_DIR)) ||
        (!IsDir(USER_EL4_DIR)) || (!IsDir(USER_EL5_DIR))) {
        LOGI("El storage dir is not existed");
        return -ENOENT;
    }
    int ret = GenerateElxAndInstallUserKey(user);
    if (ret != E_OK) {
        LOGE("Generate ELX failed!");
        return ret;
    }
    LOGI("Create user el success");
    return ret;
}

int KeyManager::GenerateElxAndInstallUserKey(unsigned int user)
{
    std::string el1Path = USER_EL1_DIR + "/" + std::to_string(user);
    std::string el2Path = USER_EL2_DIR + "/" + std::to_string(user);
    std::string el3Path = USER_EL3_DIR + "/" + std::to_string(user);
    std::string el4Path = USER_EL4_DIR + "/" + std::to_string(user);
    std::string el5Path = USER_EL5_DIR + "/" + std::to_string(user);
    if (IsDir(el1Path) || IsDir(el2Path) || IsDir(el3Path) || IsDir(el4Path) || IsDir(el5Path)) {
        return CheckAndFixUserKeyDirectory(user);
    }
    std::lock_guard<std::mutex> lock(keyMutex_);
    int ret = GenerateAndInstallUserKey(user, el1Path, NULL_KEY_AUTH, EL1_KEY);
    if (ret) {
        LOGE("user el1 create error");
        return ret;
    }

    ret = GenerateAndInstallUserKey(user, el2Path, NULL_KEY_AUTH, EL2_KEY);
    if (ret) {
        DoDeleteUserKeys(user);
        LOGE("user el2 create error");
        return ret;
    }
    ret = GenerateAndInstallUserKey(user, el3Path, NULL_KEY_AUTH, EL3_KEY);
    if (ret) {
        DoDeleteUserKeys(user);
        LOGE("user el3 create error");
        return ret;
    }
    ret = GenerateAndInstallUserKey(user, el4Path, NULL_KEY_AUTH, EL4_KEY);
    if (ret) {
        DoDeleteUserKeys(user);
        LOGE("user el4 create error");
        return ret;
    }
    ret = GenerateAndInstallUserKey(user, el5Path, NULL_KEY_AUTH, EL5_KEY);
    if (ret) {
        DoDeleteUserKeys(user);
        LOGE("user el5 create error");
        return ret;
    }
    saveLockScreenStatus[user] = true;
    return ret;
}

int KeyManager::CheckAndFixUserKeyDirectory(unsigned int user)
{
    std::string el1NeedRestorePath = USER_EL1_DIR + "/" + std::to_string(user) + RESTORE_DIR;
    std::error_code errCode;
    if (std::filesystem::exists(el1NeedRestorePath, errCode)) {
        LOGE("el1 need_restore file is existed, upgrade scene not support.");
        return -EEXIST;
    }
    int ret = GenerateIntegrityDirs(user, EL1_KEY);
    if (ret != -EEXIST) {
        LOGE("GenerateIntegrityDirs el1 failed.");
    }
    ret = GenerateIntegrityDirs(user, EL2_KEY);
    if (ret != -EEXIST) {
        LOGE("GenerateIntegrityDirs el2 failed.");
    }
    return ret;
}

int KeyManager::GenerateIntegrityDirs(int32_t userId, KeyType type)
{
    std::string dirType = (type == EL1_KEY) ? EL1 : EL2;
    std::string userDir = FSCRYPT_EL_DIR + "/" + dirType;
    uint32_t flag_type = (type == EL1_KEY) ? IStorageDaemon::CRYPTO_FLAG_EL1 : IStorageDaemon::CRYPTO_FLAG_EL2;
    std::string versionElx = userDir + "/" + std::to_string(userId) + FSCRYPT_VERSION_DIR;
    std::string encryptElx = userDir + "/" + std::to_string(userId) + ENCRYPT_VERSION_DIR;
    std::string discardElx = userDir + "/" + std::to_string(userId) + SEC_DISCARD_DIR;
    std::string shieldElx = userDir + "/" + std::to_string(userId) + SHIELD_DIR;
    std::error_code errCode;
    if (!std::filesystem::exists(versionElx, errCode) || !std::filesystem::exists(encryptElx, errCode) ||
        !std::filesystem::exists(shieldElx, errCode) || !std::filesystem::exists(discardElx, errCode) ||
        !IsWorkDirExist(dirType, userId)) {
        LOGE("user %{public}d el %{public}d is not integrity. create error", userId, type);
        int ret = DoDeleteUserCeEceSeceKeys(userId, userDir, type == EL1_KEY ? userEl1Key_ : userEl2Key_);
        if (ret != E_OK) {
            LOGE("Delete userId=%{public}d el %{public}d key failed", userId, type);
        }

        ret = GenerateUserKeyByType(userId, type, {}, {});
        if (ret != E_OK) {
            LOGE("upgrade scene:generate user key fail, userId %{public}d, KeyType %{public}d", userId, type);
            return ret;
        }

        LOGI("try to destory dir first, user %{public}d, Type %{public}d", userId, type);
        (void)UserManager::GetInstance()->DestroyUserDirs(userId, flag_type);
        ret = UserManager::GetInstance()->PrepareUserDirs(userId, flag_type);
        if (ret != E_OK) {
            LOGE("upgrade scene:prepare user dirs fail, userId %{public}d, type %{public}d", userId, type);
            return ret;
        }
    }
    LOGI("userId=%{public}d el %{public}d directory is existed, no need fix.", userId, type);
    return -EEXIST;
}

bool KeyManager::IsWorkDirExist(std::string type, int32_t userId)
{
    std::string dataDir = DATA_DIR + type + "/" + std::to_string(userId);
    std::string serviceDir = SERVICE_DIR + type + "/" + std::to_string(userId);
    std::error_code errCode;
    bool isExist = std::filesystem::exists(dataDir, errCode) && std::filesystem::exists(serviceDir, errCode);
    return isExist;
}

int KeyManager::GenerateUserKeyByType(unsigned int user, KeyType type,
                                      const std::vector<uint8_t> &token,
                                      const std::vector<uint8_t> &secret)
{
    LOGI("start, user:%{public}u, type %{public}u", user, type);
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(keyMutex_);
    std::string elPath = GetKeyDirByType(type);
    if (!IsDir(elPath)) {
        LOGI("El storage dir is not existed");
        return -ENOENT;
    }

    std::string elUserKeyPath = elPath + + "/" + std::to_string(user);
    if (IsDir(elUserKeyPath)) {
        LOGE("user %{public}d el key have existed, create error", user);
        return -EEXIST;
    }
    uint64_t secureUid = { 0 };
    if (!secret.empty() && !token.empty()) {
        IamClient::GetInstance().GetSecureUid(user, secureUid);
        LOGE("token is exist, get secure uid");
    }
    UserAuth auth = { .token = token, .secret = secret, .secureUid = secureUid };
    int ret = GenerateAndInstallUserKey(user, elUserKeyPath, auth, type);
    if (ret) {
        LOGE("user el create error, user %{public}u, type %{public}u", user, type);
        return ret;
    }
    LOGI("Create user el success, user %{public}u, type %{public}u", user, type);

    return 0;
}

int KeyManager::DoDeleteUserCeEceSeceKeys(unsigned int user,
                                          const std::string userDir,
                                          std::map<unsigned int, std::shared_ptr<BaseKey>> &userElKey_)
{
    LOGI("enter, userDir is %{public}s", userDir.c_str());
    int ret = 0;
    auto it = userElKey_.find(user);
    if (it != userElKey_.end()) {
        auto elKey = it->second;
        if (!elKey->ClearKey()) {
            LOGE("clear key failed");
            ret = -E_CLEAR_KEY_FAILED;
        }
        userElKey_.erase(user);
        saveLockScreenStatus.erase(user);
    } else {
        std::string elPath = userDir + "/" + std::to_string(user);
        std::shared_ptr<BaseKey> elKey = GetBaseKey(elPath);
        if (elKey == nullptr) {
            LOGE("Malloc el1 Basekey memory failed");
            return -ENOMEM;
        }
        if (!elKey->ClearKey()) {
            LOGE("clear key failed");
            ret = -E_CLEAR_KEY_FAILED;
        }
    }
    LOGI("end, ret is %{public}d", ret);
    return ret;
}

int KeyManager::DoDeleteUserKeys(unsigned int user)
{
    int errCode = 0;
    int deleteRet = DoDeleteUserCeEceSeceKeys(user, USER_EL1_DIR, userEl1Key_);
    if (deleteRet != 0) {
        LOGE("Delete el1 key failed");
        errCode = deleteRet;
    }
    deleteRet = DoDeleteUserCeEceSeceKeys(user, USER_EL2_DIR, userEl2Key_);
    if (deleteRet != 0) {
        LOGE("Delete el2 key failed");
        errCode = deleteRet;
    }
    deleteRet = DoDeleteUserCeEceSeceKeys(user, USER_EL3_DIR, userEl3Key_);
    if (deleteRet != 0) {
        LOGE("Delete el3 key failed");
        errCode = deleteRet;
    }
    deleteRet = DoDeleteUserCeEceSeceKeys(user, USER_EL4_DIR, userEl4Key_);
    if (deleteRet != 0) {
        LOGE("Delete el4 key failed");
        errCode = deleteRet;
    }
    if (IsUeceSupportWithErrno() != ENOENT) {
        deleteRet = DoDeleteUserCeEceSeceKeys(user, USER_EL5_DIR, userEl5Key_);
        if (deleteRet != 0) {
            LOGE("Delete el5 key failed");
            errCode = deleteRet;
        }
    }
    return errCode;
}

int KeyManager::DeleteUserKeys(unsigned int user)
{
    LOGI("start, user:%{public}d", user);
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(keyMutex_);
    int ret = DoDeleteUserKeys(user);
    LOGI("delete user key end, ret is %{public}d", ret);

    auto userTask = userLockScreenTask_.find(user);
    if (userTask != userLockScreenTask_.end()) {
        userLockScreenTask_.erase(userTask);
        LOGI("Delete user %{public}u, erase user task", user);
    }
    return ret;
}

#ifdef USER_CRYPTO_MIGRATE_KEY
int KeyManager::UpdateUserAuth(unsigned int user, struct UserTokenSecret &userTokenSecret,
                               bool needGenerateShield)
#else
int KeyManager::UpdateUserAuth(unsigned int user, struct UserTokenSecret &userTokenSecret)
#endif
{
    std::lock_guard<std::mutex> lock(keyMutex_);
#ifdef USER_CRYPTO_MIGRATE_KEY
    int ret = UpdateCeEceSeceUserAuth(user, userTokenSecret, EL2_KEY, needGenerateShield);
    if (ret != 0) {
        LOGE("user %{public}u UpdateUserAuth el2 key fail", user);
        return ret;
    }
    ret = UpdateCeEceSeceUserAuth(user, userTokenSecret, EL3_KEY, needGenerateShield);
    if (ret != 0) {
        LOGE("user %{public}u UpdateUserAuth el3 key fail", user);
        return ret;
    }
    ret = UpdateCeEceSeceUserAuth(user, userTokenSecret, EL4_KEY, needGenerateShield);
    if (ret != 0) {
        LOGE("user %{public}u UpdateUserAuth el4 key fail", user);
        return ret;
    }
#else
    int ret = UpdateCeEceSeceUserAuth(user, userTokenSecret, EL2_KEY);
    if (ret != 0) {
        LOGE("user %{public}u UpdateUserAuth el2 key fail", user);
        return ret;
    }
    ret = UpdateCeEceSeceUserAuth(user, userTokenSecret, EL3_KEY);
    if (ret != 0) {
        LOGE("user %{public}u UpdateUserAuth el3 key fail", user);
        return ret;
    }
    ret = UpdateCeEceSeceUserAuth(user, userTokenSecret, EL4_KEY);
    if (ret != 0) {
        LOGE("user %{public}u UpdateUserAuth el4 key fail", user);
        return ret;
    }
#endif
    ret = UpdateESecret(user, userTokenSecret);
    if (ret != 0) {
        LOGE("user %{public}u UpdateESecret fail", user);
        return ret;
    }
    return ret;
}

int KeyManager::UpdateESecret(unsigned int user, struct UserTokenSecret &tokenSecret)
{
    LOGI("UpdateESecret enter");
    std::shared_ptr<BaseKey> el5Key = GetUserElKey(user, EL5_KEY);
    std::string el5Path = USER_EL5_DIR + "/" + std::to_string(user);
    if (IsUeceSupport() && el5Key == nullptr) {
        if (!MkDirRecurse(el5Path, S_IRWXU)) {
            LOGE("MkDirRecurse %{public}u failed!", user);
            return -EFAULT;
        }
        LOGI("MkDirRecurse %{public}u success!", user);
        el5Key = GetUserElKey(user, EL5_KEY);
    }
    if (el5Key == nullptr) {
        LOGE("Have not found user %{public}u el key", user);
        return -ENOENT;
    }
    if (tokenSecret.newSecret.empty()) {
        if (!el5Key->DeleteClassEPinCode(user)) {
            LOGE("user %{public}u DeleteClassE fail", user);
            return -EFAULT;
        }
        saveESecretStatus[user] = false;
        return 0;
    }
    if (!tokenSecret.newSecret.empty() && !tokenSecret.oldSecret.empty()) {
        saveESecretStatus[user] = true;
        if (!el5Key->ChangePinCodeClassE(saveESecretStatus[user], user)) {
            LOGE("user %{public}u ChangePinCodeClassE fail", user);
            return -EFAULT;
        }
        return 0;
    }
    uint32_t status = tokenSecret.oldSecret.empty() ? USER_ADD_AUTH : USER_CHANGE_AUTH;
    LOGI("UpdateESecret status is %{public}u", status);
    UserAuth auth = { .token = tokenSecret.token, .secret = tokenSecret.newSecret, .secureUid = tokenSecret.secureUid };
    saveESecretStatus[user] = true;
    if (!el5Key->EncryptClassE(auth, saveESecretStatus[user], user, status)) {
        LOGE("user %{public}u EncryptClassE fail", user);
        return -EFAULT;
    }
    LOGI("saveESecretStatus is %{public}u", saveESecretStatus[user]);
    return 0;
}

#ifdef USER_CRYPTO_MIGRATE_KEY
int KeyManager::UpdateCeEceSeceUserAuth(unsigned int user,
                                        struct UserTokenSecret &userTokenSecret,
                                        KeyType type, bool needGenerateShield)
#else
int KeyManager::UpdateCeEceSeceUserAuth(unsigned int user,
                                        struct UserTokenSecret &userTokenSecret,
                                        KeyType type)
#endif
{
    LOGI("start, user:%{public}d", user);
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }
    std::shared_ptr<BaseKey> item = GetUserElKey(user, type);
    if (item == nullptr) {
        LOGE("Have not found user %{public}u el key", user);
        return -ENOENT;
    }

    UserAuth auth = { {}, userTokenSecret.oldSecret, userTokenSecret.secureUid };
    if (!userTokenSecret.oldSecret.empty()) {
        KeyBlob token(userTokenSecret.token);
        auth.token = std::move(token);
    }
    if ((item->RestoreKey(auth) == false) && (item->RestoreKey(NULL_KEY_AUTH) == false)) {
        LOGE("Restore key error");
        return -EFAULT;
    }
    if (!userTokenSecret.newSecret.empty()) {
        KeyBlob token(userTokenSecret.token);
        KeyBlob newSecret(userTokenSecret.newSecret);
        auth.token = std::move(token);
        auth.secret = std::move(newSecret);
    } else {
        auth.token.Clear();
        auth.secret.Clear();
    }
#ifdef USER_CRYPTO_MIGRATE_KEY
    if (item->StoreKey(auth, needGenerateShield) == false) {
#else
    if (item->StoreKey(auth) == false) {
#endif
        LOGE("Store key error");
        return -EFAULT;
    }

    userPinProtect[user] = !userTokenSecret.newSecret.empty();
    return 0;
}

int KeyManager::ActiveUserKey(unsigned int user, const std::vector<uint8_t> &token,
                              const std::vector<uint8_t> &secret)
{
    LOGI("start");
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }

    if (ActiveCeSceSeceUserKey(user, EL2_KEY, token, secret) != 0) {
        LOGI("Active user %{public}u el2 fail", user);
        return -EFAULT;
    }
    if (ActiveCeSceSeceUserKey(user, EL3_KEY, token, secret) != 0) {
        LOGI("Active user %{public}u el3 fail", user);
        return -EFAULT;
    }
    if (ActiveCeSceSeceUserKey(user, EL4_KEY, token, secret) != 0) {
        LOGI("Active user %{public}u el4 fail", user);
        return -EFAULT;
    }
    if (ActiveCeSceSeceUserKey(user, EL5_KEY, token, secret) != 0) {
        LOGI("Active user %{public}u el5 fail", user);
        return -EFAULT;
    }
    if (UnlockUserAppKeys(user, true) != E_OK) {
        LOGE("failed to delete appkey2");
        return -EFAULT;
    }
    saveESecretStatus[user] = !secret.empty();
    return 0;
}

std::string KeyManager::GetKeyDirByUserAndType(unsigned int user, KeyType type)
{
    std::string keyDir = "";
    switch (type) {
        case EL1_KEY:
            keyDir = USER_EL1_DIR + "/" + std::to_string(user);
            break;
        case EL2_KEY:
            keyDir = USER_EL2_DIR + "/" + std::to_string(user);
            break;
        case EL3_KEY:
            keyDir = USER_EL3_DIR + "/" + std::to_string(user);
            break;
        case EL4_KEY:
            keyDir = USER_EL4_DIR + "/" + std::to_string(user);
            break;
        case EL5_KEY:
            keyDir = USER_EL5_DIR + "/" + std::to_string(user);
            break;
        default:
            LOGE("GetKeyDirByUserAndType type %{public}u is invalid", type);
            break;
    }
    return keyDir;
}

std::string KeyManager::GetKeyDirByType(KeyType type)
{
    std::string keyDir = "";
    switch (type) {
        case EL1_KEY:
            keyDir = USER_EL1_DIR;
            break;
        case EL2_KEY:
            keyDir = USER_EL2_DIR;
            break;
        case EL3_KEY:
            keyDir = USER_EL3_DIR;
            break;
        case EL4_KEY:
            keyDir = USER_EL4_DIR;
            break;
        case EL5_KEY:
            keyDir = USER_EL5_DIR;
            break;
        default:
            LOGE("GetKeyDirByType type %{public}u is invalid", type);
            break;
    }
    return keyDir;
}

void KeyManager::SaveUserElKey(unsigned int user, KeyType type, std::shared_ptr<BaseKey> elKey)
{
    switch (type) {
        case EL1_KEY:
            userEl1Key_[user] = elKey;
            break;
        case EL2_KEY:
            userEl2Key_[user] = elKey;
            break;
        case EL3_KEY:
            userEl3Key_[user] = elKey;
            break;
        case EL4_KEY:
            userEl4Key_[user] = elKey;
            break;
        case EL5_KEY:
            userEl5Key_[user] = elKey;
            break;
        default:
            LOGE("SaveUserElKey type %{public}u is invalid", type);
    }
}

std::shared_ptr<BaseKey> KeyManager::GetUserElKey(unsigned int user, KeyType type)
{
    bool isNeedGenerateBaseKey = false;
    std::shared_ptr<BaseKey> elKey = nullptr;
    if (!HasElkey(user, type)) {
        LOGE("Have not found user %{public}u key, type %{public}u", user, type);
        std::string keyDir = GetKeyDirByUserAndType(user, type);
        if (!IsDir(keyDir)) {
            LOGE("Have not found user %{public}u el, %{public}u type", user, type);
            return nullptr;
        }
        elKey = GetBaseKey(keyDir);
        if (elKey == nullptr) {
            LOGE("BaseKey memory failed");
            return nullptr;
        }
        isNeedGenerateBaseKey = true;
        LOGI("Generate new baseKey type: %{public}u", type);
    }

    switch (type) {
        case EL1_KEY:
            if (isNeedGenerateBaseKey) {
                userEl1Key_[user] = elKey;
            }
            return userEl1Key_[user];
        case EL2_KEY:
            if (isNeedGenerateBaseKey) {
                userEl2Key_[user] = elKey;
            }
            return userEl2Key_[user];
        case EL3_KEY:
            if (isNeedGenerateBaseKey) {
                userEl3Key_[user] = elKey;
            }
            return userEl3Key_[user];
        case EL4_KEY:
            if (isNeedGenerateBaseKey) {
                userEl4Key_[user] = elKey;
            }
            return userEl4Key_[user];
        case EL5_KEY:
            if (isNeedGenerateBaseKey) {
                userEl5Key_[user] = elKey;
            }
            return userEl5Key_[user];
        default:
            LOGE("GetUserElKey type %{public}u is invalid", type);
            return nullptr;
    }
}

int KeyManager::ActiveCeSceSeceUserKey(unsigned int user,
                                       KeyType type,
                                       const std::vector<uint8_t> &token,
                                       const std::vector<uint8_t> &secret)
{
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }
    if (CheckUserPinProtect(user, token, secret) != E_OK) {
        LOGE("IAM & Storage mismatch, wait user input pin.");
        return -EFAULT;
    }
    std::lock_guard<std::mutex> lock(keyMutex_);
    if (HasElkey(user, type) && type != EL5_KEY && HashElxActived(user, type)) {
        LOGE("The user %{public}u el have been actived, key type is %{public}u", user, type);
        return 0;
    }
    std::string keyDir = GetKeyDirByUserAndType(user, type);
    if (keyDir == "") {
        return E_KEY_TYPE_INVAL;
    }
    if ((type != EL5_KEY) && !IsDir(keyDir)) {
        LOGE("Have not found user %{public}u el", user);
        return -ENOENT;
    }
    if ((type == EL5_KEY) && CheckAndDeleteEmptyEl5Directory(keyDir, user) != 0) {
        return -ENOENT;
    }
    std::shared_ptr<BaseKey> elKey = GetBaseKey(keyDir);
    if (elKey == nullptr) {
        LOGE("elKey failed");
        return -EOPNOTSUPP;
    }
    if (type == EL5_KEY) {
        if (ActiveUeceUserKey(user, token, secret, elKey) != 0) {
            LOGE("ActiveUeceUserKey failed");
            return -EFAULT;
        }
        return 0;
    }
    if (ActiveElXUserKey(user, token, type, secret, elKey) != 0) {
        LOGE("ActiveElXUserKey failed");
        return -EFAULT;
    }

    SaveUserElKey(user, type, elKey);
    userPinProtect.erase(user);
    userPinProtect.insert(std::make_pair(user, !secret.empty()));
    saveLockScreenStatus[user] = true;
    LOGI("Active user %{public}u el success", user);
    LOGI("saveLockScreenStatus is %{public}d", saveLockScreenStatus[user]);
    return 0;
}

bool KeyManager::HashElxActived(unsigned int user, KeyType type)
{
    LOGI("enter");
    switch (type) {
        case EL1_KEY:
            return HasElxDesc(userEl1Key_, type, user);
            break;
        case EL2_KEY:
            return HasElxDesc(userEl2Key_, type, user);
            break;
        case EL3_KEY:
            return HasElxDesc(userEl3Key_, type, user);
            break;
        case EL4_KEY:
            return HasElxDesc(userEl4Key_, type, user);
            break;
        case EL5_KEY:
            return HasElxDesc(userEl5Key_, type, user);
            break;
        default:
            LOGE("key type error");
            break;
    }
    return false;
}

bool KeyManager::HasElxDesc(std::map<unsigned int, std::shared_ptr<BaseKey>> &userElKey_,
                            KeyType type,
                            unsigned int user)
{
    auto it = userElKey_.find(user);
    auto elKey = it->second;
    if (it != userElKey_.end() && !elKey->KeyDesclsEmpty()) {
        LOGI("user el%{public}u key desc has existed", type);
        return true;
    }
    return false;
}

int KeyManager::CheckAndDeleteEmptyEl5Directory(std::string keyDir, unsigned int user)
{
    std::string keyUeceDir = UECE_DIR + "/" + std::to_string(user);
    if (!IsDir(keyDir) || !IsDir(keyUeceDir)) {
        LOGE("Have not found dir %{public}u el5", user);
        return -ENOENT;
    }

    if (IsDir(keyDir) && std::filesystem::is_empty(keyDir)) {
        OHOS::ForceRemoveDirectory(keyDir);
        LOGE("Have removed key dir %{public}u el5", user);
        return -ENOENT;
    }
    return 0;
}

bool KeyManager::GetUserDelayHandler(uint32_t userId, std::shared_ptr<DelayHandler> &delayHandler)
{
    LOGI("enter");
    auto iterTask = userLockScreenTask_.find(userId);
    if (iterTask == userLockScreenTask_.end()) {
        std::shared_ptr<DelayHandler> lockScreenTask = std::make_shared<DelayHandler>(userId);
        userLockScreenTask_[userId] = std::make_shared<DelayHandler>(userId);
    }
    delayHandler = userLockScreenTask_[userId];
    if (delayHandler == nullptr) {
        LOGE("user %{public}d delayHandler is nullptr !", userId);
        return false;
    }
    return true;
}

int KeyManager::ActiveUeceUserKey(unsigned int user,
                                       const std::vector<uint8_t> &token,
                                       const std::vector<uint8_t> &secret, std::shared_ptr<BaseKey> elKey)
{
    saveESecretStatus[user] = !secret.empty();
    LOGI("userId %{public}u, token empty %{public}d sec empty %{public}d", user, token.empty(), secret.empty());
    userEl5Key_[user] = elKey;
    UserAuth auth = { .token = token, .secret = secret };
    bool eBufferStatue = false;
    if (!elKey->DecryptClassE(auth, saveESecretStatus[user], eBufferStatue, user, USER_UNLOCK)) {
        LOGE("Unlock user %{public}u E_Class failed", user);
        return -EFAULT;
    }

    if (!token.empty() && !secret.empty() && eBufferStatue) {
        if (TryToFixUeceKey(user, token, secret) != E_OK) {
            LOGE("TryToFixUeceKey el5 failed !");
            return -EFAULT;
        }
    }
    LOGI("ActiveCeSceSeceUserKey user %{public}u, saveESecretStatus %{public}d", user, saveESecretStatus[user]);
    return 0;
}

int KeyManager::ActiveElXUserKey(unsigned int user,
                                 const std::vector<uint8_t> &token, KeyType keyType,
                                 const std::vector<uint8_t> &secret, std::shared_ptr<BaseKey> elKey)
{
    if (elKey->InitKey(false) == false) {
        LOGE("Init el failed");
        return -EFAULT;
    }
    UserAuth auth = { token, secret };
    bool keyResult = elKey->RestoreKey(auth);
    bool noKeyResult = !keyResult && elKey->RestoreKey(NULL_KEY_AUTH);
    // key and no-key situation all failed, include upgrade situation, return err
    if (!keyResult && !noKeyResult) {
        LOGE("Restore el failed, type: %{public}u", keyType);
        return -EFAULT;
    }
    // if device has pwd and decrypt success, continue.otherwise try no pwd and fix situation.
    if (!keyResult && noKeyResult) {
        if (TryToFixUserCeEceSeceKey(user, keyType, token, secret) != E_OK) {
            LOGE("TryToFixUserCeEceSeceKey elx failed, type %{public}u", keyType);
            return -EFAULT;
        }
    }
    std::string NEED_UPDATE_PATH = GetKeyDirByUserAndType(user, keyType) + PATH_LATEST + SUFFIX_NEED_UPDATE;
    if (!FileExists(NEED_UPDATE_PATH) && (elKey->StoreKey(auth) == false)) {
        LOGE("Store el failed");
        return -EFAULT;
    }
    if (elKey->ActiveKey(RETRIEVE_KEY) == false) {
        LOGE("Active user %{public}u key failed", user);
        return -EFAULT;
    }
    return 0;
}

int KeyManager::UnlockUserScreen(uint32_t user, const std::vector<uint8_t> &token, const std::vector<uint8_t> &secret)
{
    LOGI("start");
    userPinProtect[user] = !secret.empty() || !token.empty();
    std::shared_ptr<DelayHandler> userDelayHandler;
    if (GetUserDelayHandler(user, userDelayHandler)) {
        userDelayHandler->CancelDelayTask();
    }
    auto iter = saveLockScreenStatus.find(user);
    if (iter == saveLockScreenStatus.end()) {
        saveLockScreenStatus.insert(std::make_pair(user, false));
    }
    if (!IsUserCeDecrypt(user)) {
        LOGE("user ce does not decrypt, skip");
        return 0;
    }
    if (!KeyCtrlHasFscryptSyspara()) {
        saveLockScreenStatus[user] = true;
        LOGI("saveLockScreenStatus is %{public}d", saveLockScreenStatus[user]);
        return 0;
    }
    std::lock_guard<std::mutex> lock(keyMutex_);
    int ret = 0;
    if (!UnlockEceSece(user, token, secret, ret)) {
        return ret;
    }
    if (!UnlockUece(user, token, secret, ret)) {
        return ret;
    }
    saveLockScreenStatus[user] = true;
    LOGI("UnlockUserScreen user %{public}u el3 and el4 success and saveLockScreenStatus is %{public}d", user,
         saveLockScreenStatus[user]);
    return 0;
}

bool KeyManager::UnlockEceSece(uint32_t user,
                            const std::vector<uint8_t> &token,
                            const std::vector<uint8_t> &secret,
                            int &ret)
{
    auto el4Key = GetUserElKey(user, EL4_KEY);
    if (el4Key == nullptr) {
        saveLockScreenStatus[user] = true;
        LOGE("The user %{public}u not been actived and saveLockScreenStatus is %{public}d", user,
             saveLockScreenStatus[user]);
        ret = 0;
        return false;
    }
    if (!el4Key->RestoreKey({token, secret}) && !el4Key->RestoreKey(NULL_KEY_AUTH)) {
        LOGE("Restore user %{public}u el4 key failed", user);
        ret = -EFAULT;
        return false;
    }
    if (!el4Key->UnlockUserScreen(user, FSCRYPT_SDP_ECE_CLASS)) {
        LOGE("UnlockUserScreen user %{public}u el4 key failed", user);
        ret = -EFAULT;
        return false;
    }
    LOGI("DecryptClassE user %{public}u saveESecretStatus %{public}d", user, saveESecretStatus[user]);
    return true;
}

bool KeyManager::UnlockUece(uint32_t user,
                            const std::vector<uint8_t> &token,
                            const std::vector<uint8_t> &secret,
                            int &ret)
{
    UserAuth auth = {.token = token, .secret = secret};
    saveESecretStatus[user] = !auth.token.IsEmpty();
    auto el5Key = GetUserElKey(user, EL5_KEY);
    bool eBufferStatue = false;
    if (el5Key != nullptr && !el5Key->DecryptClassE(auth, saveESecretStatus[user], eBufferStatue, user, USER_UNLOCK)) {
        LOGE("Unlock user %{public}u uece failed", user);
        ret = -EFAULT;
        return false;
    }
    if (UnlockUserAppKeys(user, false) != E_OK) {
        LOGE("failed to delete appkey2");
        ret = -EFAULT;
        return false;
    }
    return true;
}

int KeyManager::GetLockScreenStatus(uint32_t user, bool &lockScreenStatus)
{
    LOGI("start");
    std::lock_guard<std::mutex> lock(keyMutex_);
    auto iter = saveLockScreenStatus.find(user);
    lockScreenStatus = (iter == saveLockScreenStatus.end()) ? false: iter->second;
    LOGI("lockScreenStatus is %{public}d", lockScreenStatus);
    return 0;
}

int KeyManager::GenerateAppkey(uint32_t userId, uint32_t hashId, std::string &keyId)
{
    if (!IsUeceSupport()) {
        LOGI("Not support uece !");
        return -ENOTSUP;
    }
    if (userEl4Key_.find(userId) == userEl4Key_.end()) {
        LOGE("userEl4Key_ has not existed");
        if (!IsUserCeDecrypt(userId)) {
            LOGE("user ce does not decrypt, skip");
            return -ENOENT;
        }
        GetUserElKey(userId, EL4_KEY);
    }
    auto el4Key = userEl4Key_[userId];
    if (el4Key == nullptr) {
        LOGE("el4Key_ is nullptr");
        return -ENOENT;
    }
    if (el4Key->GenerateAppkey(userId, hashId, keyId) == false) {
        LOGE("Failed to generate Appkey2");
        return -EFAULT;
    }
    return 0;
}

int KeyManager::DeleteAppkey(uint32_t userId, const std::string keyId)
{
    std::lock_guard<std::mutex> lock(keyMutex_);
    if (userEl4Key_.find(userId) == userEl4Key_.end()) {
        LOGE("userEl4Key_ has not existed");
        if (!IsUserCeDecrypt(userId)) {
            LOGE("user ce does not decrypt, skip");
            return -ENOENT;
        }
        GetUserElKey(userId, EL4_KEY);
    }
    auto el4Key = userEl4Key_[userId];
    if (el4Key == nullptr) {
        LOGE("el4Key_ is nullptr");
        return -ENOENT;
    }
    if (el4Key->DeleteAppkey(keyId) == false) {
        LOGE("Failed to delete Appkey2");
        return -EFAULT;
    }
    return 0;
}

int KeyManager::UnlockUserAppKeys(uint32_t userId, bool needGetAllAppKey)
{
    if (!IsUeceSupport()) {
        LOGI("E type is not support");
        return E_OK;
    }
#ifdef EL5_FILEKEY_MANAGER
    std::vector<std::pair<int, std::string>> keyInfo;
    std::vector<std::pair<std::string, bool>> loadInfos;
    if (needGetAllAppKey) {
        if (El5FilekeyManagerKit::GetUserAllAppKey(userId, keyInfo) != 0) {
            LOGE("get user all app keys fail.");
            return -EFAULT;
        }
        LOGI("get user all app keys success.");
    } else {
        if (El5FilekeyManagerKit::GetUserAppKey(userId, keyInfo) != 0) {
            LOGE("get User Appkeys fail.");
            return -EFAULT;
        }
        LOGI("get User Appkeys success.");
    }
    if (keyInfo.size() == 0) {
        LOGE("The keyInfo is empty!");
        return 0;
    }
    if (userEl5Key_.find(userId) == userEl5Key_.end()) {
        LOGE("userEl5Key_ has not existed");
        return -ENOENT;
    }
    auto elKey = userEl5Key_[userId];
    std::string keyId;
    for (auto keyInfoAppUid :keyInfo) {
        if (elKey->GenerateAppkey(userId, keyInfoAppUid.first, keyId) == false) {
            LOGE("Failed to Generate Appkey2!");
            loadInfos.push_back(std::make_pair(keyInfoAppUid.second, false));
        }
        if (keyInfoAppUid.second != keyId) {
            LOGE("The keyId check fails!");
            loadInfos.push_back(std::make_pair(keyInfoAppUid.second, false));
        }
        loadInfos.push_back(std::make_pair(keyInfoAppUid.second, true));
    }
    if (El5FilekeyManagerKit::ChangeUserAppkeysLoadInfo(userId, loadInfos) != 0) {
        LOGE("Change User Appkeys LoadInfo fail.");
        return -EFAULT;
    }
#endif
    LOGI("UnlockUserAppKeys success!");
    return E_OK;
}

int KeyManager::InActiveUserKey(unsigned int user)
{
    LOGI("start");
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(keyMutex_);
    int ret = InactiveUserElKey(user, userEl2Key_);
    if (ret != E_OK) {
        LOGE("Inactive userEl2Key_ failed");
        return ret;
    }
    ret = InactiveUserElKey(user, userEl3Key_);
    if (ret != E_OK) {
        LOGE("Inactive userEl3Key_ failed");
        return ret;
    }
    ret = InactiveUserElKey(user, userEl4Key_);
    if (ret != E_OK) {
        LOGE("Inactive userEl4Key_ failed");
        return ret;
    }
    ret = InactiveUserElKey(user, userEl5Key_);
    if (ret != E_OK) {
        LOGE("Inactive userEl5Key_ failed");
        return ret;
    }
    auto userTask = userLockScreenTask_.find(user);
    if (userTask != userLockScreenTask_.end()) {
        userLockScreenTask_.erase(userTask);
        LOGI("InActive user %{public}u, erase user task", user);
    }
    return 0;
}

int KeyManager::InactiveUserElKey(unsigned int user, std::map<unsigned int, std::shared_ptr<BaseKey>> &userElxKey_)
{
    if (userElxKey_.find(user) == userElxKey_.end()) {
        LOGE("Have not found user %{public}u el2", user);
        return -ENOENT;
    }
    auto elKey = userElxKey_[user];
    if (elKey->InactiveKey(USER_LOGOUT) == false) {
        LOGE("Clear user %{public}u key failed", user);
        return -EFAULT;
    }
    userElxKey_.erase(user);
    LOGI("Inactive user %{public}u elX success", user);
    return 0;
}

int KeyManager::LockUserScreen(uint32_t user)
{
    LOGI("start");
    std::lock_guard<std::mutex> lock(keyMutex_);
    if (!IsUserCeDecrypt(user)) {
        LOGE("user ce does not decrypt, skip");
        return 0;
    }
    CheckAndClearTokenInfo(user);
    auto iter = userPinProtect.find(user);
    if (iter == userPinProtect.end() || iter->second == false) {
        if (!IamClient::GetInstance().HasPinProtect(user)) {
            LOGI("Has no pin protect, saveLockScreenStatus is %{public}d", saveLockScreenStatus[user]);
            return 0;
        }
        userPinProtect.erase(user);
        userPinProtect.insert(std::make_pair(user, true));
    }
    iter = saveLockScreenStatus.find(user);
    if (iter == saveLockScreenStatus.end()) {
        saveLockScreenStatus.insert(std::make_pair(user, false));
    }
    if (!KeyCtrlHasFscryptSyspara()) {
        saveLockScreenStatus[user] = false;
        LOGI("KeyCtrlHasFscryptSyspara is false, saveLockScreenStatus is %{public}d",
            saveLockScreenStatus[user]);
        return 0;
    }
    auto el5Key = GetUserElKey(user, EL5_KEY);
    saveESecretStatus[user] = true;
    if (el5Key != nullptr && !el5Key->LockUece(saveESecretStatus[user])) {
        LOGE("lock user %{public}u el5 key failed !", user);
    }
    auto el4Key = GetUserElKey(user, EL4_KEY);
    if (el4Key == nullptr) {
        LOGE("Have not found user %{public}u el3 or el4", user);
        return -ENOENT;
    }
    std::shared_ptr<DelayHandler> userDelayHandler;
    if (GetUserDelayHandler(user, userDelayHandler)) {
        userDelayHandler->StartDelayTask(el4Key);
    }

    saveLockScreenStatus[user] = false;
    LOGI("LockUserScreen user %{public}u el3 and el4 success, saveLockScreenStatus is %{public}d",
        user, saveLockScreenStatus[user]);
    return 0;
}

int KeyManager::SetDirectoryElPolicy(unsigned int user, KeyType type, const std::vector<FileList> &vec)
{
    LOGI("start");
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }
    std::string keyPath;
    std::string eceSeceKeyPath;
    std::lock_guard<std::mutex> lock(keyMutex_);
    if (type == EL1_KEY) {
        if (userEl1Key_.find(user) == userEl1Key_.end()) {
            LOGE("Have not found user %{public}u el1 key, not enable el1", user);
            return -ENOENT;
        }
        keyPath = userEl1Key_[user]->GetDir();
    } else if (type == EL2_KEY || type == EL3_KEY || type == EL4_KEY || type == EL5_KEY) {
        if (userEl2Key_.find(user) == userEl2Key_.end()) {
            LOGE("Have not found user %{public}u el2 key, not enable el2", user);
            return -ENOENT;
        }
        keyPath = userEl2Key_[user]->GetDir();
    } else {
        LOGE("Not specify el flags, no need to crypt");
        return 0;
    }
    if (getEceSeceKeyPath(user, type, eceSeceKeyPath) != 0) {
        LOGE("method getEceSeceKeyPath fail");
        return -ENOENT;
    }
    for (auto item : vec) {
        int ret = LoadAndSetPolicy(keyPath.c_str(), item.path.c_str());
        if (ret != 0) {
            LOGE("Set directory el policy error, ret: %{public}d", ret);
            return -EFAULT;
        }
    }
    if (type == EL3_KEY || type == EL4_KEY) {
        for (auto item : vec) {
            if (LoadAndSetEceAndSecePolicy(eceSeceKeyPath.c_str(), item.path.c_str(), static_cast<int>(type)) != 0) {
                LOGE("Set directory el policy error!");
                return -EFAULT;
            }
        }
    }
    LOGI("Set user %{public}u el policy success", user);
    return 0;
}

int KeyManager::getEceSeceKeyPath(unsigned int user, KeyType type, std::string &eceSeceKeyPath)
{
    if (type == EL3_KEY) {
        if (userEl3Key_.find(user) == userEl3Key_.end()) {
            LOGI("Have not found user %{public}u el3 key, not enable el3", user);
            return -ENOENT;
        }
        eceSeceKeyPath = userEl3Key_[user]->GetDir();
    }
    if (type == EL4_KEY) {
        if (userEl4Key_.find(user) == userEl4Key_.end()) {
            LOGI("Have not found user %{public}u el4 key, not enable el4", user);
            return -ENOENT;
        }
        eceSeceKeyPath = userEl4Key_[user]->GetDir();
    }
    return 0;
}

int KeyManager::UpdateCeEceSeceKeyContext(uint32_t userId, KeyType type)
{
    LOGI("start");
    if (!KeyCtrlHasFscryptSyspara()) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(keyMutex_);
    if (HasElkey(userId, type) == false) {
        LOGE("Have not found user %{public}u el%{public}u", userId, type);
        return -ENOENT;
    }
    std::shared_ptr<BaseKey> elKey = GetUserElKey(userId, type);
    if (elKey == nullptr) {
        LOGE("Have not found user %{public}u, type el%{public}u", userId, type);
        return -ENOENT;
    }
    if (!elKey->UpdateKey()) {
        LOGE("Basekey update newest context failed");
        return -EFAULT;
    }
    return 0;
}

int KeyManager::UpdateKeyContext(uint32_t userId)
{
    LOGI("UpdateKeyContext enter");
    int ret = UpdateCeEceSeceKeyContext(userId, EL2_KEY);
    if (ret != 0) {
        LOGE("Basekey update EL2 newest context failed");
        return ret;
    }
    ret = UpdateCeEceSeceKeyContext(userId, EL3_KEY);
    if (ret != 0) {
        LOGE("Basekey update EL3 newest context failed");
        return ret;
    }
    ret = UpdateCeEceSeceKeyContext(userId, EL4_KEY);
    if (ret != 0) {
        LOGE("Basekey update EL4 newest context failed");
        return ret;
    }
    if (IsUeceSupport() && saveESecretStatus[userId]) {
        ret = UpdateCeEceSeceKeyContext(userId, EL5_KEY);
    }
    if (ret != 0) {
        LOGE("Basekey update EL5 newest context failed");
        return ret;
    }
    LOGI("Basekey update key context success");
    return 0;
}

bool KeyManager::IsUeceSupport()
{
    int fd = open(UECE_PATH, O_RDWR);
    if (fd < 0) {
        if (errno == ENOENT) {
            LOGE("uece does not support !");
        }
        LOGE("open uece failed, errno : %{public}d", errno);
        return false;
    }
    close(fd);
    LOGI("uece is support.");
    return true;
}

int KeyManager::IsUeceSupportWithErrno()
{
    int fd = open(UECE_PATH, O_RDWR);
    if (fd < 0) {
        if (errno == ENOENT) {
            LOGE("uece does not support !");
            return ENOENT;
        }
        LOGE("open uece failed, errno : %{public}d", errno);
        return errno;
    }
    close(fd);
    LOGI("uece is support.");
    return E_OK;
}

int KeyManager::UpgradeKeys(const std::vector<FileList> &dirInfo)
{
    for (const auto &it : dirInfo) {
        std::shared_ptr<BaseKey> elKey = GetBaseKey(it.path);
        if (elKey == nullptr) {
            LOGE("Basekey memory failed");
            continue;
        }
        elKey->UpgradeKeys();
    }
    return 0;
}

int KeyManager::GetFileEncryptStatus(uint32_t userId, bool &isEncrypted)
{
    LOGI("Begin check encrypted status.");
    isEncrypted = true;
    const char rootPath[] = "/data/app/el2/";
    const char basePath[] = "/base";
    size_t allPathSize = strlen(rootPath) + strlen(basePath) + 1 + USER_ID_SIZE_VALUE;
    char *path = reinterpret_cast<char *>(malloc(sizeof(char) * (allPathSize)));
    if (path == nullptr) {
        LOGE("Failed to malloce path.");
        return -ENOENT;
    }
    int len = sprintf_s(path, allPathSize, "%s%u%s", rootPath, userId, basePath);
    if (len <= 0 || (size_t)len >= allPathSize) {
        LOGE("Failed to get base path");
        free(path);
        return -ENOENT;
    }
    if (access(path, F_OK) != 0) {
        free(path);
        LOGI("This is encrypted status");
        return E_OK;
    }
    free(path);
    if (!MountManager::GetInstance()->CheckMountFileByUser(userId)) {
        LOGI("The virturalDir is not exists.");
        return E_OK;
    }
    isEncrypted = false;
    LOGI("This is unencrypted status");
    return E_OK;
}

bool KeyManager::IsUserCeDecrypt(uint32_t userId)
{
    bool isCeEncrypt = false;
    int ret = GetFileEncryptStatus(userId, isCeEncrypt);
    if (ret != E_OK || isCeEncrypt) {
        LOGE("User %{public}d de has not decrypt.", userId);
        return false;
    }
    LOGI("User %{public}d de decrypted.", userId);
    return true;
}

void KeyManager::CheckAndClearTokenInfo(uint32_t user)
{
    bool isExist = false;
    if (IamClient::GetInstance().HasFaceFinger(user, isExist) == 0 && !isExist) {
        LOGI("Toke info is not exist.");
        if ((userEl3Key_.find(user) != userEl3Key_.end()) && (userEl3Key_[user] != nullptr)) {
            userEl3Key_[user]->ClearMemoryKeyCtx();
        }
        if ((userEl4Key_.find(user) != userEl4Key_.end()) && (userEl4Key_[user] != nullptr)) {
            userEl4Key_[user]->ClearMemoryKeyCtx();
        }
    }
}

int KeyManager::CheckUserPinProtect(unsigned int userId,
                                    const std::vector<uint8_t> &token,
                                    const std::vector<uint8_t> &secret)
{
    LOGI("enter CheckUserPinProtect");
    // judge if device has PIN protect
    if ((token.empty() && secret.empty()) && IamClient::GetInstance().HasPinProtect(userId)) {
        LOGE("User %{public}d has pin code protect.", userId);
        return E_ERR;
    }
    return E_OK;
}

int KeyManager::TryToFixUserCeEceSeceKey(unsigned int userId,
                                         KeyType keyType,
                                         const std::vector<uint8_t> &token,
                                         const std::vector<uint8_t> &secret)
{
    LOGI("enter TryToFixUserCeEceSeceKey");
    keyMutex_.unlock();
    if (!IamClient::GetInstance().HasPinProtect(userId)) {
        LOGE("User %{public}d has no pin code protect.", userId);
        return E_OK;
    }

    uint64_t secureUid = { 0 };
    if (!secret.empty() && !token.empty()) {
        IamClient::GetInstance().GetSecureUid(userId, secureUid);
        LOGE("Pin code is exist, get secure uid.");
    }
    UserAuth auth = { .token = token, .secret = secret, .secureUid = secureUid };
    UserTokenSecret userTokenSecret = { .token = token, .oldSecret = {}, .newSecret = secret, .secureUid = secureUid };

#ifdef USER_CRYPTO_MIGRATE_KEY
    if (UpdateCeEceSeceUserAuth(userId, userTokenSecret, keyType, false) != E_OK) {
#else
    if (UpdateCeEceSeceUserAuth(userId, userTokenSecret, keyType) != E_OK) {
#endif
        LOGE("try to fix elx key failed !");
        return -EFAULT;
    }
    if (UpdateCeEceSeceKeyContext(userId, keyType) != E_OK) {
        LOGE("try to fix elx key context failed !");
        return -EFAULT;
    }
    return E_OK;
}

int KeyManager::TryToFixUeceKey(unsigned int userId,
                                const std::vector<uint8_t> &token,
                                const std::vector<uint8_t> &secret)
{
    LOGI("enter TryToFixUeceKey");
    keyMutex_.unlock();
    if (!IamClient::GetInstance().HasPinProtect(userId)) {
        LOGE("User %{public}d has no pin code protect.", userId);
        return E_OK;
    }

    uint64_t secureUid = { 0 };
    if (!secret.empty() && !token.empty()) {
        IamClient::GetInstance().GetSecureUid(userId, secureUid);
        LOGE("Pin code is exist, get secure uid.");
    }
    UserAuth auth = { .token=token, .secret=secret, .secureUid = secureUid };
    UserTokenSecret tokenSecret = { .token = token, .oldSecret = { }, .newSecret = secret, .secureUid = secureUid};

    if (UpdateESecret(userId, tokenSecret) != E_OK) {
        LOGE("try to fix elx key failed !");
        return -EFAULT;
    }
    if (UpdateCeEceSeceKeyContext(userId, EL5_KEY) != E_OK) {
        LOGE("try to fix elx key context failed !");
        return -EFAULT;
    }
    return E_OK;
}

#ifdef USER_CRYPTO_MIGRATE_KEY
int KeyManager::RestoreUserKey(uint32_t userId, KeyType type)
{
    LOGI("start, user is %{public}u , type is %{public}d", userId, type);
    std::string dir = GetKeyDirByUserAndType(userId, type);
    if (dir == "") {
        LOGE("type is invalid, %{public}u", type);
        return -EFAULT;
    }

    if (!IsDir(dir)) {
        LOGE("dir not exist");
        return -ENOENT;
    }
    return RestoreUserKey(userId, dir, NULL_KEY_AUTH, type);
}
#endif
} // namespace StorageDaemon
} // namespace OHOS
