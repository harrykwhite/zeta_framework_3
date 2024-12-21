#include <zf3_assets.h>

namespace zf3 {
    static Assets* i_assets;

    bool load_assets() {
        assert(!i_assets);

        // Allocate memory for assets.
        i_assets = alloc_zeroed<Assets>();

        if (!i_assets) {
            log_error("Failed to allocate memory for assets!");
            return false;
        }

        // Open the assets file.
        FILE* const fs = fopen(gk_assetsFileName, "rb");

        if (!fs) {
            log_error("Failed to open \"%s\"!", gk_assetsFileName);

            free(i_assets);
            i_assets = nullptr;

            return false;
        }

        // Allocate a memory arena for working space.
        constexpr int scratchSpaceSize = zf3::megabytes_to_bytes(32);
        MemArena scratchSpace = {};

        if (!init_mem_arena(scratchSpace, scratchSpaceSize)) {
            log_error("Failed to initialise scratch space for assets initialisation!");

            fclose(fs);

            free(i_assets);
            i_assets = nullptr;

            return false;
        }

        // Reserve space for pixel data, used for both textures and fonts.
        const int pxDataSize = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;
        auto pxData = push_to_mem_arena<unsigned char>(scratchSpace, pxDataSize);
        assert(pxData);

        // Load textures.
        fread(&i_assets->textures.cnt, sizeof(i_assets->textures.cnt), 1, fs);

        if (i_assets->textures.cnt > 0) {
            glGenTextures(i_assets->textures.cnt, i_assets->textures.glIDs);

            for (int i = 0; i < i_assets->textures.cnt; ++i) {
                fread(&i_assets->textures.sizes[i], sizeof(i_assets->textures.sizes[i]), 1, fs);
                fread(pxData, gk_texChannelCnt * i_assets->textures.sizes[i].x * i_assets->textures.sizes[i].y, 1, fs);

                glBindTexture(GL_TEXTURE_2D, i_assets->textures.glIDs[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_assets->textures.sizes[i].x, i_assets->textures.sizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxData);
            }
        }

        // Load fonts.
        fread(&i_assets->fonts.cnt, sizeof(i_assets->fonts.cnt), 1, fs);

        if (i_assets->fonts.cnt > 0) {
            glGenTextures(i_assets->fonts.cnt, i_assets->fonts.texGLIDs);

            for (int i = 0; i < i_assets->fonts.cnt; ++i) {
                fread(&i_assets->fonts.arrangementInfos[i], sizeof(i_assets->fonts.arrangementInfos[i]), 1, fs);
                fread(&i_assets->fonts.texSizes[i], sizeof(i_assets->fonts.texSizes[i]), 1, fs);
                fread(pxData, gk_texPxDataSizeLimit, 1, fs);

                glBindTexture(GL_TEXTURE_2D, i_assets->fonts.texGLIDs[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_assets->fonts.texSizes[i].x, i_assets->fonts.texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxData);
            }
        }

        // Clear the scratch space of the pixel data; it is no longer needed.
        reset_mem_arena(scratchSpace);
        pxData = nullptr;

        // Load sounds.
        fread(&i_assets->sounds.cnt, sizeof(i_assets->sounds.cnt), 1, fs);

        if (i_assets->sounds.cnt > 0) {
            alGenBuffers(i_assets->sounds.cnt, i_assets->sounds.bufALIDs);

            for (int i = 0; i < i_assets->sounds.cnt; ++i) {
                AudioInfo audioInfo;
                fread(&audioInfo, sizeof(audioInfo), 1, fs);

                const int sampleCnt = audioInfo.sampleCntPerChannel * audioInfo.channelCnt;
                const auto samples = push_to_mem_arena<AudioSample>(scratchSpace, sampleCnt);
                fread(samples, sizeof(*samples), sampleCnt, fs);

                const ALenum format = audioInfo.channelCnt == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
                alBufferData(i_assets->sounds.bufALIDs[i], format, samples, sizeof(*samples) * sampleCnt, audioInfo.sampleRate);

                reset_mem_arena(scratchSpace);
            }
        }

        // Load music.
        fread(&i_assets->music.cnt, sizeof(i_assets->music.cnt), 1, fs);

        for (int i = 0; i < i_assets->music.cnt; ++i) {
            fread(&i_assets->music.infos[i], sizeof(i_assets->music.infos[i]), 1, fs);

            i_assets->music.sampleDataFilePositions[i] = ftell(fs);

            const int sampleCnt = i_assets->music.infos[i].sampleCntPerChannel * i_assets->music.infos[i].channelCnt;
            fseek(fs, sizeof(AudioSample) * sampleCnt, SEEK_CUR);
        }

        // Clean up.
        clean_mem_arena(scratchSpace);
        fclose(fs);

        return true;
    }

    void unload_assets() {
        if (!i_assets) {
            return;
        }

        if (i_assets->sounds.cnt > 0) {
            alDeleteBuffers(i_assets->sounds.cnt, i_assets->sounds.bufALIDs);
        }

        if (i_assets->fonts.cnt > 0) {
            glDeleteTextures(i_assets->fonts.cnt, i_assets->fonts.texGLIDs);
        }

        if (i_assets->textures.cnt > 0) {
            glDeleteTextures(i_assets->textures.cnt, i_assets->textures.glIDs);
        }

        free(i_assets);
        i_assets = nullptr;
    }

    const Assets& get_assets() {
        return *i_assets;
    }
}
