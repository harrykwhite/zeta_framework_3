#ifndef ZF3_LOCAL_H
#define ZF3_LOCAL_H

#include "zf3_public.h"

#define ZF3_GL_VERSION_MAJOR 4
#define ZF3_GL_VERSION_MINOR 1

//
// Window
//
GLFWwindow* zf3_create_glfw_window(ZF3WindowMeta* const windowMeta, const int width, const int height, const char* const title, const bool resizable);

//
// Assets
//
#define ZF3_SPRITE_QUAD_SHADER_PROG_VERT_CNT 11

#define ZF3_TEX_LIMIT 256

bool zf3_load_assets(ZF3Assets* const assets);
void zf3_unload_assets(ZF3Assets* const assets);

void zf3_load_shader_progs(ZF3ShaderProgs* const progs);
void zf3_unload_shader_progs(ZF3ShaderProgs* const progs);

//
// Rendering
//
void zf3_init_rendering_internals();
void zf3_clean_renderer(ZF3Renderer* const renderer);
void zf3_render_sprite_batches(const ZF3Renderer* const renderer, const ZF3Camera* const cam, const ZF3ShaderProgs* const shaderProgs, const ZF3WindowMeta* const windowMeta, const ZF3Assets* const assets);
void zf3_empty_sprite_batches(ZF3Renderer* const renderer);

#endif
