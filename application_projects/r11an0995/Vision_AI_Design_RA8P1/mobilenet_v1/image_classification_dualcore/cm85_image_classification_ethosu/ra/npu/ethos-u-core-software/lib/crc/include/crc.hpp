/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstddef>
#include <inttypes.h>

namespace {

class Crc {
public:
    constexpr Crc() : table() {
        uint32_t poly = 0xedb88320;

        for (uint32_t i = 0; i < 256; i++) {
            uint32_t crc = i;

            for (int j = 0; j < 8; j++) {
                if (crc & 1) {
                    crc = poly ^ (crc >> 1);
                } else {
                    crc >>= 1;
                }
            }

            table[i] = crc;
        }
    }

    uint32_t crc32(const void *data, const size_t length, uint32_t init = 0) const {
        uint32_t crc = init ^ 0xffffffff;

        const uint8_t *v = static_cast<const uint8_t *>(data);

        for (size_t i = 0; i < length; i++) {
            crc = table[(crc ^ v[i]) & 0xff] ^ (crc >> 8);
        }

        return crc ^ 0xffffffff;
    }

private:
    uint32_t table[256];
};
} // namespace
