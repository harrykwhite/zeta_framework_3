#include <zf3_assets.h>

namespace zf3 {
    static Assets* i_assets;

    static bool load_textures(Textures* const textures, FILE* const fs) {
        const int pxDataBufSize = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;
        const auto pxDataBuf = alloc<unsigned char>(pxDataBufSize); // For temporarily storing the pixel data of each texture.

        if (!pxDataBuf) {
            log_error("Failed to allocate memory for a texture pixel data buffer!");
            return false;
        }

        fread(&textures->cnt, sizeof(textures->cnt), 1, fs);

        if (textures->cnt > 0) {
            glGenTextures(textures->cnt, textures->glIDs);

            for (int i = 0; i < textures->cnt; ++i) {
                fread(&textures->sizes[i], sizeof(textures->sizes[i]), 1, fs);

                fread(pxDataBuf, gk_texChannelCnt * textures->sizes[i].x * textures->sizes[i].y, 1, fs);

                glBindTexture(GL_TEXTURE_2D, textures->glIDs[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures->sizes[i].x, textures->sizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
            }
        }

        free(pxDataBuf);

        return true;
    }

    static bool load_fonts(Fonts* const fonts, FILE* const fs) {
        const int pxDataBufSize = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;
        const auto pxDataBuf = alloc<unsigned char>(pxDataBufSize); // For temporarily storing the pixel data of each font texture.

        if (!pxDataBuf) {
            log_error("Failed to allocate memory for a font texture pixel data buffer!");
            return false;
        }

        fread(&fonts->cnt, sizeof(fonts->cnt), 1, fs);

        if (fonts->cnt > 0) {
            glGenTextures(fonts->cnt, fonts->texGLIDs);

            for (int i = 0; i < fonts->cnt; ++i) {
                fread(&fonts->arrangementInfos[i], sizeof(fonts->arrangementInfos[i]), 1, fs);
                fread(&fonts->texSizes[i], sizeof(fonts->texSizes[i]), 1, fs);
                fread(pxDataBuf, gk_texPxDataSizeLimit, 1, fs);

                glBindTexture(GL_TEXTURE_2D, fonts->texGLIDs[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fonts->texSizes[i].x, fonts->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
            }
        }

        free(pxDataBuf);

        return true;
    }

    static bool load_sounds(Sounds* const snds, FILE* const fs) {
        const auto samples = alloc_zeroed<AudioSample>(gk_audioSamplesPerChunk); // TODO: Set a proper limit on sample count for sounds.

        if (!samples) {
            log_error("Failed to allocate memory for a sound sample buffer!");
            return false;
        }

        fread(&snds->cnt, sizeof(snds->cnt), 1, fs);

        if (snds->cnt > 0) {
            alGenBuffers(snds->cnt, snds->bufALIDs);

            for (int i = 0; i < snds->cnt; ++i) {
                AudioInfo audioInfo;
                fread(&audioInfo, sizeof(audioInfo), 1, fs);

                const int sampleCnt = audioInfo.sampleCntPerChannel * audioInfo.channelCnt;
                fread(samples, sizeof(*samples), sampleCnt, fs);

                const ALenum format = audioInfo.channelCnt == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
                alBufferData(snds->bufALIDs[i], format, samples, sizeof(*samples) * sampleCnt, audioInfo.sampleRate);
            }
        }

        free(samples);

        return true;
    }

    static bool load_music(Music* const music, FILE* const fs) {
        fread(&music->cnt, sizeof(music->cnt), 1, fs);

        for (int i = 0; i < music->cnt; ++i) {
            fread(&music->infos[i], sizeof(music->infos[i]), 1, fs);

            music->sampleDataFilePositions[i] = ftell(fs);

            const int sampleCnt = music->infos[i].sampleCntPerChannel * music->infos[i].channelCnt;
            fseek(fs, sizeof(AudioSample) * sampleCnt, SEEK_CUR);
        }

        return true;
    }

    bool init_assets() {
        assert(!i_assets);

        i_assets = alloc_zeroed<Assets>();

        if (!i_assets) {
            log_error("Failed to allocate memory for assets!");
            return false;
        }

        FILE* const fs = fopen(gk_assetsFileName, "rb");

        if (!fs) {
            log_error("Failed to open \"%s\"!", gk_assetsFileName);
            return false;
        }

        const bool success = load_textures(&i_assets->textures, fs)
            && load_fonts(&i_assets->fonts, fs)
            && load_sounds(&i_assets->sounds, fs)
            && load_music(&i_assets->music, fs);

        fclose(fs);

        return success;
    }

    void clean_assets() {
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
