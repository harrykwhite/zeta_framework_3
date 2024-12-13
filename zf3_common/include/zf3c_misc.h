#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace zf3 {

using Byte = unsigned char;

struct MemArena {
    Byte* buf;
    int size;
    int used;
};

constexpr int conv_kilobytes_to_bytes(int x) {
    return x << 10;
}

constexpr int conv_megabytes_to_bytes(int x) {
    return x << 20;
}

constexpr int conv_gigabytes_to_bytes(int x) {
    return x << 30;
}

constexpr int conv_bits_to_bytes(int x) {
    return ((x + 7) & ~7) / 8;
}

bool init_mem_arena(MemArena* const memArena, const int size);
void clean_mem_arena(MemArena* const memArena);
void reset_mem_arena(MemArena* const memArena);

template<typename T>
T* push_to_mem_arena(MemArena* const memArena, const int cnt = 1) {
    const int size = sizeof(T) * cnt;

    if (memArena->used + size > memArena->size) {
        return NULL;
    }

    const auto ptr = reinterpret_cast<T*>(memArena->buf + memArena->used);
    memArena->used += size;
    return ptr;
}

template<typename T>
bool is_zero(const T* const data) {
    const auto dataBytes = reinterpret_cast<const Byte*>(data);

    for (int i = 0; i < sizeof(*data); ++i) {
        if (dataBytes[i]) {
            return false;
        }
    }

    return true;
}

char* get_file_contents(const char* const filename);

}
