#include "zf3ap.h"

bool pack_textures(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen) {
    const cJSON* const cjTextures = getCJAssetsArrayAndWriteAssetCnt(instrsCJ, outputFS, "textures");

    if (!cjTextures) {
        return true;
    }

    const cJSON* cjTexRelFilePath = NULL;

    cJSON_ArrayForEach(cjTexRelFilePath, cjTextures) {
        if (!cJSON_IsString(cjTexRelFilePath)) {
            zf3::log_error("Invalid texture entry in packing instructions JSON file!");
            return false;
        }

        if (!complete_asset_file_path(srcAssetFilePathBuf, srcAssetFilePathStartLen, cjTexRelFilePath->valuestring)) {
            return false;
        }

        zf3::Vec2DInt texSize;
        stbi_uc* const texPxData = stbi_load(srcAssetFilePathBuf, &texSize.x, &texSize.y, NULL, zf3::gk_texChannelCnt);

        if (!texPxData) {
            zf3::log_error("Failed to load pixel data for texture with relative file path \"%s\"!", cjTexRelFilePath->valuestring);
            return false;
        }

        fwrite(&texSize, sizeof(texSize), 1, outputFS);
        fwrite(texPxData, sizeof(*texPxData), texSize.x * texSize.y * zf3::gk_texChannelCnt, outputFS);

        zf3::log("Packed texture with file path \"%s\".", srcAssetFilePathBuf);

        stbi_image_free(texPxData);
    }

    return true;
}
