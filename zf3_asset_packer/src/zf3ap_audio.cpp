#include "zf3ap.h"

#include <sndfile.h>

static bool load_and_write_audio_data_from_file(FILE* const outputFS, zf3::AudioSample* const samples, char* const errMsgBuf, const char* const filePath) {
    // Open the audio file.
    SF_INFO sfInfo;
    SNDFILE* sf = sf_open(filePath, SFM_READ, &sfInfo);

    if (!sf) {
        snprintf(errMsgBuf, gk_errorMsgBufSize, "Failed to open audio file with path \"%s\".", filePath);
        return false;
    }

    // Get and check audio file information.
    const zf3::AudioInfo info = {
        .channelCnt = sfInfo.channels,
        .sampleCntPerChannel = sfInfo.frames,
        .sampleRate = sfInfo.samplerate
    };

    if (info.channelCnt != 1 && info.channelCnt != 2) {
        snprintf(errMsgBuf, gk_errorMsgBufSize, "Audio file with path \"%s\" has an unsupported channel count of %d.", filePath, info.channelCnt);
        sf_close(sf);
        return false;
    }

    // Write the information.
    fwrite(&info, sizeof(info), 1, outputFS);

    // Read sample chunks into buffer and write them.
    sf_count_t samplesRead;

    while ((samplesRead = sf_read_float(sf, samples, zf3::gk_audioSamplesPerChunk)) > 0) {
        fwrite(samples, sizeof(*samples), samplesRead, outputFS);

        if (samplesRead < zf3::gk_audioSamplesPerChunk) {
            break;
        }
    }

    sf_close(sf);

    return true;
}

static bool pack_sounds(FILE* const outputFS, char* const srcAssetFilePathBuf, zf3::AudioSample* const samples, char* const errorMsgBuf, const cJSON* const instrsCJ, const int srcAssetFilePathStartLen) {
    // Get the sounds array from the packing instructions JSON file.
    const cJSON* const cjSnds = get_cj_assets_array(instrsCJ, "sounds");

    if (!cjSnds) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to get the sounds array from the packing instructions JSON file!");
        return false;
    }

    // Get, check, and write the number of sounds to pack.
    const int sndCnt = cJSON_GetArraySize(cjSnds);

    if (sndCnt > zf3::gk_soundLimit) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Sound count exceeds the limit of %d!", zf3::gk_soundLimit);
        return false;
    }

    fwrite(&sndCnt, sizeof(sndCnt), 1, outputFS);

    // Pack each sound.
    const cJSON* cjSndRelFilePath = nullptr;

    cJSON_ArrayForEach(cjSndRelFilePath, cjSnds) {
        if (!cJSON_IsString(cjSndRelFilePath)) {
            zf3::log_error("Invalid sound entry in packing instructions JSON file!");
            return false;
        }

        if (!complete_asset_file_path(srcAssetFilePathBuf, errorMsgBuf, srcAssetFilePathStartLen, cjSndRelFilePath->valuestring)) {
            return false;
        }

        if (!load_and_write_audio_data_from_file(outputFS, samples, errorMsgBuf, srcAssetFilePathBuf)) {
            return false;
        }

        zf3::log("Packed sound with file path \"%s\".", srcAssetFilePathBuf);
    }

    return true;
}

// TODO: Rid this world of such horrid duplicity!
static bool pack_music(FILE* const outputFS, char* const srcAssetFilePathBuf, zf3::AudioSample* const samples, char* const errorMsgBuf, const cJSON* const instrsCJ, const int srcAssetFilePathStartLen) {
    // Get the music array from the packing instructions JSON file.
    const cJSON* const cjMusic = get_cj_assets_array(instrsCJ, "music");

    if (!cjMusic) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to get the music array from the packing instructions JSON file!");
        return false;
    }

    // Get, check, and write the number of music tracks to pack.
    const int musicCnt = cJSON_GetArraySize(cjMusic);

    if (musicCnt > zf3::gk_musicLimit) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Music count exceeds the limit of %d!", zf3::gk_musicLimit);
        return false;
    }

    fwrite(&musicCnt, sizeof(musicCnt), 1, outputFS);

    // Pack each music track.
    const cJSON* cjMusicRelFilePath = nullptr;

    cJSON_ArrayForEach(cjMusicRelFilePath, cjMusic) {
        if (!cJSON_IsString(cjMusicRelFilePath)) {
            zf3::log_error("Invalid music entry in packing instructions JSON file!");
            return false;
        }

        if (!complete_asset_file_path(srcAssetFilePathBuf, errorMsgBuf, srcAssetFilePathStartLen, cjMusicRelFilePath->valuestring)) {
            return false;
        }

        if (!load_and_write_audio_data_from_file(outputFS, samples, errorMsgBuf, srcAssetFilePathBuf)) {
            return false;
        }

        zf3::log("Packed music with file path \"%s\".", srcAssetFilePathBuf);
    }

    return true;
}

bool pack_audio(FILE* const outputFS, const cJSON* const instrsCJ, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, char* const errorMsgBuf) {
    const auto samples = static_cast<zf3::AudioSample*>(malloc(sizeof(zf3::AudioSample) * zf3::gk_audioSamplesPerChunk));

    if (!samples) {
        return false;
    }

    const bool success = pack_sounds(outputFS, srcAssetFilePathBuf, samples, errorMsgBuf, instrsCJ, srcAssetFilePathStartLen)
        && pack_music(outputFS, srcAssetFilePathBuf, samples, errorMsgBuf, instrsCJ, srcAssetFilePathStartLen);

    free(samples);

    return success;
}
