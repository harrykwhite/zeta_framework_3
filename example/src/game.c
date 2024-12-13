#include "game.h"

Game i_game;

static void spawn_bullet(const ZF3Vec2D pos, const float spd, const float dir) {
    if (i_game.bulletCnt >= BULLET_LIMIT) {
        return;
    }

    const int bulletIndex = i_game.bulletCnt++;

    i_game.bullets[bulletIndex].pos = pos;
    i_game.bullets[bulletIndex].vel.x = spd * cosf(dir);
    i_game.bullets[bulletIndex].vel.y = spd * sinf(dir);
}

void init_game(const ZF3UserGameFuncData* const zf3Data) {
    zf3_load_render_layers(zf3Data->renderer, RENDER_LAYER_CNT, UI_RENDER_LAYER);

    i_game.player.pos.x = zf3Data->windowMeta->size.x / 2.0f;
    i_game.player.pos.y = zf3Data->windowMeta->size.y / 2.0f;

    zf3Data->cam->scale = 2.0f;
}

void run_game_tick(const ZF3UserGameFuncData* const zf3Data) {
    //
    // Player
    //
    {
        const ZF3Vec2DInt moveAxis = {
            zf3_is_key_down(ZF3_KEY_D, zf3Data->windowMeta) - zf3_is_key_down(ZF3_KEY_A, zf3Data->windowMeta),
            zf3_is_key_down(ZF3_KEY_S, zf3Data->windowMeta) - zf3_is_key_down(ZF3_KEY_W, zf3Data->windowMeta)
        };

        const float moveSpd = 3.0f;

        i_game.player.pos.x += moveAxis.x * moveSpd;
        i_game.player.pos.y += moveAxis.y * moveSpd;

        zf3Data->cam->pos = i_game.player.pos;

        if (zf3_is_mouse_button_pressed(ZF3_MOUSE_BUTTON_LEFT, zf3Data->windowMeta)) {
            const ZF3Vec2D mousePos = zf3_conv_screen_to_camera_pos(zf3Data->windowMeta->inputState.mousePos, zf3Data->cam, zf3Data->windowMeta);
            const ZF3Vec2D playerToMouse = {
                mousePos.x - i_game.player.pos.x,
                mousePos.y - i_game.player.pos.y
            };

            spawn_bullet(i_game.player.pos, 12.0f, atan2f(playerToMouse.y, playerToMouse.x));
        }

        const ZF3SpriteBatchWriteData writeData = {
            .texIndex = 0,
            .pos = i_game.player.pos,
            .srcRect = {0, 0, 24, 24},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3_write_to_sprite_batch(zf3Data->renderer, WORLD_RENDER_LAYER, &writeData, zf3Data->assets);
    }

    //
    // Bullets
    //
    for (int i = 0; i < i_game.bulletCnt; ++i) {
        // Movement
        i_game.bullets[i].pos.x += i_game.bullets[i].vel.x;
        i_game.bullets[i].pos.y += i_game.bullets[i].vel.y;

        // Rendering
        const ZF3SpriteBatchWriteData writeData = {
            .texIndex = 0,
            .pos = i_game.bullets[i].pos,
            .srcRect = {24, 0, 6, 6},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3_write_to_sprite_batch(zf3Data->renderer, WORLD_RENDER_LAYER, &writeData, zf3Data->assets);
    }
}

void clean_game() {
}
