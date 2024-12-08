#ifndef ZF3_H
#define ZF3_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define ZF3_KILOBYTES(X) ((X) << 10)
#define ZF3_MEGABYTES(X) ((X) << 20)
#define ZF3_GIGABYTES(X) ((X) << 30)

#define ZF3_GL_VERSION_MAJOR 4
#define ZF3_GL_VERSION_MINOR 1

typedef unsigned char ZF3Byte;

//
// zf3_game.c
//
typedef struct {
    int windowInitWidth;
    int windowInitHeight;
    const char* windowTitle;
    bool windowResizable;
} ZF3GameInfo;

void zf3_run_game(const ZF3GameInfo* const gameInfo);

//
// zf3_window.c
//
GLFWwindow* zf3_create_glfw_window(const int width, const int height, const char* const title, const bool resizable);

//
// zf3_utils.c
//
typedef struct {
    ZF3Byte* buf;
    int size;
    int used;
} ZF3MemArena;

bool zf3_mem_arena_init(ZF3MemArena* const memArena, const int size);
void* zf3_mem_arena_push(ZF3MemArena* const memArena, const int size);
void zf3_mem_arena_reset(ZF3MemArena* const memArena);
void zf3_mem_arena_cleanup(ZF3MemArena* const memArena);

#endif
