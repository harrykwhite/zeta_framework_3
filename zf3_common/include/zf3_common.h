#ifndef ZF3_COMMON_H
#define ZF3_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
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

void zf3_init_identity_matrix_4x4(ZF3Matrix4x4* const mat);
void zf3_init_ortho_matrix_4x4(ZF3Matrix4x4* const mat, const float left, const float right, const float bottom, const float top, const float near, const float far);

//
// Memory
//
#define ZF3_KILOBYTES(X) ((X) << 10)
#define ZF3_MEGABYTES(X) ((X) << 20)
#define ZF3_GIGABYTES(X) ((X) << 30)

typedef unsigned char ZF3Byte;

typedef struct {
    ZF3Byte* buf;
    int size;
    int used;
} ZF3MemArena;

bool zf3_mem_arena_init(ZF3MemArena* const memArena, const int size);
void* zf3_mem_arena_push(ZF3MemArena* const memArena, const int size);
void zf3_mem_arena_reset(ZF3MemArena* const memArena);
void zf3_mem_arena_cleanup(ZF3MemArena* const memArena);

bool zf3_is_zero(const void* const mem, const int size);

//
// Miscellaneous
//
char* zf3_get_file_contents(const char* const filename);

#endif
