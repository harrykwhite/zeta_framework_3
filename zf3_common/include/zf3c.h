#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

namespace zf3 {

//
// Assets
//
constexpr const char* gk_assetsFileName = "assets.zf3";

constexpr int gk_texChannelCnt = 4;
constexpr int TEX_WIDTH_LIMIT = 2048;
constexpr int TEX_HEIGHT_LIMIT = 2048;

//
// Logging
//
void log(const char* const format, ...);
void log_error(const char* const format, ...);

//
// Math
//
constexpr float gk_pi = 3.14159265358979323846f;

template<typename T>
constexpr T min(T x, T y) {
    return (x < y) ? x : y;
}

template<typename T>
constexpr T max(T x, T y) {
    return (x > y) ? x : y;
}

struct Vec2D {
    float x;
    float y;
};

struct Vec2DInt {
    int x;
    int y;
};

struct Matrix4x4 {
    float elems[4][4];
};

struct Rect {
    int x;
    int y;
    int width;
    int height;
};

struct Range {
    int begin;
    int end;
};

void init_identity_matrix_4x4(Matrix4x4* const mat);
void init_ortho_matrix_4x4(Matrix4x4* const mat, const float left, const float right, const float bottom, const float top, const float near, const float far);

//
// Memory
//
using Byte = unsigned char;

struct MemArena {
    Byte* buf;
    int size;
    int used;
};

bool init_mem_arena(MemArena* const memArena, const int size);
void clean_mem_arena(MemArena* const memArena);
void reset_mem_arena(MemArena* const memArena);

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

//
// Miscellaneous
//
char* get_file_contents(const char* const filename);

}
