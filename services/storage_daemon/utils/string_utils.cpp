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

#include "utils/string_utils.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

using namespace std;

namespace OHOS {
namespace StorageDaemon{
bool IsEndWith(const string &str, const string &end)
{
    size_t stringLen = str.size();
    size_t endLen = end.size();

    if (stringLen < endLen) {
        return false;
    }

    for (uint32_t i = stringLen; i > stringLen - endLen; i--) {
        if (str[i] != end[i]) {
            return false;
        }
    }

    return true;
}


std::string StringPrintf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    std::string result;
    char buf[BUFF_SIZE] = {0};
    int count = vsnprintf(buf, BUFF_SIZE, format, ap);
    if (count >= 0 && count < BUFF_SIZE) {
        result.append(buf, count);
    }

    va_end(ap);

    return result;
}
} // namespace StorageDaemon
} // namespace OHOS
