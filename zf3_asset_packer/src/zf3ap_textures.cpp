#include "zf3ap.h"

bool pack_textures(FILE* const outputFS, const cJSON* const instrsCJ, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, char* const errorMsgBuf) {
    // Get the textures array from the packing instructions JSON file.
    const cJSON* const cjTextures = get_cj_assets_array(instrsCJ, "textures");

    if (!cjTextures) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to get the textures array from the packing instructions JSON file!");
        return false;
    }

    // Get, check, and write the number of textures to pack.
    const int texCnt = cJSON_GetArraySize(cjTextures);

    if (texCnt > zf3::gk_texLimit) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Texture count exceeds the limit of %d!", zf3::gk_texLimit);
        return false;
    }

    fwrite(&texCnt, sizeof(texCnt), 1, outputFS);

    // Pack each texture.
    const cJSON* cjTexRelFilePath = nullptr;

    cJSON_ArrayForEach(cjTexRelFilePath, cjTextures) {
        // Get the relative file path of the texture.
        if (!cJSON_IsString(cjTexRelFilePath)) {
            snprintf(errorMsgBuf, gk_errorMsgBufSize, "Invalid texture entry in packing instructions JSON file!");
            return false;
        }

        if (!complete_asset_file_path(srcAssetFilePathBuf, errorMsgBuf, srcAssetFilePathStartLen, cjTexRelFilePath->valuestring)) {
            return false;
        }

        // Load and write the size and pixel data of the texture.
        zf3::Vec2DInt texSize;
        stbi_uc* const texPxData = stbi_load(srcAssetFilePathBuf, &texSize.x, &texSize.y, nullptr, zf3::gk_texChannelCnt);

        if (!texPxData) {
            snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to load pixel data for texture with relative file path \"%s\"!", cjTexRelFilePath->valuestring);
            return false;
        }

        fwrite(&texSize, sizeof(texSize), 1, outputFS);
        fwrite(texPxData, sizeof(*texPxData), texSize.x * texSize.y * zf3::gk_texChannelCnt, outputFS);

        zf3::log("Packed texture with file path \"%s\".", srcAssetFilePathBuf);

        stbi_image_free(texPxData);
    }

    return true;
}
