#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace zf3 {
    using Byte = unsigned char;

    constexpr int kilobytes_to_bytes(int kb) {
        return kb << 10;
    }

    constexpr int megabytes_to_bytes(int mb) {
        return mb << 20;
    }

    constexpr int gigabytes_to_bytes(int gb) {
        return gb << 30;
    }

    constexpr int bits_to_bytes(int bits) {
        return ((bits + 7) & ~7) / 8;
    }

    char* get_file_contents(const char* const filename);
}
