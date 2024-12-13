#ifndef PLAYER_H
#define PLAYER_H

#include <zf3_public.h>

#define BULLET_LIMIT 256

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

void game_init();
void game_tick();
void game_cleanup();

#endif
