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

#include "mtpfs_libmtp.h"

#include <cstdlib>

extern "C" {
void LIBMTPFreeFilesAndFolders(LIBMTP_file_t **files)
{
    if (files == nullptr || *files == nullptr) {
        return;
    }
    LIBMTP_file_t *f = *files;
    while (f != nullptr) {
        LIBMTP_file_t *tmp = f;
        char *filename = f->filename;
        f = f->next;
        free(static_cast<void *>(filename));
        free(static_cast<void *>(tmp));
    }
    *files = nullptr;
}
}
