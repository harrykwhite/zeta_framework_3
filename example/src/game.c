#include "game.h"

Game i_game;

void game_init() {
    i_game.player.pos.x = zf3_get_window_size().x / 2.0f;
    i_game.player.pos.y = zf3_get_window_size().y / 2.0f;
}

void game_tick() {
    //
    // Player
    //
    {
        const ZF3Vec2DInt moveAxis = {
            zf3_is_key_down(ZF3_KEY_D) - zf3_is_key_down(ZF3_KEY_A),
            zf3_is_key_down(ZF3_KEY_S) - zf3_is_key_down(ZF3_KEY_W)
        };

        const float moveSpd = 3.0f;

        i_game.player.pos.x += moveAxis.x * moveSpd;
        i_game.player.pos.y += moveAxis.y * moveSpd;

        const ZF3SpriteBatchWriteData writeData = {
            .texIndex = 0,
            .pos = i_game.player.pos,
            .srcRect = {0, 0, 24, 24},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3_write_to_sprite_batch(&writeData);
    }
}

void game_cleanup() {
}
