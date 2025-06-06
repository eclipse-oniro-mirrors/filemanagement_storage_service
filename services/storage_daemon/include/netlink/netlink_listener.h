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
#ifndef OHOS_STORAGE_DAEMON_NETLINK_LISTENER_H
#define OHOS_STORAGE_DAEMON_NETLINK_LISTENER_H

#include <poll.h>
#include <cstdint>
#include <memory>
#include <thread>

namespace OHOS {
namespace StorageDaemon {
class NetlinkListener {
public:
    NetlinkListener(int32_t socket);
    int32_t StartListener();
    int32_t StopListener();

protected:
    virtual void OnEvent(char *msg) = 0;

private:
    int32_t socketFd_ { -1 };
    int32_t socketPipe_[2] { -1, -1 };
    std::unique_ptr<std::thread> socketThread_;
    void RecvUeventMsg();
    int32_t ReadMsg(int32_t fd_count, struct pollfd ufds[2]);
    void RunListener();
    static void EventProcess(void*);
};
} // STORAGE_DAEMON
} // OHOS

#endif // OHOS_STORAGE_DAEMON_NETLINK_LISTENER_H
