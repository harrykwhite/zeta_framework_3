#include <zf3_game.h>

namespace zf3 {
    static constexpr int ik_targTicksPerSec = 60;
    static constexpr double ik_targTickDur = 1.0 / ik_targTicksPerSec;
    static constexpr double ik_tickDurLimitMult = 8.0;

    struct Game {
        Window window;
        InputManager inputManager;
        Assets assets;
        ShaderProgs shaderProgs;
        Renderer renderer;
        SoundSrcManager sndSrcManager;
        MusicSrcManager musicSrcManager;

        UserGameInfo userInfo;
        UserGameFuncData userFuncData;
    };

    static bool init_game(Game* const game) {
        log("Initialising...");

        if (!glfwInit()) {
            return false;
        }

        if (!init_window(&game->window, &game->inputManager, game->userInfo.initWindowWidth, game->userInfo.initWindowHeight, game->userInfo.windowTitle, game->userInfo.windowResizable, game->userInfo.hideCursor)) {
            return false;
        }

        glfwSwapInterval(1); // Enable VSync.

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            return false;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!init_audio_system()) {
            return false;
        }

        if (!init_assets(&game->assets)) {
            return false;
        }

        game->shaderProgs = load_shader_progs();

        init_rng();

        game->userFuncData = {
            .window = &game->window,
            .inputManager = &game->inputManager,
            .assets = &game->assets,
            .renderer = &game->renderer,
            .sndSrcManager = &game->sndSrcManager,
            .musicSrcManager = &game->musicSrcManager
        };

        if (!game->userInfo.init(&game->userFuncData)) {
            return false;
        }

        glfwShowWindow(game->window.glfwWindow);

        return true;
    }

    static double calc_valid_frame_dur(const double frameTime, const double frameTimeLast) {
        const double dur = frameTime - frameTimeLast;
        return dur >= 0.0 && dur <= ik_targTickDur * ik_tickDurLimitMult ? dur : 0.0;
    }

    static void run_game_loop(Game* const game) {
        double frameTime = glfwGetTime();
        double frameDurAccum = 0.0;

        log("Entering the game loop...");

        while (!glfwWindowShouldClose(game->window.glfwWindow)) {
            const double frameTimeLast = frameTime;
            frameTime = glfwGetTime();

            const double frameDur = calc_valid_frame_dur(frameTime, frameTimeLast);
            frameDurAccum += frameDur;

            const int tickCnt = frameDurAccum / ik_targTickDur;

            if (tickCnt > 0) {
                int i = 0;

                do {
                    handle_auto_release_sound_srcs(&game->sndSrcManager);
                    refresh_music_src_bufs(&game->musicSrcManager, game->assets.music); // TEMP

                    empty_sprite_batches(&game->renderer);

                    if (!game->userInfo.tick(&game->userFuncData)) {
                        return;
                    }

                    frameDurAccum -= ik_targTickDur;
                    ++i;
                } while (i < tickCnt);

                save_input_state(&game->inputManager);
            }

            render_all(&game->renderer, &game->shaderProgs, &game->window, &game->assets);
            glfwSwapBuffers(game->window.glfwWindow);

            glfwPollEvents();
        }
    }

    void run_game(const UserGameInfo& userInfo) {
        const auto game = static_cast<Game*>(calloc(1, sizeof(Game)));

        if (!game) {
            log_error("Failed to allocate game memory!");
            return;
        }

        game->userInfo = userInfo;

        if (init_game(game)) {
            run_game_loop(game);
        }

        log("Cleaning up...");

        if (game) {
            clean_music_srcs(&game->musicSrcManager);
            clean_sound_srcs(&game->sndSrcManager);
            clean_renderer(&game->renderer);
            clean_shader_progs(&game->shaderProgs);
            clean_assets(&game->assets);
            clean_window(&game->window);
            free(game);
        }

        clean_audio_system();
        glfwTerminate();
    }
}
