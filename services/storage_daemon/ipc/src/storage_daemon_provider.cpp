/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ipc/storage_daemon_provider.h"
#include "securec.h"
#include "storage_service_errno.h"
#include "storage_service_log.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "utils/storage_radar.h"
#include "utils/storage_xcollie.h"
#include <cinttypes>
#ifdef EXTERNAL_STORAGE_MANAGER
#include "disk/disk_manager.h"
#include "volume/volume_manager.h"
#endif
#include "file_ex.h"
#include "hi_audit.h"
#include "user/mount_manager.h"
#include "utils/string_utils.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <fstream>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <thread>
#ifdef USER_CRYPTO_MANAGER
#include "crypto/app_clone_key_manager.h"
#include "crypto/iam_client.h"
#include "crypto/key_crypto_utils.h"
#include "crypto/key_manager.h"
#endif
#include "file_share.h"
#include "file_sharing/file_sharing.h"
#include "quota/quota_manager.h"
#include "user/user_manager.h"
namespace OHOS {
namespace StorageDaemon {
using namespace std;
constexpr unsigned int LOCAL_TIME_OUT_SECONDS = 10;
constexpr unsigned int INACTIVE_USER_KEY_OUT_SECONDS = 25;
constexpr unsigned int UPDATE_RADAR_STATISTIC_INTERVAL_SECONDS = 300;
constexpr unsigned int RADAR_REPORT_STATISTIC_INTERVAL_MINUTES = 1440;
constexpr unsigned int USER0ID = 0;
constexpr unsigned int USER100ID = 100;
constexpr unsigned int RADAR_STATISTIC_THREAD_WAIT_SECONDS = 60;
constexpr size_t MAX_IPC_RAW_DATA_SIZE = 128 * 1024 * 1024; // 128M

std::map<uint32_t, RadarStatisticInfo>::iterator StorageDaemonProvider::GetUserStatistics(const uint32_t userId)
{
    std::lock_guard<std::mutex> lock(mutexStats_);
    auto it = opStatistics_.find(userId);
    if (it != opStatistics_.end()) {
        return it;
    }
    RadarStatisticInfo radarInfo = {0};
    return opStatistics_.insert(make_pair(userId, radarInfo)).first;
}

void StorageDaemonProvider::GetTempStatistics(std::map<uint32_t, RadarStatisticInfo> &statistics)
{
    std::lock_guard<std::mutex> lock(mutexStats_);
    statistics.insert(opStatistics_.begin(), opStatistics_.end());
    opStatistics_.clear();
}

void StorageDaemonProvider::StorageRadarThd(void)
{
    // report radar statistics when restart
    std::unique_lock<std::mutex> lock(onRadarReportLock_);
    if (execRadarReportCon_.wait_for(lock, std::chrono::seconds(RADAR_STATISTIC_THREAD_WAIT_SECONDS),
                                     [this] { return this->stopRadarReport_.load(); })) {
        LOGI("Storage statistic radar exit.");
        return;
    }
    lock.unlock();
    LOGI("Storage statistic thread start.");
    StorageStatisticRadar::GetInstance().CreateStatisticFile();
    std::map<uint32_t, RadarStatisticInfo> opStatisticsTemp;
    StorageStatisticRadar::GetInstance().ReadStatisticFile(opStatisticsTemp);
    if (!opStatisticsTemp.empty()) {
        for (auto ele : opStatisticsTemp) {
            StorageService::StorageRadar::ReportStatistics(ele.first, ele.second);
        }
        StorageStatisticRadar::GetInstance().CleanStatisticFile();
    }
    lastRadarReportTime_ = std::chrono::system_clock::now();
    while (!stopRadarReport_.load()) {
        std::unique_lock<std::mutex> lock(onRadarReportLock_);
        if (execRadarReportCon_.wait_for(lock, std::chrono::seconds(UPDATE_RADAR_STATISTIC_INTERVAL_SECONDS),
                                         [this] { return this->stopRadarReport_.load(); })) {
            LOGI("Storage statistic radar exit.");
            return;
        }
        std::chrono::time_point<std::chrono::system_clock> nowTime = std::chrono::system_clock::now();
        int64_t intervalMinutes =
            std::chrono::duration_cast<std::chrono::minutes>(nowTime - lastRadarReportTime_).count();
        if ((intervalMinutes > RADAR_REPORT_STATISTIC_INTERVAL_MINUTES) && !opStatistics_.empty()) {
            LOGI("Storage statistic report, intervalMinutes:%{public}" PRId64, intervalMinutes);
            opStatisticsTemp.clear();
            GetTempStatistics(opStatisticsTemp);
            for (auto ele : opStatisticsTemp) {
                StorageService::StorageRadar::ReportStatistics(ele.first, ele.second);
            }
            lastRadarReportTime_ = std::chrono::system_clock::now();
            StorageStatisticRadar::GetInstance().CleanStatisticFile();
            continue;
        }
        if (!isNeedUpdateRadarFile_) {
            LOGD("Storage statistic not need update.");
            continue;
        }
        LOGI("Storage statistic update, intervalMinutes:%{public}" PRId64, intervalMinutes);
        isNeedUpdateRadarFile_ = false;
        StorageStatisticRadar::GetInstance().UpdateStatisticFile(opStatistics_);
    }
}

StorageDaemonProvider::StorageDaemonProvider()
{
    callRadarStatisticReportThread_ = std::thread([this]() { StorageRadarThd(); });
}

StorageDaemonProvider::~StorageDaemonProvider()
{
    std::unique_lock<std::mutex> lock(onRadarReportLock_);
    stopRadarReport_ = true;
    execRadarReportCon_.notify_one();
    lock.unlock();
    if (callRadarStatisticReportThread_.joinable()) {
        callRadarStatisticReportThread_.join();
    }
}

int32_t StorageDaemonProvider::Shutdown()
{
    return E_OK;
}

int32_t StorageDaemonProvider::Mount(const std::string &volId, uint32_t flags)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("Handle Mount");
    int32_t ret = VolumeManager::Instance()->Mount(volId, flags);
    if (ret != E_OK) {
        LOGW("Mount failed, please check");
        StorageService::StorageRadar::ReportVolumeOperation("VolumeManager::Mount", ret);
        AuditLog storageAuditLog = {false, "FAILED TO Mount", "ADD", "Mount", 1, "FAIL"};
        HiAudit::GetInstance().Write(storageAuditLog);
    } else {
        AuditLog storageAuditLog = {false, "SUCCESS TO Mount", "ADD", "Mount", 1, "SUCCESS"};
        HiAudit::GetInstance().Write(storageAuditLog);
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::UMount(const std::string &volId)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("Handle UMount");
    int32_t ret = VolumeManager::Instance()->UMount(volId);
    if (ret != E_OK) {
        LOGW("UMount failed, please check");
        StorageService::StorageRadar::ReportVolumeOperation("VolumeManager::UMount", ret);
        AuditLog storageAuditLog = {false, "FAILED TO UMount", "DEL", "UMount", 1, "FAIL"};
        HiAudit::GetInstance().Write(storageAuditLog);
    } else {
        AuditLog storageAuditLog = {false, "SUCCESS TO UMount", "DEL", "UMount", 1, "SUCCESS"};
        HiAudit::GetInstance().Write(storageAuditLog);
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::TryToFix(const std::string &volId, uint32_t flags)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("Handle TryToFix");
    int32_t ret = VolumeManager::Instance()->TryToFix(volId, flags);
    if (ret != E_OK) {
        LOGW("TryToFix failed, please check");
        StorageService::StorageRadar::ReportVolumeOperation("VolumeManager::TryToFix", ret);
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::Check(const std::string &volId)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("Handle Check");
    int32_t ret = VolumeManager::Instance()->Check(volId);
    if (ret != E_OK) {
        LOGW("Check failed, please check");
        StorageService::StorageRadar::ReportVolumeOperation("VolumeManager::Check", ret);
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::Format(const std::string &volId, const std::string &fsType)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("Handle Format");
    int32_t ret = VolumeManager::Instance()->Format(volId, fsType);
    if (ret != E_OK) {
        LOGW("Format failed, please check");
        StorageService::StorageRadar::ReportVolumeOperation("VolumeManager::Format", ret);
        AuditLog storageAuditLog = {true, "FAILED TO Format", "UPDATE", "Format", 1, "FAIL"};
        HiAudit::GetInstance().Write(storageAuditLog);
    } else {
        AuditLog storageAuditLog = {true, "SUCCESS TO Format", "UPDATE", "Format", 1, "SUCCESS"};
        HiAudit::GetInstance().Write(storageAuditLog);
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::Partition(const std::string &diskId, int32_t type)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("Handle Partition");
    int32_t ret = DiskManager::Instance()->HandlePartition(diskId);
    if (ret != E_OK) {
        LOGW("HandlePartition failed, please check");
        StorageService::StorageRadar::ReportVolumeOperation("VolumeManager::Partition", ret);
        AuditLog storageAuditLog = {true, "FAILED TO Partition", "UPDATE", "Partition", 1, "FAIL"};
        HiAudit::GetInstance().Write(storageAuditLog);
    } else {
        AuditLog storageAuditLog = {true, "SUCCESS TO Partition", "UPDATE", "Partition", 1, "SUCCESS"};
        HiAudit::GetInstance().Write(storageAuditLog);
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::SetVolumeDescription(const std::string &volId, const std::string &description)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("Handle SetVolumeDescription");
    int32_t ret = VolumeManager::Instance()->SetVolumeDescription(volId, description);
    if (ret != E_OK) {
        LOGW("SetVolumeDescription failed, please check");
        StorageService::StorageRadar::ReportVolumeOperation("VolumeManager::SetVolumeDescription", ret);
        AuditLog storageAuditLog = {true,  "FAILED TO SetVolumeDescription", "UPDATE", "SetVolumeDescription", 1,
                                    "FAIL"};
        HiAudit::GetInstance().Write(storageAuditLog);
    } else {
        AuditLog storageAuditLog = {true,     "SUCCESS TO SetVolumeDescription", "UPDATE", "SetVolumeDescription", 1,
                                    "SUCCESS"};
        HiAudit::GetInstance().Write(storageAuditLog);
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::QueryUsbIsInUse(const std::string &diskPath, bool &isInUse)
{
#ifdef EXTERNAL_STORAGE_MANAGER
    LOGI("StorageDaemon::QueryUsbIsInUse diskPath: %{public}s", diskPath.c_str());
    isInUse = true;
    return VolumeManager::Instance()->QueryUsbIsInUse(diskPath, isInUse);
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t StorageDaemonProvider::StartUser(int32_t userId)
{
    auto startTime = StorageService::StorageRadar::RecordCurrentTime();
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    (void)StorageDaemon::GetInstance()->SetPriority();  // set tid priority to 40
    int32_t ret = UserManager::GetInstance()->StartUser(userId);
    if (ret != E_OK && ret != E_KEY_NOT_ACTIVED) {
        LOGE("StartUser failed, please check");
        StorageService::StorageRadar::ReportUserManager("StartUser", userId, ret, "");
        AuditLog storageAuditLog = { false, "FAILED TO StartUser", "ADD", "StartUser", 1, "FAIL" };
        HiAudit::GetInstance().Write(storageAuditLog);
    } else {
        AuditLog storageAuditLog = { false, "SUCCESS TO StartUser", "ADD", "StartUser", 1, "SUCCESS" };
        HiAudit::GetInstance().Write(storageAuditLog);
    }
    if (ret != E_OK) {
        it->second.userStartFailCount++;
    } else {
        it->second.userStartSuccCount++;
    }
    auto delay = StorageService::StorageRadar::ReportDuration("START USER",
        startTime, StorageService::DELAY_TIME_THRESH_HIGH, userId);
    LOGI("SD_DURATION: START USER: delay time = %{public}s", delay.c_str());
    return ret;
}

int32_t StorageDaemonProvider::StopUser(int32_t userId)
{
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    int32_t ret = UserManager::GetInstance()->StopUser(userId);
    LOGE("StopUser end, ret is %{public}d.", ret);
    StorageService::StorageRadar::ReportUserManager("StopUser", userId, ret, "");
    std::string status = ret == E_OK ? "SUCCESS" : "FAIL";
    std::string cause = ret == E_OK ? "SUCCESS TO StopUser" : "FAILED TO StopUser";
    AuditLog storageAuditLog = { false, cause, "DEL", "StopUser", 1, status };
    HiAudit::GetInstance().Write(storageAuditLog);
    if (ret != E_OK) {
        it->second.userStopFailCount++;
    } else {
        it->second.userStopSuccCount++;
    }
    return ret;
}

int32_t StorageDaemonProvider::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    int32_t err = StorageDaemon::GetInstance()->PrepareUserDirs(userId, flags);
    if (err != E_OK) {
        it->second.userAddFailCount++;
    } else {
        it->second.userAddSuccCount++;
    }
    return err;
}

int32_t StorageDaemonProvider::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    int err = StorageDaemon::GetInstance()->DestroyUserDirs(userId, flags);
    if (err != E_OK) {
        it->second.userRemoveFailCount++;
    } else {
        it->second.userRemoveSuccCount++;
    }
    return err;
}

int32_t StorageDaemonProvider::CompleteAddUser(int32_t userId)
{
    int32_t err = StorageDaemon::GetInstance()->CompleteAddUser(userId);
    return err;
}

int32_t StorageDaemonProvider::InitGlobalKey()
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(USER0ID);
    isNeedUpdateRadarFile_ = true;
    int err = StorageDaemon::GetInstance()->InitGlobalKey();
    if (err != E_OK) {
        it->second.keyLoadFailCount++;
    } else {
        it->second.keyLoadSuccCount++;
    }
    return err;
}

int32_t StorageDaemonProvider::InitGlobalUserKeys()
{
    LOGI("StorageDaemonProvider_InitGlobalUserKeys start.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(USER100ID);
    isNeedUpdateRadarFile_ = true;
    int32_t err = StorageDaemon::GetInstance()->InitGlobalUserKeys();
    if (err != E_OK) {
        it->second.keyLoadFailCount++;
    } else {
        it->second.keyLoadSuccCount++;
    }
    return err;
}

int32_t StorageDaemonProvider::GenerateUserKeys(uint32_t userId, uint32_t flags)
{
    int timerId = StorageXCollie::SetTimer("storage:GenerateUserKeys", LOCAL_TIME_OUT_SECONDS);
    int err = StorageDaemon::GetInstance()->GenerateUserKeys(userId, flags);
    StorageXCollie::CancelTimer(timerId);
    return err;
}

int32_t StorageDaemonProvider::DeleteUserKeys(uint32_t userId)
{
    int timerId = StorageXCollie::SetTimer("storage:DeleteUserKeys", LOCAL_TIME_OUT_SECONDS);
    int err = StorageDaemon::GetInstance()->DeleteUserKeys(userId);
    StorageXCollie::CancelTimer(timerId);
    return err;
}

int32_t StorageDaemonProvider::UpdateUserAuth(uint32_t userId,
                                              uint64_t secureUid,
                                              const std::vector<uint8_t> &token,
                                              const std::vector<uint8_t> &oldSecret,
                                              const std::vector<uint8_t> &newSecret)
{
    int timerId = StorageXCollie::SetTimer("storage:UpdateUserAuth", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    int err =  StorageDaemon::GetInstance()->UpdateUserAuth(userId, secureUid, token, oldSecret, newSecret);
    StorageXCollie::CancelTimer(timerId);
    return err;
}

int32_t StorageDaemonProvider::UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                                            const std::vector<uint8_t> &newSecret,
                                                            uint64_t secureUid,
                                                            uint32_t userId,
                                                            const std::vector<std::vector<uint8_t>> &plainText)
{
    return StorageDaemon::GetInstance()->UpdateUseAuthWithRecoveryKey(authToken, newSecret, secureUid, userId,
                                                                      plainText);
}

int32_t StorageDaemonProvider::ActiveUserKey(uint32_t userId,
                                             const std::vector<uint8_t> &token,
                                             const std::vector<uint8_t> &secret)
{
    auto startTime = StorageService::StorageRadar::RecordCurrentTime();
    int timerId = StorageXCollie::SetTimer("storage:ActiveUserKey", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    int32_t err = StorageDaemon::GetInstance()->ActiveUserKey(userId, token, secret);
    StorageXCollie::CancelTimer(timerId);
    if ((err == E_OK) || ((err == E_ACTIVE_EL2_FAILED) && token.empty() && secret.empty())) {
        it->second.keyLoadSuccCount++;
    } else {
        it->second.keyLoadFailCount++;
    }
    auto delay = StorageService::StorageRadar::ReportDuration("ACTIVE USER KEY",
        startTime, StorageService::DELAY_TIME_THRESH_HIGH, userId);
    LOGI("SD_DURATION: ACTIVE USER KEY: delay time = %{public}s", delay.c_str());
    return err;
}

int32_t StorageDaemonProvider::InactiveUserKey(uint32_t userId)
{
    int timerId = StorageXCollie::SetTimer("storage:InactiveUserKey", INACTIVE_USER_KEY_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    int32_t ret = StorageDaemon::GetInstance()->InactiveUserKey(userId);
    StorageXCollie::CancelTimer(timerId);
    if (ret != E_OK) {
        it->second.keyUnloadFailCount++;
    } else {
        it->second.keyUnloadSuccCount++;
    }
    return ret;
}

int32_t StorageDaemonProvider::UpdateKeyContext(uint32_t userId, bool needRemoveTmpKey)
{
    int timerId = StorageXCollie::SetTimer("storage:UpdateKeyContext", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = StorageDaemon::GetInstance()->UpdateKeyContext(userId, needRemoveTmpKey);
    StorageXCollie::CancelTimer(timerId);
    return ret;
}

int32_t StorageDaemonProvider::MountCryptoPathAgain(uint32_t userId)
{
    LOGI("begin to MountCryptoPathAgain");
#ifdef USER_CRYPTO_MANAGER
    auto ret = MountManager::GetInstance()->MountCryptoPathAgain(userId);
    if (ret != E_OK) {
        StorageService::StorageRadar::ReportUserManager("MountCryptoPathAgain::MountManager::MountCryptoPathAgain",
                                                        userId, ret, "");
    }
    return ret;
#else
    return E_OK;
#endif
}

int32_t StorageDaemonProvider::LockUserScreen(uint32_t userId)
{
    int timerId = StorageXCollie::SetTimer("storage:LockUserScreen", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    int ret = StorageDaemon::GetInstance()->LockUserScreen(userId);
    StorageXCollie::CancelTimer(timerId);
    if (ret != E_OK) {
        it->second.keyUnloadFailCount++;
    } else {
        it->second.keyUnloadSuccCount++;
    }
    return ret;
}

int32_t StorageDaemonProvider::UnlockUserScreen(uint32_t userId,
                                                const std::vector<uint8_t> &token,
                                                const std::vector<uint8_t> &secret)
{
    int timerId = StorageXCollie::SetTimer("storage:UnlockUserScreen", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetUserStatistics(userId);
    isNeedUpdateRadarFile_ = true;
    int ret = StorageDaemon::GetInstance()->UnlockUserScreen(userId, token, secret);
    StorageXCollie::CancelTimer(timerId);
    if (ret != E_OK) {
        it->second.keyLoadFailCount++;
    } else {
        it->second.keyLoadSuccCount++;
    }
    return ret;
}

int32_t StorageDaemonProvider::GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus)
{
    lockScreenStatus = false;
    int timerId = StorageXCollie::SetTimer("storage:GetLockScreenStatus", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = StorageDaemon::GetInstance()->GetLockScreenStatus(userId, lockScreenStatus);
    StorageXCollie::CancelTimer(timerId);
    return ret;
}

int32_t StorageDaemonProvider::GenerateAppkey(uint32_t userId, uint32_t hashId, std::string &keyId, bool needReSet)
{
    int timerId = StorageXCollie::SetTimer("storage:GenerateAppkey", LOCAL_TIME_OUT_SECONDS);
    int32_t ret = StorageDaemon::GetInstance()->GenerateAppkey(userId, hashId, keyId, needReSet);
    StorageXCollie::CancelTimer(timerId);
    return ret;
}

int32_t StorageDaemonProvider::DeleteAppkey(uint32_t userId, const std::string &keyId)
{
    int timerId = StorageXCollie::SetTimer("storage:DeleteAppkey", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = StorageDaemon::GetInstance()->DeleteAppkey(userId, keyId);
    StorageXCollie::CancelTimer(timerId);
    return ret;
}

int32_t StorageDaemonProvider::CreateRecoverKey(uint32_t userId,
                                                uint32_t userType,
                                                const std::vector<uint8_t> &token,
                                                const std::vector<uint8_t> &secret)
{
    return StorageDaemon::GetInstance()->CreateRecoverKey(userId, userType, token, secret);
}

int32_t StorageDaemonProvider::SetRecoverKey(const std::vector<uint8_t> &key)
{
    return StorageDaemon::GetInstance()->SetRecoverKey(key);
}

int32_t StorageDaemonProvider::RawDataToStringVec(const StorageFileRawData &rawData,
    std::vector<std::string> &stringVec)
{
    if (rawData.data == nullptr) {
        LOGE("rawData is null");
        return ERR_DEAD_OBJECT;
    }
    if (rawData.size == 0 || rawData.size > MAX_IPC_RAW_DATA_SIZE) {
        LOGE("size invalid: %{public}u", rawData.size);
        return ERR_DEAD_OBJECT;
    }
    std::stringstream ss;
    ss.write(reinterpret_cast<const char *>(rawData.data), rawData.size);
    uint32_t stringVecSize = 0;
    ss.read(reinterpret_cast<char *>(&stringVecSize), sizeof(stringVecSize));
    uint32_t ssLength = static_cast<uint32_t>(ss.str().length());
    for (uint32_t i = 0; i < stringVecSize; i++) {
        uint32_t strLen = 0;
        ss.read(reinterpret_cast<char *>(&strLen), sizeof(strLen));
        if (strLen > ssLength - static_cast<uint32_t>(ss.tellg())) {
            LOGE("string length:%{public}u is invalid", strLen);
            return ERR_DEAD_OBJECT;
        }
        std::string str;
        str.resize(strLen);
        ss.read(&str[0], strLen);
        stringVec.emplace_back(str);
    }
    return ERR_OK;
}

int32_t StorageDaemonProvider::CreateShareFile(const StorageFileRawData &rawData,
                                               uint32_t tokenId,
                                               uint32_t flag,
                                               std::vector<int32_t> &funcResult)
{
    LOGI("Create Share file list len is %{public}u", rawData.size);
    funcResult.clear();
    std::vector<std::string> uriList;
    RawDataToStringVec(rawData, uriList);
    LOGI("StorageDaemonProvider::CreateShareFile start. file size is %{public}zu", uriList.size());
    AppFileService::FileShare::CreateShareFile(uriList, tokenId, flag, funcResult);
    LOGI("StorageDaemonProvider::CreateShareFile end. result size is %{public}zu", funcResult.size());
    return E_OK;
}

int32_t StorageDaemonProvider::DeleteShareFile(uint32_t tokenId, const StorageFileRawData &rawData)
{
    std::vector<std::string> uriList;
    RawDataToStringVec(rawData, uriList);
    return AppFileService::FileShare::DeleteShareFile(tokenId, uriList);
}

int32_t StorageDaemonProvider::SetBundleQuota(const std::string &bundleName,
                                              int32_t uid,
                                              const std::string &bundleDataDirPath,
                                              int32_t limitSizeMb)
{
    return QuotaManager::GetInstance()->SetBundleQuota(bundleName, uid, bundleDataDirPath, limitSizeMb);
}

int32_t StorageDaemonProvider::GetOccupiedSpace(int32_t idType, int32_t id, int64_t &size)
{
    size = 0;
    return QuotaManager::GetInstance()->GetOccupiedSpace(idType, id, size);
}

int32_t StorageDaemonProvider::UpdateMemoryPara(int32_t size, int32_t &oldSize)
{
    oldSize = 0;
    return E_OK;
}

int32_t StorageDaemonProvider::MountDfsDocs(int32_t userId,
                                            const std::string &relativePath,
                                            const std::string &networkId,
                                            const std::string &deviceId)
{
    LOGI("StorageDaemon::MountDfsDocs start.");
    return MountManager::GetInstance()->MountDfsDocs(userId, relativePath, networkId, deviceId);
}

int32_t StorageDaemonProvider::UMountDfsDocs(int32_t userId,
                                             const std::string &relativePath,
                                             const std::string &networkId,
                                             const std::string &deviceId)
{
    LOGI("StorageDaemon::UMountDfsDocs start.");
    return MountManager::GetInstance()->UMountDfsDocs(userId, relativePath, networkId, deviceId);
}

int32_t StorageDaemonProvider::GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount)
{
    isEncrypted = true;
    int timerId = StorageXCollie::SetTimer("storage:GetFileEncryptStatus", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = StorageDaemon::GetInstance()->GetFileEncryptStatus(userId, isEncrypted, needCheckDirMount);
    StorageXCollie::CancelTimer(timerId);
    return ret;
}

int32_t StorageDaemonProvider::GetUserNeedActiveStatus(uint32_t userId, bool &needActive)
{
    needActive = false;
    int timerId = StorageXCollie::SetTimer("storage:GetUserNeedActiveStatus", LOCAL_TIME_OUT_SECONDS);
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = StorageDaemon::GetInstance()->GetUserNeedActiveStatus(userId, needActive);
    StorageXCollie::CancelTimer(timerId);
    return ret;
}

int32_t StorageDaemonProvider::MountMediaFuse(int32_t userId, int32_t &devFd)
{
#ifdef STORAGE_SERVICE_MEDIA_FUSE
    LOGI("StorageDaemonProvider::MountMediaFuse start.");
    devFd = -1;
    return MountManager::GetInstance()->MountMediaFuse(userId, devFd);
#endif
    return E_OK;
}

int32_t StorageDaemonProvider::UMountMediaFuse(int32_t userId)
{
#ifdef STORAGE_SERVICE_MEDIA_FUSE
    LOGI("StorageDaemonProvider::UMountMediaFuse start.");
    return MountManager::GetInstance()->UMountMediaFuse(userId);
#endif
    return E_OK;
}

int32_t StorageDaemonProvider::MountFileMgrFuse(int32_t userId, const std::string &path, int32_t &fuseFd)
{
    LOGI("StorageDaemonProvider::MountFileMgrFuse, userId:%{public}d, path:%{public}s", userId, path.c_str());
    fuseFd = -1;
    return MountManager::GetInstance()->MountFileMgrFuse(userId, path, fuseFd);
}

int32_t StorageDaemonProvider::UMountFileMgrFuse(int32_t userId, const std::string &path)
{
    LOGI("StorageDaemonProvider::MountFileMgrFuse, userId:%{public}d, path:%{public}s", userId, path.c_str());
    return MountManager::GetInstance()->UMountFileMgrFuse(userId, path);
}

int32_t StorageDaemonProvider::IsFileOccupied(const std::string &path,
                                              const std::vector<std::string> &inputList,
                                              std::vector<std::string> &outputList,
                                              bool &isOccupy)
{
    return StorageDaemon::GetInstance()->IsFileOccupied(path, inputList, outputList, isOccupy);
}

int32_t StorageDaemonProvider::ResetSecretWithRecoveryKey(uint32_t userId,
                                                          uint32_t rkType,
                                                          const std::vector<uint8_t> &key)
{
    return StorageDaemon::GetInstance()->ResetSecretWithRecoveryKey(userId, rkType, key);
}

void StorageDaemonProvider::SystemAbilityStatusChangeListener::OnAddSystemAbility(int32_t systemAbilityId,
                                                                                  const std::string &deviceId)
{
    LOGI("SystemAbilityId:%{public}d", systemAbilityId);
#ifdef EXTERNAL_STORAGE_MANAGER
    if (systemAbilityId == ACCESS_TOKEN_MANAGER_SERVICE_ID) {
        DiskManager::Instance()->ReplayUevent();
    }
#endif
    if (systemAbilityId == FILEMANAGEMENT_CLOUD_DAEMON_SERVICE_SA_ID) {
        MountManager::GetInstance()->SetCloudState(true);
    }
}

void StorageDaemonProvider::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(int32_t systemAbilityId,
                                                                                     const std::string &deviceId)
{
    LOGI("SystemAbilityId:%{public}d", systemAbilityId);
    if (systemAbilityId == FILEMANAGEMENT_CLOUD_DAEMON_SERVICE_SA_ID) {
        MountManager::GetInstance()->SetCloudState(false);
    }
}

int32_t StorageDaemonProvider::MountDisShareFile(int32_t userId, const std::map<std::string, std::string> &shareFiles)
{
    return MountManager::GetInstance()->MountDisShareFile(userId, shareFiles);
}

int32_t StorageDaemonProvider::UMountDisShareFile(int32_t userId, const std::string &networkId)
{
    return MountManager::GetInstance()->UMountDisShareFile(userId, networkId);
}

int32_t StorageDaemonProvider::InactiveUserPublicDirKey(uint32_t userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return StorageDaemon::GetInstance()->InactiveUserPublicDirKey(userId);
}
} // namespace StorageDaemon
} // namespace OHOS
