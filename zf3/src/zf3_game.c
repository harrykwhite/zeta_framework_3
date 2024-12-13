#include <zf3.h>

#define TARG_TICKS_PER_SEC 60
#define TARG_TICK_DUR (1.0 / TARG_TICKS_PER_SEC)
#define TICK_DUR_LIMIT_MULT 8

typedef unsigned short GameCleanupBitset;

enum GameCleanupBit {
    WINDOW_CLEANUP_BIT = 1 << 0,
    ASSETS_CLEANUP_BIT = 1 << 1,
    SHADER_PROGS_CLEANUP_BIT = 1 << 2,
    SCENE_SYSTEM_CLEANUP_BIT = 1 << 3
};

static bool game_init(GameCleanupBitset* const cleanupBitset, const ZF3GameInfo* const gameInfo) {
    // Initialise GLFW.
    if (!glfwInit()) {
        return false;
    }

    // Create the GLFW window.
    if (!zf3_window_init(gameInfo->windowInitWidth, gameInfo->windowInitHeight, gameInfo->windowTitle, gameInfo->windowResizable)) {
        return false;
    }

    *cleanupBitset |= WINDOW_CLEANUP_BIT;

    // Enable VSync.
    glfwSwapInterval(1);

    // Initialise OpenGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }

    // Load assets.
    if (!zf3_load_assets()) {
        return false;
    }

    *cleanupBitset |= ASSETS_CLEANUP_BIT;

    // Load shader programs.
    if (!zf3_load_shader_progs()) {
        return false;
    }

    *cleanupBitset |= SHADER_PROGS_CLEANUP_BIT;

    //
    zf3_init_rendering_internals();

    // Load the initial scene.
    if (!zf3_scene_system_init()) {
        return false;
    }

    *cleanupBitset |= SCENE_SYSTEM_CLEANUP_BIT;

    // Show the window now that everything is set up.
    zf3_show_window();

    return true;
}

static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
    const double dur = frameTime - frameTimeLast;
    return dur >= 0.0 && dur <= TARG_TICK_DUR * TICK_DUR_LIMIT_MULT ? dur : 0.0;
}

static void game_loop() {
    ZF3Renderer renderer = {0}; // TEMP

    double frameTime = glfwGetTime();
    double frameDurAccum = 0.0;

    while (!zf3_should_window_close()) {
        const double frameTimeLast = frameTime;
        frameTime = glfwGetTime();

        const double frameDur = calc_valid_frame_dur(frameTime, frameTimeLast);
        frameDurAccum += frameDur;

        const int tickCnt = frameDurAccum / TARG_TICK_DUR;

        if (tickCnt > 0) {
            // Execute ticks.
            int i = 0;

            do {
                zf3_empty_sprite_batches(&renderer);
                zf3_proc_scene_tick();

                frameDurAccum -= TARG_TICK_DUR;
                ++i;
            } while (i < tickCnt);

            // Save the input state.
            zf3_save_input_state();
        }

        zf3_render_sprite_batches(&renderer);
        zf3_swap_buffers();

        glfwPollEvents();
    }

    zf3_clean_renderer(&renderer);
}

static void game_cleanup(const GameCleanupBitset cleanupBitset) {
    if (cleanupBitset & SCENE_SYSTEM_CLEANUP_BIT) {
        zf3_scene_system_cleanup();
    }

    if (cleanupBitset & SHADER_PROGS_CLEANUP_BIT) {
        zf3_unload_shader_progs();
    }

    if (cleanupBitset & ASSETS_CLEANUP_BIT) {
        zf3_unload_assets();
    }

    if (cleanupBitset & WINDOW_CLEANUP_BIT) {
        zf3_window_cleanup();
    }

    glfwTerminate();
}

void zf3_run_game(const ZF3GameInfo* const gameInfo) {
    GameCleanupBitset cleanupBitset = 0;

    if (game_init(&cleanupBitset, gameInfo)) {
        game_loop();
    }

    game_cleanup(cleanupBitset);
}
