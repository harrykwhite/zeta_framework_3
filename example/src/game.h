#pragma once

#include <zf3_public.h>

constexpr int gk_bulletLimit = 256;

enum RenderLayer {
    WORLD_RENDER_LAYER,
    UI_RENDER_LAYER,

    RENDER_LAYER_CNT
};

struct Player {
    zf3::Vec2D pos;
};

struct Bullet {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
};

struct Game {
    Player player;

    Bullet bullets[gk_bulletLimit];
    int bulletCnt;
};

void init_game(const zf3::UserGameFuncData* const zf3Data);
void run_game_tick(const zf3::UserGameFuncData* const zf3Data);
void clean_game();
