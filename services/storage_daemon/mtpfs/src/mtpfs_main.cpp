/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <iostream>
#include "mtpfs_fuse.h"
#include "mtpfs_util.h"
#include "storage_service_log.h"

int main(int argc, char **argv)
{
    if (!DelayedSingleton<MtpFileSystem>::GetInstance()->ParseOptions(argc, argv)) {
        LOGE("mtpfs wrong usage, see %{public}s -h for details.", SmtpfsBaseName(argv[0]).c_str());
        std::cout << "mtpfs wrong usage, see " << SmtpfsBaseName(argv[0]) << " -h for details.\n";
        return 1;
    }

    bool success = DelayedSingleton<MtpFileSystem>::GetInstance()->Exec();
    if (!success) {
        LOGE("Exec mtpfs.bin to mount mtp device failed.");
        std::cout << "Exec mtpfs to mount mtp device failed.\n";
        return 1;
    }

    LOGI("Exec mtpfs.bin to mount mtp device success.");
    return 0;
}
