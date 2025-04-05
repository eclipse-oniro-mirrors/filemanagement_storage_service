/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "mock/storage_daemon_proxy_mock.h"
#include "storage_service_errno.h"

namespace OHOS {
namespace StorageDaemon {
StorageDaemonProxy::StorageDaemonProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IStorageDaemon>(impl)
{}

int32_t StorageDaemonProxy::Shutdown()
{
    return E_OK;
}

int32_t StorageDaemonProxy::Mount(const std::string &volId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UMount(const std::string &volId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::Check(const std::string &volId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::Format(const std::string &volId, const std::string &fsType)
{
    return E_OK;
}

int32_t StorageDaemonProxy::Partition(const std::string &diskId, int32_t type)
{
    return E_OK;
}

int32_t StorageDaemonProxy::SetVolumeDescription(const std::string &volId, const std::string &description)
{
    return E_OK;
}

int32_t StorageDaemonProxy::QueryUsbIsInUse(const std::string &diskPath, bool &isInUse)
{
    return E_OK;
}

int32_t StorageDaemonProxy::PrepareUserDirs(int32_t userId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageDaemonProxy::DestroyUserDirs(int32_t userId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageDaemonProxy::StartUser(int32_t userId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::StopUser(int32_t userId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::CompleteAddUser(int32_t userId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::InitGlobalKey(void)
{
    return E_OK;
}

int32_t StorageDaemonProxy::InitGlobalUserKeys(void)
{
    return E_OK;
}

int32_t StorageDaemonProxy::GenerateUserKeys(uint32_t userId, uint32_t flags)
{
    return E_OK;
}

int32_t StorageDaemonProxy::DeleteUserKeys(uint32_t userId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                                           const std::vector<uint8_t> &token,
                                           const std::vector<uint8_t> &oldSecret,
                                           const std::vector<uint8_t> &newSecret)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                                         const std::vector<uint8_t> &newSecret,
                                                         uint64_t secureUid,
                                                         uint32_t userId,
                                                         std::vector<std::vector<uint8_t>> &plainText)
{
    return E_OK;
}

int32_t StorageDaemonProxy::ResetSecretWithRecoveryKey(uint32_t userId,
                                                       uint32_t rkType,
                                                       const std::vector<uint8_t> &key)
{
    return E_OK;
}

int32_t StorageDaemonProxy::ActiveUserKey(uint32_t userId,
                                          const std::vector<uint8_t> &token,
                                          const std::vector<uint8_t> &secret)
{
    return E_OK;
}

int32_t StorageDaemonProxy::InactiveUserKey(uint32_t userId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::LockUserScreen(uint32_t userId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UnlockUserScreen(uint32_t userId,
                                             const std::vector<uint8_t> &token,
                                             const std::vector<uint8_t> &secret)
{
    return E_OK;
}

int32_t StorageDaemonProxy::GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus)
{
    return E_OK;
}

int32_t StorageDaemonProxy::GenerateAppkey(uint32_t userId, uint32_t hashId, std::string &keyId, bool needReSet)
{
    return E_OK;
}

int32_t StorageDaemonProxy::DeleteAppkey(uint32_t userId, const std::string &keyId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::CreateRecoverKey(uint32_t userId,
                                             uint32_t userType,
                                             const std::vector<uint8_t> &token,
                                             const std::vector<uint8_t> &secret)
{
    return E_OK;
}

int32_t StorageDaemonProxy::SetRecoverKey(const std::vector<uint8_t> &key)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UpdateKeyContext(uint32_t userId, bool needRemoveTmpKey)
{
    return E_OK;
}

int32_t StorageDaemonProxy::MountCryptoPathAgain(uint32_t userId)
{
    return E_OK;
}

std::vector<int32_t> StorageDaemonProxy::CreateShareFile(const std::vector<std::string> &uriList,
                                                         uint32_t tokenId, uint32_t flag)
{
    return {E_OK};
}

int32_t StorageDaemonProxy::DeleteShareFile(uint32_t tokenId, const std::vector<std::string> &uriList)
{
    return E_OK;
}

int32_t StorageDaemonProxy::SetBundleQuota(const std::string &bundleName, int32_t uid,
    const std::string &bundleDataDirPath, int32_t limitSizeMb)
{
    return E_OK;
}

int32_t StorageDaemonProxy::GetOccupiedSpace(int32_t idType, int32_t id, int64_t &size)
{
    return E_OK;
}

int32_t StorageDaemonProxy::GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
    const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
    std::vector<int64_t> &incPkgFileSizes)
{
    return E_OK;
}

int32_t StorageDaemonProxy::MountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UMountDfsDocs(int32_t userId, const std::string &relativePath,
    const std::string &networkId, const std::string &deviceId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UpdateMemoryPara(int32_t size, int32_t &oldSize)
{
    return E_OK;
}

int32_t StorageDaemonProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return E_OK;
}

int32_t StorageDaemonProxy::GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount)
{
    return E_OK;
}

int32_t StorageDaemonProxy::MountMediaFuse(int32_t userId, int32_t &devFd)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UMountMediaFuse(int32_t userId)
{
    return E_OK;
}

int32_t StorageDaemonProxy::GetUserNeedActiveStatus(uint32_t userId, bool &needActive)
{
    return E_OK;
}

int32_t StorageDaemonProxy::MountFileMgrFuse(int32_t userId, const std::string &path, int32_t &fuseFd)
{
    return E_OK;
}

int32_t StorageDaemonProxy::UMountFileMgrFuse(int32_t userId, const std::string &path)
{
    return E_OK;
}

int32_t StorageDaemonProxy::IsFileOccupied(const std::string &path, const std::vector<std::string> &inputList,
    std::vector<std::string> &outputList, bool &isOccupy)
{
    return E_OK;
}
} // StorageDaemon
} // OHOS
