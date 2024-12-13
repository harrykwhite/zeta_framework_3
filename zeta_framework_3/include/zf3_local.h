#ifndef ZF3_LOCAL_H
#define ZF3_LOCAL_H

#include "zf3_public.h"

#define ZF3_GL_VERSION_MAJOR 4
#define ZF3_GL_VERSION_MINOR 1

//
// Window
//
bool zf3_window_init(const int width, const int height, const char* const title, const bool resizable);
void zf3_window_cleanup();
void zf3_show_window();
bool zf3_should_window_close();
void zf3_swap_buffers();

void zf3_save_input_state();

//
// Assets
//
#define ZF3_SPRITE_QUAD_SHADER_PROG_VERT_CNT 11

#define ZF3_TEX_LIMIT 256

bool zf3_load_assets();
void zf3_unload_assets();

bool zf3_load_shader_progs();
void zf3_unload_shader_progs();

//
// Rendering
//
void zf3_init_rendering_internals();
void zf3_clean_renderer();
void zf3_render_sprite_batches();
void zf3_empty_sprite_batches();

#endif
