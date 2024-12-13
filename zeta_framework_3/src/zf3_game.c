#include <zf3_local.h>

#define TARG_TICKS_PER_SEC 60
#define TARG_TICK_DUR (1.0 / TARG_TICKS_PER_SEC)
#define TICK_DUR_LIMIT_MULT 8

typedef unsigned short GameCleanupBitset;

enum GameCleanupBit {
    WINDOW_CLEANUP_BIT = 1 << 0,
    ASSETS_CLEANUP_BIT = 1 << 1,
    SHADER_PROGS_CLEANUP_BIT = 1 << 2
};

static bool game_init(GameCleanupBitset* const cleanupBitset, const ZF3GameUserInfo* const userInfo) {
    // Initialise GLFW.
    if (!glfwInit()) {
        return false;
    }

    // Create the GLFW window.
    if (!zf3_window_init(userInfo->windowInitWidth, userInfo->windowInitHeight, userInfo->windowTitle, userInfo->windowResizable)) {
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

    // Set up rendering.
    zf3_init_rendering_internals();

    // Call the user-defined initialisation function.
    userInfo->init();

    // Show the window now that everything is set up.
    zf3_show_window();

    return true;
}

static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
    const double dur = frameTime - frameTimeLast;
    return dur >= 0.0 && dur <= TARG_TICK_DUR * TICK_DUR_LIMIT_MULT ? dur : 0.0;
}

static void game_loop(const ZF3GameUserInfo* const userInfo) {
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
                zf3_empty_sprite_batches();
                userInfo->tick();

                frameDurAccum -= TARG_TICK_DUR;
                ++i;
            } while (i < tickCnt);

            // Save the input state.
            zf3_save_input_state();
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        zf3_render_sprite_batches();
        zf3_swap_buffers();

        glfwPollEvents();
    }
}

void zf3_run_game(const ZF3GameUserInfo* const userInfo) {
    GameCleanupBitset cleanupBitset = 0;

    if (game_init(&cleanupBitset, userInfo)) {
        game_loop(userInfo);
    }

    //
    // Cleanup
    //
    userInfo->cleanup();

    zf3_clean_renderer();

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
