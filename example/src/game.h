#ifndef PLAYER_H
#define PLAYER_H

#include <zf3_public.h>

typedef struct {
    ZF3Vec2D pos;
} Player;

typedef struct {
    Player player;
} Game;

void game_init();
void game_tick();
void game_cleanup();

#endif
