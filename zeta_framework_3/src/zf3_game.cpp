#include <zf3_game.h>

namespace zf3 {
    static constexpr int ik_targTicksPerSec = 60;
    static constexpr double ik_targTickDur = 1.0 / ik_targTicksPerSec;
    static constexpr double ik_tickDurLimitMult = 8.0;

    struct Game {
        Window window;
        InputManager inputManager;
        ShaderProgs shaderProgs;
        Renderer renderer;
        SoundSrcManager sndSrcManager;
        MusicSrcManager musicSrcManager;
    };

    static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
        const double dur = frameTime - frameTimeLast;
        return dur >= 0.0 && dur <= ik_targTickDur * ik_tickDurLimitMult ? dur : 0.0;
    }

    static void run_game(Game& game, const UserGameInfo& userInfo) {
        //
        // Initialisation
        //
        log("Initialising...");

        if (!glfwInit()) {
            log_error("Failed to initialise GLFW!");
            return;
        }

        if (!init_window(game.window, game.inputManager, userInfo.initWindowWidth, userInfo.initWindowHeight, userInfo.windowTitle, userInfo.windowResizable, userInfo.hideCursor)) {
            log_error("Failed to initialise the window!");
            return;
        }

        glfwSwapInterval(1); // Enable VSync.

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            log_error("Failed to initialise OpenGL function pointers!");
            return;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!init_audio_system()) {
            return;
        }

        if (!init_assets()) {
            return;
        }

        game.shaderProgs = load_shader_progs();

        init_rng();

        const UserGameFuncData userFuncData = {
            .window = game.window,
            .inputManager = game.inputManager,
            .renderer = game.renderer,
            .sndSrcManager = game.sndSrcManager,
            .musicSrcManager = game.musicSrcManager
        };

        if (!userInfo.init(userFuncData)) {
            return;
        }

        glfwShowWindow(game.window.glfwWindow);

        //
        // Main Loop
        //
        double frameTime = glfwGetTime();
        double frameDurAccum = 0.0;

        log("Entering the main loop...");

        while (!glfwWindowShouldClose(game.window.glfwWindow)) {
            const double frameTimeLast = frameTime;
            frameTime = glfwGetTime();

            const double frameDur = calc_valid_frame_dur(frameTime, frameTimeLast);
            frameDurAccum += frameDur;

            const int tickCnt = frameDurAccum / ik_targTickDur;

            if (tickCnt > 0) {
                int i = 0;

                do {
                    handle_auto_release_sound_srcs(game.sndSrcManager);
                    refresh_music_src_bufs(game.musicSrcManager); // TEMP: Put on another thread.

                    empty_sprite_batches(game.renderer);

                    if (!userInfo.tick(userFuncData)) {
                        return;
                    }

                    frameDurAccum -= ik_targTickDur;
                    ++i;
                } while (i < tickCnt);

                save_input_state(game.inputManager);
            }

            render_all(game.renderer, game.shaderProgs, game.window);
            glfwSwapBuffers(game.window.glfwWindow);

            glfwPollEvents();
        }
    }

    void start_game(const UserGameInfo& userInfo) {
        const auto game = alloc_zeroed<Game>();

        if (!game) {
            log_error("Failed to allocate game memory!");
            return;
        }

        run_game(*game, userInfo);

        log("Cleaning up...");

        userInfo.clean();

        if (game) {
            clean_music_srcs(game->musicSrcManager);
            clean_sound_srcs(game->sndSrcManager);
            clean_renderer(game->renderer);
            clean_shader_progs(game->shaderProgs);
            clean_assets();
            clean_window(game->window);
            free(game);
        }

        clean_audio_system();
        glfwTerminate();
    }
}
