#include "zf3ap.h"

#include <sndfile.h>

bool pack_sounds(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen) {
    const cJSON* const cjSnds = getCJAssetsArrayAndWriteAssetCnt(instrsCJ, outputFS, "sounds");

    if (!cjSnds) {
        return true;
    }

    const int sndCnt = cJSON_GetArraySize(cjSnds);

    if (sndCnt > zf3::gk_soundLimit) {
        zf3::log_error("Sound count exceeds the limit of %d!", zf3::gk_soundLimit);
        return false;
    }

    const auto samples = static_cast<zf3::AudioSample*>(malloc(sizeof(zf3::AudioSample) * zf3::gk_soundSampleLimit));

    if (!samples) {
        zf3::log_error("Failed to allocate memory for sound samples!");
        return false;
    }

    bool success = true;

    const cJSON* cjSndRelFilePath = nullptr;

    cJSON_ArrayForEach(cjSndRelFilePath, cjSnds) {
        if (!cJSON_IsString(cjSndRelFilePath)) {
            zf3::log_error("Invalid sound entry in packing instructions JSON file!");
            success = false;
            break;
        }

        if (!complete_asset_file_path(srcAssetFilePathBuf, srcAssetFilePathStartLen, cjSndRelFilePath->valuestring)) {
            success = false;
            break;
        }

        // Open the sound file.
        SF_INFO sfInfo;
        SNDFILE* sf = sf_open(srcAssetFilePathBuf, SFM_READ, &sfInfo);

        if (!sf) {
            zf3::log_error("Failed to open sound file with path \"%s\".", srcAssetFilePathBuf);
            success = false;
            break;
        }

        // Get and check sound file information.
        const zf3::AudioInfo info = {
            .channelCnt = sfInfo.channels,
            .sampleCntPerChannel = sfInfo.frames,
            .sampleRate = sfInfo.samplerate
        };

        if (info.channelCnt != 1 && info.channelCnt != 2) {
            zf3::log_error("Sound file with path \"%s\" has an unsupported channel count of %d.", srcAssetFilePathBuf, info.channelCnt);
            sf_close(sf);
            success = false;
            break;
        }

        const int sampleCnt = info.sampleCntPerChannel * info.channelCnt;

        if (sampleCnt > zf3::gk_soundSampleLimit) {
            zf3::log_error("Sound file with path \"%s\" exceeds the sample limit of %d.", srcAssetFilePathBuf, sampleCnt, zf3::gk_soundSampleLimit);
            sf_close(sf);
            success = false;
            break;
        }

        // Write the information.
        fwrite(&info, sizeof(info), 1, outputFS);

        // Read samples into buffer.
        const sf_count_t samplesRead = sf_read_float(sf, samples, sampleCnt);

        if (samplesRead != sampleCnt) {
            zf3::log_error("Failed to read all samples from sound file with path \"%s\".", srcAssetFilePathBuf);
            sf_close(sf);
            success = false;
            break;
        }

        // Write the samples.
        if (info.channelCnt == 1) {
            // Mono
            fwrite(samples, sizeof(*samples), info.sampleCntPerChannel, outputFS);
        } else {
            // Stereo
            for (int i = 0; i < info.sampleCntPerChannel; ++i) {
                const zf3::AudioSample frame[2] = {
                    samples[i * 2],      // Left Channel
                    samples[(i * 2) + 1] // Right Channel
                };

                fwrite(frame, sizeof(frame), 1, outputFS);
            }
        }

        sf_close(sf);

        zf3::log("Packed sound with file path \"%s\".", srcAssetFilePathBuf);
    }

    free(samples);

    return success;
}
