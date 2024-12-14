#include <zf3_game.h>

namespace zf3 {

static constexpr int ik_targTicksPerSec = 60;
static constexpr double ik_targTickDur = 1.0 / ik_targTicksPerSec;
static constexpr double ik_tickDurLimitMult = 8.0;

static void clean_game(const UserGameInfo& userInfo) {
    log("Cleaning up...");

    userInfo.cleanup();
    rendering_cleanup();
    unload_assets();
    clean_window();
    glfwTerminate();
}

static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
    const double dur = frameTime - frameTimeLast;
    return dur >= 0.0 && dur <= ik_targTickDur * ik_tickDurLimitMult ? dur : 0.0;
}

void run_game(const UserGameInfo& userInfo) {
    //
    // Initialisation
    //
    log("Initialising...");

    if (!glfwInit()) {
        clean_game(userInfo);
        return;
    }

    if (!init_window(userInfo.initWindowWidth, userInfo.initWindowHeight, userInfo.windowTitle, userInfo.windowResizable, userInfo.hideCursor)) {
        clean_game(userInfo);
        return;
    }

    glfwSwapInterval(1); // Enables VSync.

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        clean_game(userInfo);
        return;
    }

    if (!load_assets()) {
        clean_game(userInfo);
        return;
    }

    init_rendering_internals();

    init_rng();

    userInfo.init();

    show_window();

    //
    // Main Loop
    //
    double frameTime = glfwGetTime();
    double frameDurAccum = 0.0;

    log("Entering the main loop...");

    while (!should_window_close()) {
        const double frameTimeLast = frameTime;
        frameTime = glfwGetTime();

        const double frameDur = calc_valid_frame_dur(frameTime, frameTimeLast);
        frameDurAccum += frameDur;

        const int tickCnt = frameDurAccum / ik_targTickDur;

        if (tickCnt > 0) {
            int i = 0;

            do {
                empty_sprite_batches();
                userInfo.tick();

                frameDurAccum -= ik_targTickDur;
                ++i;
            } while (i < tickCnt);

            save_input_state();
        }

        render_all();
        swap_window_buffers();

        glfwPollEvents();
    }

    clean_game(userInfo);
}

}
