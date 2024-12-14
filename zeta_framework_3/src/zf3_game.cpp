#include <zf3_game.h>

namespace zf3 {

static constexpr int ik_targTicksPerSec = 60;
static constexpr double ik_targTickDur = 1.0 / ik_targTicksPerSec;
static constexpr double ik_tickDurLimitMult = 8.0;

typedef struct {
    MemArena* memArena;
    Assets* assets;
    ShaderProgs* shaderProgs;
    Renderer* renderer;
} GameCleanupPtrs;

static void clean_game(const GameCleanupPtrs* const ptrs, const UserGameInfo* const userInfo) {
    log("Cleaning up...");

    userInfo->cleanup();

    if (ptrs->renderer) {
        clean_renderer(ptrs->renderer);
    }

    if (ptrs->shaderProgs) {
        unload_shader_progs(ptrs->shaderProgs);
    }

    if (ptrs->assets) {
        unload_assets(ptrs->assets);
    }

    clean_window();

    glfwTerminate();

    if (ptrs->memArena) {
        clean_mem_arena(ptrs->memArena);
    }
}

static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
    const double dur = frameTime - frameTimeLast;
    return dur >= 0.0 && dur <= ik_targTickDur * ik_tickDurLimitMult ? dur : 0.0;
}

void run_game(const UserGameInfo* const userInfo) {
    GameCleanupPtrs cleanupPtrs = {0};

    //
    // Initialisation
    //
    log("Initialising...");

    MemArena memArena;

    if (!init_mem_arena(&memArena, megabytes_to_bytes(4))) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    cleanupPtrs.memArena = &memArena;

    if (!glfwInit()) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    if (!init_window(userInfo->initWindowWidth, userInfo->initWindowHeight, userInfo->windowTitle, userInfo->windowResizable, userInfo->hideCursor)) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    // Enable VSync.
    glfwSwapInterval(1);

    // Initialise OpenGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    const auto assets = push_to_mem_arena<Assets>(&memArena);
    assert(assets);

    if (!load_assets(assets)) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    ShaderProgs shaderProgs;
    load_shader_progs(&shaderProgs);

    init_rendering_internals();

    const auto renderer = push_to_mem_arena<Renderer>(&memArena);
    assert(renderer);

    Camera cam = {
        .scale = 1.0f
    };

    init_rng();

    const UserGameFuncData userFuncData = {
        .assets = assets,
        .renderer = renderer,
        .cam = &cam
    };

    userInfo->init(&userFuncData);

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
                empty_sprite_batches(renderer);
                userInfo->tick(&userFuncData);

                frameDurAccum -= ik_targTickDur;
                ++i;
            } while (i < tickCnt);

            save_input_state();
        }

        render_all(renderer, &cam, &shaderProgs, assets);
        swap_window_buffers();

        glfwPollEvents();
    }

    clean_game(&cleanupPtrs, userInfo);
}

}
