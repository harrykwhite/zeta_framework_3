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

    template<typename T>
    T* alloc(const int cnt = 1) {
        return static_cast<T*>(malloc(sizeof(T) * cnt));
    }

    template<typename T>
    T* alloc_zeroed(const int cnt = 1) {
        return static_cast<T*>(calloc(cnt, sizeof(T)));
    }

    template<typename T>
    void zero_out(T& data) {
        memset(&data, 0, sizeof(data));
    }

    template<typename T>
    bool is_zero(const T& data) {
        const auto dataBytes = reinterpret_cast<const Byte*>(&data);

        for (int i = 0; i < sizeof(T); ++i) {
            if (dataBytes[i]) {
                return false;
            }
        }

        return true;
    }

    char* get_file_contents(const char* const filename);
}
