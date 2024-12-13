#pragma once

#include "zf3_public.h"

namespace zf3 {

constexpr int gk_glVersionMajor = 4;
constexpr int gk_glVersionMinor = 1;

//
// Window
//
GLFWwindow* create_glfw_window(WindowMeta* const windowMeta, const int width, const int height, const char* const title, const bool resizable);

//
// Assets
//
bool load_assets(Assets* const assets);
void unload_assets(Assets* const assets);

void load_shader_progs(ShaderProgs* const progs);
void unload_shader_progs(ShaderProgs* const progs);

//
// Rendering
//
void init_rendering_internals();
void clean_renderer(Renderer* const renderer);
void render_sprite_batches(const Renderer* const renderer, const Camera* const cam, const ShaderProgs* const shaderProgs, const WindowMeta* const windowMeta, const Assets* const assets);
void empty_sprite_batches(Renderer* const renderer);

}
