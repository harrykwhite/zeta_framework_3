#ifndef ZF3_COMMON_H
#define ZF3_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

//
// Assets
//
#define ZF3_ASSETS_FILE_NAME "assets.zf3"

#define ZF3_TEX_CHANNEL_COUNT 4
#define ZF3_TEX_WIDTH_LIMIT 2048
#define ZF3_TEX_HEIGHT_LIMIT 2048

//
// Logging
//
void zf3_log(const char* const format, ...);
void zf3_log_error(const char* const format, ...);

//
// Math
//
#define ZF3_MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define ZF3_MAX(X, Y) ((X) > (Y) ? (X) : (Y))

#define ZF3_PI 3.14159265358979323846f

typedef struct {
    float x;
    float y;
} ZF3Vec2D;

typedef struct {
    int x;
    int y;
} ZF3Vec2DInt;

typedef struct {
    float elems[4][4];
} ZF3Matrix4x4;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} ZF3Rect;

typedef struct {
    int begin;
    int end;
} ZF3Range;

void zf3_init_identity_matrix_4x4(ZF3Matrix4x4* const mat);
void zf3_init_ortho_matrix_4x4(ZF3Matrix4x4* const mat, const float left, const float right, const float bottom, const float top, const float near, const float far);

//
// Memory
//
#define ZF3_KILOBYTES(X) ((X) << 10)
#define ZF3_MEGABYTES(X) ((X) << 20)
#define ZF3_GIGABYTES(X) ((X) << 30)

#define ZF3_BITS_TO_BYTES(X) ((((X) + 7) & ~7) / 8)

typedef unsigned char ZF3Byte;

typedef struct {
    ZF3Byte* buf;
    int size;
    int used;
} ZF3MemArena;

bool zf3_init_mem_arena(ZF3MemArena* const memArena, const int size);
void zf3_clean_mem_arena(ZF3MemArena* const memArena);
void* zf3_push_to_mem_arena(ZF3MemArena* const memArena, const int size);
void zf3_reset_mem_arena(ZF3MemArena* const memArena);

bool zf3_is_zero(const void* const mem, const int size);

//
// Miscellaneous
//
#define ZF3_MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define ZF3_MAX(X, Y) ((X) > (Y) ? (X) : (Y))

#define ZF3_BITS_TO_BYTES(X) ((((X) + 7) & ~7) / 8)
#define ZF3_BYTES_TO_BITS(X) ((X) * 8)

char* zf3_get_file_contents(const char* const filename);

#endif
