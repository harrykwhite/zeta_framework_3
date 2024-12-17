#include "zf3ap.h"

#include <sndfile.h>

static bool load_audio_file_and_write_data(const char* const filePath, FILE* const infoFS, FILE* const samplesFS) {
    // Open the audio file.
    SF_INFO sfInfo;
    SNDFILE* sf = sf_open(filePath, SFM_READ, &sfInfo);

    if (!sf) {
        fprintf(stderr, "Failed to open audio file \"%s\": %s\n", filePath, sf_strerror(NULL));
        return false;
    }

    // Write audio file information.
    const zf3::AudioInfo info = {
        .channelCnt = sfInfo.channels,
        .sampleCntPerChannel = sfInfo.frames,
        .sampleRate = sfInfo.samplerate
    };

    fwrite(&info, sizeof(info), 1, infoFS);

    // Allocate buffer for samples.
    const int sampleCnt = info.sampleCntPerChannel * info.channelCnt;
    const auto samples = static_cast<zf3::AudioSample*>(malloc(sizeof(zf3::AudioSample) * sampleCnt));

    if (!samples) {
        zf3::log_error("Failed to allocate memory for audio samples.");
        sf_close(sf);
        return false;
    }

    // Read samples into buffer.
    const sf_count_t samplesRead = sf_read_float(sf, samples, sampleCnt);

    if (samplesRead != sampleCnt) {
        zf3::log_error("Failed to read all samples from audio file with path \"%s\".", filePath);
    }

    // Write the samples.
    if (info.channelCnt == 1) {
        fwrite(samples, sizeof(*samples), info.sampleCntPerChannel, samplesFS);
    } else if (info.channelCnt == 2) {
        for (int i = 0; i < info.sampleCntPerChannel; ++i) {
            const zf3::AudioSample frame[2] = {
                samples[i * 2],      // Left Channel
                samples[(i * 2) + 1] // Right Channel
            };

            fwrite(frame, sizeof(frame), 1, samplesFS);
        }
    } else {
        zf3::log_error("Audio file with path \"%s\" has an unsupported channel count of %d.", filePath, info.channelCnt);
        free(samples);
        sf_close(sf);
        return false;
    }

    // Clean up.
    free(samples);
    sf_close(sf);

    return true;
}

bool pack_sounds(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen) {
    const cJSON* const cjSnds = getCJAssetsArrayAndWriteAssetCnt(instrsCJ, outputFS, "sounds");

    if (!cjSnds) {
        return true;
    }

    const int sndCnt = cJSON_GetArraySize(cjSnds);

    if (sndCnt > zf3::gk_texLimit) {
        zf3::log_error("Sound count exceeds the limit of %d!", zf3::gk_soundLimit);
        return false;
    }

    const cJSON* cjSndRelFilePath = nullptr;

    cJSON_ArrayForEach(cjSndRelFilePath, cjSnds) {
        if (!cJSON_IsString(cjSndRelFilePath)) {
            zf3::log_error("Invalid sound entry in packing instructions JSON file!");
            return false;
        }

        if (!complete_asset_file_path(srcAssetFilePathBuf, srcAssetFilePathStartLen, cjSndRelFilePath->valuestring)) {
            return false;
        }

        zf3::log("Packed sound with file path \"%s\".", srcAssetFilePathBuf);
    }

    return true;
}
