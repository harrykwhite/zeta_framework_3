#include <zf3.h>

#define MEM_ARENA_SIZE ZF3_MEGABYTES(64)

#define TARG_TICKS_PER_SEC 60
#define TARG_TICK_DUR (1.0 / TARG_TICKS_PER_SEC)
#define TICK_DUR_LIMIT_MULT 8

typedef unsigned short GameCleanupBitset;

enum GameCleanupBit {
    MEM_ARENA_CLEANUP_BIT = 1 << 0,
    GLFW_CLEANUP_BIT = 1 << 1,
    GLFW_WINDOW_CLEANUP_BIT = 1 << 2
};

typedef struct {
    GameCleanupBitset cleanupBitset;
    ZF3MemArena memArena;
    GLFWwindow* glfwWindow;
} Game;

static bool game_init(Game* const game, const ZF3GameInfo* const gameInfo) {
    // Initialise the memory arena.
    if (!zf3_mem_arena_init(&game->memArena, MEM_ARENA_SIZE)) {
        return false;
    }

    game->cleanupBitset |= MEM_ARENA_CLEANUP_BIT;

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

    // Show the window now that everything is set up.
    glfwShowWindow(game->glfwWindow);

    return true;
}

static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
    const double dur = frameTime - frameTimeLast;
    return dur >= 0.0 && dur <= TARG_TICK_DUR * TICK_DUR_LIMIT_MULT ? dur : 0.0;
}

static void game_loop(Game* const game) {
    double frameTime = glfwGetTime();
    double frameDurAccum = 0.0;

    ZF3InputState inputState = {0};

    while (!glfwWindowShouldClose(game->glfwWindow)) {
        glfwPollEvents();

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

        // Render.
        glfwSwapBuffers(game->glfwWindow);
    }
}

static void game_cleanup(Game* const game) {
    if (game->cleanupBitset & GLFW_WINDOW_CLEANUP_BIT) {
        glfwDestroyWindow(game->glfwWindow);
    }

    if (game->cleanupBitset & GLFW_CLEANUP_BIT) {
        glfwTerminate();
    }

    if (game->cleanupBitset & MEM_ARENA_CLEANUP_BIT) {
        zf3_mem_arena_cleanup(&game->memArena);
    }
}

void zf3_run_game(const ZF3GameInfo* const gameInfo) {
    Game game = {0};

    if (game_init(&game, gameInfo)) {
        game_loop(&game);
    }

    game_cleanup(&game);
}
