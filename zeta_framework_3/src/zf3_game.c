#include <zf3_local.h>

#define TARG_TICKS_PER_SEC 60
#define TARG_TICK_DUR (1.0 / TARG_TICKS_PER_SEC)
#define TICK_DUR_LIMIT_MULT 8

typedef struct {
    ZF3MemArena* memArena;
    GLFWwindow* glfwWindow;
    ZF3Assets* assets;
    ZF3ShaderProgs* shaderProgs;
    ZF3Renderer* renderer;
} GameCleanupPtrs;

static void clean_game(const GameCleanupPtrs* const ptrs, const ZF3UserGameInfo* const userInfo) {
    userInfo->cleanup();

    if (ptrs->renderer) {
        zf3_clean_renderer(ptrs->renderer);
    }

    if (ptrs->shaderProgs) {
        zf3_unload_shader_progs(ptrs->shaderProgs);
    }

    if (ptrs->assets) {
        zf3_unload_assets(ptrs->assets);
    }

    if (ptrs->glfwWindow) {
        glfwDestroyWindow(ptrs->glfwWindow);
    }

    glfwTerminate();

    if (ptrs->memArena) {
        zf3_clean_mem_arena(ptrs->memArena);
    }
}

static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
    const double dur = frameTime - frameTimeLast;
    return dur >= 0.0 && dur <= TARG_TICK_DUR * TICK_DUR_LIMIT_MULT ? dur : 0.0;
}

void zf3_run_game(const ZF3UserGameInfo* const userInfo) {
    GameCleanupPtrs cleanupPtrs = {0};

    //
    // Initialisation
    //
    ZF3MemArena memArena;

    if (!zf3_init_mem_arena(&memArena, ZF3_MEGABYTES(4))) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    cleanupPtrs.memArena = &memArena;

    if (!glfwInit()) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    ZF3WindowMeta windowMeta = {0};
    GLFWwindow* const glfwWindow = zf3_create_glfw_window(&windowMeta, userInfo->initWindowWidth, userInfo->initWindowHeight, userInfo->windowTitle, userInfo->windowResizable);

    if (!glfwWindow) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    cleanupPtrs.glfwWindow = glfwWindow;

    // Enable VSync.
    glfwSwapInterval(1);

    // Initialise OpenGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    ZF3Assets* const assets = zf3_push_to_mem_arena(&memArena, sizeof(*assets));
    assert(assets);

    if (!zf3_load_assets(assets)) {
        clean_game(&cleanupPtrs, userInfo);
        return;
    }

    ZF3ShaderProgs shaderProgs;
    zf3_load_shader_progs(&shaderProgs);

    zf3_init_rendering_internals();

    ZF3Renderer* const renderer = zf3_push_to_mem_arena(&memArena, sizeof(*renderer));
    assert(renderer);

    ZF3Camera cam = {
        .scale = 1.0f
    };

    const ZF3UserGameFuncData userFuncData = {
        .windowMeta = &windowMeta,
        .assets = assets,
        .renderer = renderer,
        .cam = &cam
    };

    userInfo->init(&userFuncData);

    glfwShowWindow(glfwWindow);

    //
    // Main Loop
    //
    double frameTime = glfwGetTime();
    double frameDurAccum = 0.0;

    while (!glfwWindowShouldClose(glfwWindow)) {
        const double frameTimeLast = frameTime;
        frameTime = glfwGetTime();

        const double frameDur = calc_valid_frame_dur(frameTime, frameTimeLast);
        frameDurAccum += frameDur;

        const int tickCnt = frameDurAccum / TARG_TICK_DUR;

        if (tickCnt > 0) {
            int i = 0;

            do {
                zf3_empty_sprite_batches(renderer);
                userInfo->tick(&userFuncData);

                frameDurAccum -= TARG_TICK_DUR;
                ++i;
            } while (i < tickCnt);

            windowMeta.inputStateSaved = windowMeta.inputState;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        zf3_render_sprite_batches(renderer, &cam, &shaderProgs, &windowMeta, assets);
        glfwSwapBuffers(glfwWindow);

        glfwPollEvents();
    }

    clean_game(&cleanupPtrs, userInfo);
}
