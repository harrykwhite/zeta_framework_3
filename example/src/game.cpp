#include "game.h"

Game i_game;

static void spawn_bullet(const zf3::Vec2D pos, const float spd, const float dir) {
    if (i_game.bulletCnt == gk_bulletLimit) {
        return;
    }

    const int bulletIndex = i_game.bulletCnt++;

    i_game.bullets[bulletIndex].pos = pos;
    i_game.bullets[bulletIndex].vel.x = spd * cosf(dir);
    i_game.bullets[bulletIndex].vel.y = spd * sinf(dir);
}

void init_game(const zf3::UserGameFuncData* const zf3Data) {
    zf3::load_render_layers(zf3Data->renderer, RENDER_LAYER_CNT, UI_RENDER_LAYER);

    i_game.player.pos.x = zf3Data->windowMeta->size.x / 2.0f;
    i_game.player.pos.y = zf3Data->windowMeta->size.y / 2.0f;

    zf3Data->cam->scale = 2.0f;
}

void run_game_tick(const zf3::UserGameFuncData* const zf3Data) {
    //
    // Player
    //
    {
        const zf3::Vec2DInt moveAxis = {
            zf3::is_key_down(zf3::KEY_D, zf3Data->windowMeta) - zf3::is_key_down(zf3::KEY_A, zf3Data->windowMeta),
            zf3::is_key_down(zf3::KEY_S, zf3Data->windowMeta) - zf3::is_key_down(zf3::KEY_W, zf3Data->windowMeta)
        };

        const float moveSpd = 3.0f;

        i_game.player.pos.x += moveAxis.x * moveSpd;
        i_game.player.pos.y += moveAxis.y * moveSpd;

        zf3Data->cam->pos = i_game.player.pos;

        if (zf3::is_mouse_button_pressed(zf3::MOUSE_BUTTON_LEFT, zf3Data->windowMeta)) {
            const zf3::Vec2D mousePos = zf3::conv_screen_to_camera_pos(zf3Data->windowMeta->inputState.mousePos, zf3Data->cam, zf3Data->windowMeta);
            const zf3::Vec2D playerToMouse = {
                mousePos.x - i_game.player.pos.x,
                mousePos.y - i_game.player.pos.y
            };

            spawn_bullet(i_game.player.pos, 12.0f, atan2f(playerToMouse.y, playerToMouse.x));
        }

        const zf3::SpriteBatchWriteData writeData = {
            .texIndex = 0,
            .pos = i_game.player.pos,
            .srcRect = {0, 0, 24, 24},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3::write_to_sprite_batch(zf3Data->renderer, WORLD_RENDER_LAYER, &writeData, zf3Data->assets);
    }

    //
    // Bullets
    //
    for (int i = 0; i < i_game.bulletCnt; ++i) {
        // Movement
        i_game.bullets[i].pos.x += i_game.bullets[i].vel.x;
        i_game.bullets[i].pos.y += i_game.bullets[i].vel.y;

        // Rendering
        const zf3::SpriteBatchWriteData writeData = {
            .texIndex = 0,
            .pos = i_game.bullets[i].pos,
            .srcRect = {24, 0, 6, 6},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3::write_to_sprite_batch(zf3Data->renderer, WORLD_RENDER_LAYER, &writeData, zf3Data->assets);
    }
}

void clean_game() {
}
