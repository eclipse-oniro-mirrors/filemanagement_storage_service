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
#ifndef MOCK_STORAGE_DAEMON_SERVICE_H
#define MOCK_STORAGE_DAEMON_SERVICE_H

#include "gmock/gmock.h"
#include "ipc/storage_daemon_stub.h"
#include "storage_service_errno.h"

namespace OHOS {
namespace StorageDaemon {
class StorageDaemonServiceMock :  public IRemoteStub<IStorageDaemon> {
public:
    int code_ = 0;
    StorageDaemonServiceMock() : code_(0) {}
    virtual ~StorageDaemonServiceMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));
    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return E_OK;
    }

    virtual int32_t Shutdown() override
    {
        return E_OK;
    }

    virtual int32_t Mount(const std::string &volId, uint32_t flags) override
    {
        return E_OK;
    }

    virtual int32_t UMount(const std::string &volId) override
    {
        return E_OK;
    }

    virtual int32_t Check(const std::string &volId) override
    {
        return E_OK;
    }

    virtual int32_t Format(const std::string &volId, const std::string &fsType) override
    {
        return E_OK;
    }

    virtual int32_t Partition(const std::string &diskId, int32_t type) override
    {
        return E_OK;
    }

    virtual int32_t SetVolumeDescription(const std::string &volId, const std::string &description) override
    {
        return E_OK;
    }

    virtual int32_t StartUser(int32_t userId) override
    {
        return E_OK;
    }

    virtual int32_t StopUser(int32_t userId) override
    {
        return E_OK;
    }

    virtual int32_t CompleteAddUser(int32_t userId) override
    {
        return E_OK;
    }

    virtual int32_t MountDfsDocs(int32_t userId, const std::string &relativePath,
        const std::string &networkId, const std::string &deviceId) override
    {
        return E_OK;
    }

    virtual int32_t UMountDfsDocs(int32_t userId, const std::string &relativePath,
        const std::string &networkId, const std::string &deviceId) override
    {
        return E_OK;
    }

    virtual int32_t PrepareUserDirs(int32_t userId, uint32_t flags) override
    {
        return E_OK;
    }

    virtual int32_t DestroyUserDirs(int32_t userId, uint32_t flags) override
    {
        return E_OK;
    }

    virtual int32_t InitGlobalKey(void) override
    {
        return E_OK;
    }

    virtual int32_t InitGlobalUserKeys(void) override
    {
        return E_OK;
    }

    virtual int32_t GenerateUserKeys(uint32_t userId, uint32_t flags) override
    {
        return E_OK;
    }
    virtual int32_t DeleteUserKeys(uint32_t userId) override
    {
        return E_OK;
    }

    virtual int32_t UpdateUserAuth(uint32_t userId, uint64_t secureUid,
                                   const std::vector<uint8_t> &token,
                                   const std::vector<uint8_t> &oldSecret,
                                   const std::vector<uint8_t> &newSecret) override
    {
        return E_OK;
    }

    virtual int32_t UpdateUseAuthWithRecoveryKey(const std::vector<uint8_t> &authToken,
                                                 const std::vector<uint8_t> &newSecret,
                                                 uint64_t secureUid,
                                                 uint32_t userId,
                                                 std::vector<std::vector<uint8_t>> &plainText)
    {
        return E_OK;
    }

    virtual int32_t ActiveUserKey(uint32_t userId,
                                  const std::vector<uint8_t> &token,
                                  const std::vector<uint8_t> &secret) override
    {
        return E_OK;
    }

    virtual int32_t InactiveUserKey(uint32_t userId) override
    {
        return E_OK;
    }

    virtual int32_t LockUserScreen(uint32_t userId) override
    {
        return E_OK;
    }

    virtual int32_t UnlockUserScreen(uint32_t userId,
                                     const std::vector<uint8_t> &token,
                                     const std::vector<uint8_t> &secret) override
    {
        return E_OK;
    }

    virtual int32_t GetLockScreenStatus(uint32_t userId, bool &lockScreenStatus) override
    {
        return E_OK;
    }

    virtual int32_t GenerateAppkey(uint32_t userId, uint32_t hashId, std::string &keyId) override
    {
        return E_OK;
    }

    virtual int32_t DeleteAppkey(uint32_t userId, const std::string &keyId) override
    {
        return E_OK;
    }

    virtual int32_t CreateRecoverKey(uint32_t userId,
                                     uint32_t userType,
                                     const std::vector<uint8_t> &token,
                                     const std::vector<uint8_t> &secret) override
    {
        return E_OK;
    }

    virtual int32_t SetRecoverKey(const std::vector<uint8_t> &key) override
    {
        return E_OK;
    }

    virtual int32_t UpdateKeyContext(uint32_t userId, bool needRemoveTmpKey = false) override
    {
        return E_OK;
    }

    virtual int32_t MountCryptoPathAgain(uint32_t userId) override
    {
        return E_OK;
    }

    virtual std::vector<int32_t> CreateShareFile(const std::vector<std::string> &uriList,
                                                uint32_t tokenId, uint32_t flag) override
    {
        return std::vector<int32_t>{E_OK};
    }

    virtual int32_t DeleteShareFile(uint32_t tokenId, const std::vector<std::string> &uriList) override
    {
        return E_OK;
    }

    virtual int32_t GetOccupiedSpace(int32_t idType, int32_t id, int64_t &size) override
    {
        return E_OK;
    }
    virtual int32_t UpdateMemoryPara(int32_t size, int32_t &oldSize) override
    {
        return E_OK;
    }
    virtual int32_t GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
        const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes,
        std::vector<int64_t> &incPkgFileSizes) override
    {
        return E_OK;
    }
    virtual int32_t GetFileEncryptStatus(uint32_t userId, bool &isEncrypted, bool needCheckDirMount = false) override
    {
        return E_OK;
    }

    virtual int32_t MountMediaFuse(int32_t userId, int32_t &devFd) override
    {
        return E_OK;
    }

    virtual int32_t UMountMediaFuse(int32_t userId) override
    {
        return E_OK;
    }

    virtual int32_t GetUserNeedActiveStatus(uint32_t userId, bool &needActive) override
    {
        return E_OK;
    }
};
} // namespace StorageDaemon
} // namespace OHOS
#endif // MOCK_STORAGE_DAEMON_SERVICE_H