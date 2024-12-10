#include <zf3.h>

#define PERM_MEM_ARENA_SIZE ZF3_MEGABYTES(128)
#define TEMP_MEM_ARENA_SIZE ZF3_MEGABYTES(32)

#define TARG_TICKS_PER_SEC 60
#define TARG_TICK_DUR (1.0 / TARG_TICKS_PER_SEC)
#define TICK_DUR_LIMIT_MULT 8

typedef unsigned short GameCleanupBitset;

enum GameCleanupBit {
    PERM_MEM_ARENA_CLEANUP_BIT = 1 << 0,
    TEMP_MEM_ARENA_CLEANUP_BIT = 1 << 1,
    GLFW_CLEANUP_BIT = 1 << 2,
    GLFW_WINDOW_CLEANUP_BIT = 1 << 3,
    ASSETS_CLEANUP_BIT = 1 << 4,
    SHADER_PROGS_CLEANUP_BIT = 1 << 5
};

typedef struct {
    GameCleanupBitset cleanupBitset;

    ZF3MemArena permMemArena;
    ZF3MemArena tempMemArena;

    GLFWwindow* glfwWindow;
    ZF3ShaderProgs shaderProgs;
} Game;

ZF3Assets* i_assets; // TEMP?

static bool game_init(Game* const game, const ZF3GameInfo* const gameInfo) {
    assert(zf3_is_zero(game, sizeof(*game)));

    // Initialise the memory arenas.
    if (!zf3_mem_arena_init(&game->permMemArena, PERM_MEM_ARENA_SIZE)) {
        return false;
    }

    game->cleanupBitset |= PERM_MEM_ARENA_CLEANUP_BIT;

    if (!zf3_mem_arena_init(&game->tempMemArena, TEMP_MEM_ARENA_SIZE)) {
        return false;
    }

    game->cleanupBitset |= TEMP_MEM_ARENA_CLEANUP_BIT;

    // Initialise GLFW.
    if (!glfwInit()) {
        return false;
    }

    game->cleanupBitset |= GLFW_CLEANUP_BIT;

    // Create the GLFW window.
    game->glfwWindow = zf3_create_glfw_window(gameInfo->windowInitWidth, gameInfo->windowInitHeight, gameInfo->windowTitle, gameInfo->windowResizable);

    if (!game->glfwWindow) {
        return false;
    }

    game->cleanupBitset |= GLFW_WINDOW_CLEANUP_BIT;

    // Enable VSync.
    glfwSwapInterval(1);

    // Initialise OpenGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }

    // Load assets.
    i_assets = zf3_mem_arena_push(&game->permMemArena, sizeof(*i_assets));
    assert(i_assets);

    if (!zf3_load_assets(i_assets, &game->tempMemArena)) {
        return false;
    }

    game->cleanupBitset |= ASSETS_CLEANUP_BIT;

    // Load shader programs.
    zf3_load_shader_progs(&game->shaderProgs);
    game->cleanupBitset |= SHADER_PROGS_CLEANUP_BIT;

    // Show the window now that everything is set up.
    glfwShowWindow(game->glfwWindow);

    return true;
}

static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
    const double dur = frameTime - frameTimeLast;
    return dur >= 0.0 && dur <= TARG_TICK_DUR * TICK_DUR_LIMIT_MULT ? dur : 0.0;
}

static void game_loop(Game* const game) {
    ZF3SpriteRenderer* const spriteRenderer = zf3_mem_arena_push(&game->permMemArena, sizeof(*spriteRenderer)); // TEMP

    ZF3Sprite* const sprite = zf3_gen_sprites(spriteRenderer, 1);
    sprite->alpha = 1.0f;
    sprite->size = (ZF3Vec2D) {32.0f, 32.0f};

    double frameTime = glfwGetTime();
    double frameDurAccum = 0.0;

    ZF3InputState inputState = {0};

    while (!glfwWindowShouldClose(game->glfwWindow)) {
        zf3_mem_arena_reset(&game->tempMemArena);

        const double frameTimeLast = frameTime;
        frameTime = glfwGetTime();

        const double frameDur = calc_valid_frame_dur(frameTime, frameTimeLast);
        frameDurAccum += frameDur;

        const int tickCnt = frameDurAccum / TARG_TICK_DUR;

        if (tickCnt > 0) {
            // Refresh input.
            const ZF3InputState inputStateLast = inputState;
            zf3_load_input_state(&inputState, game->glfwWindow);

            // Execute ticks.
            int i = 0;

            do {
                frameDurAccum -= TARG_TICK_DUR;
                ++i;
            } while (i < tickCnt);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        zf3_render_sprites(spriteRenderer, &game->shaderProgs);

        glfwSwapBuffers(game->glfwWindow);

        glfwPollEvents();
    }

    zf3_sprite_renderer_cleanup(spriteRenderer);
}

static void game_cleanup(Game* const game) {
    if (game->cleanupBitset & SHADER_PROGS_CLEANUP_BIT) {
        zf3_unload_shader_progs(&game->shaderProgs);
    }

    if (game->cleanupBitset & ASSETS_CLEANUP_BIT) {
        zf3_unload_assets(i_assets);
    }

    if (game->cleanupBitset & GLFW_WINDOW_CLEANUP_BIT) {
        glfwDestroyWindow(game->glfwWindow);
    }

    if (game->cleanupBitset & GLFW_CLEANUP_BIT) {
        glfwTerminate();
    }

    if (game->cleanupBitset & TEMP_MEM_ARENA_CLEANUP_BIT) {
        zf3_mem_arena_cleanup(&game->tempMemArena);
    }

    if (game->cleanupBitset & PERM_MEM_ARENA_CLEANUP_BIT) {
        zf3_mem_arena_cleanup(&game->permMemArena);
    }
}

void zf3_run_game(const ZF3GameInfo* const gameInfo) {
    Game game = {0};

    if (game_init(&game, gameInfo)) {
        game_loop(&game);
    }

    game_cleanup(&game);
}

const ZF3Assets* zf3_get_assets() {
    return i_assets;
}
