#ifndef PLAYER_H
#define PLAYER_H

#include <zf3_public.h>

#define BULLET_LIMIT 256

typedef enum {
    WORLD_RENDER_LAYER,
    UI_RENDER_LAYER,

    RENDER_LAYER_CNT
} RenderLayer;

typedef struct {
    ZF3Vec2D pos;
} Player;

typedef struct {
    ZF3Vec2D pos;
    ZF3Vec2D vel;
} Bullet;

typedef struct {
    Player player;

    Bullet bullets[BULLET_LIMIT];
    int bulletCnt;
} Game;

void init_game(const ZF3UserGameFuncData* const zf3Data);
void run_game_tick(const ZF3UserGameFuncData* const zf3Data);
void clean_game();

#endif
