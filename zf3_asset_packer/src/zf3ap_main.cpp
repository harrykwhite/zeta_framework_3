#include "zf3ap.h"

const char* const ik_packingInstrsFileName = "packing_instrs.json";
constexpr int ik_outputFilePathBufSize = 256;

typedef struct {
    char* instrsFileChars;
    cJSON* instrsCJ;
    FILE* outputFileStream;
} AssetPacker;

static char* get_packing_instrs_file_chars(char* const srcAssetFilePathBuf, const int srcAssetFilePathBufStartLen) {
    // Append the packing instructions file name to the source asset file path buffer.
    strncpy(srcAssetFilePathBuf + srcAssetFilePathBufStartLen, ik_packingInstrsFileName, gk_srcAssetFilePathBufSize - srcAssetFilePathBufStartLen);

    if (srcAssetFilePathBuf[gk_srcAssetFilePathBufSize - 1]) {
        zf3::log_error("The packing instructions file name is too long!");
        return NULL;
    }

    return zf3::get_file_contents(srcAssetFilePathBuf);
}

static bool run_asset_packer(AssetPacker* const packer, const char* const srcDir, const char* const outputDir) {
    memset(packer, 0, sizeof(*packer));

    // Determine the output file path using the directory.
    char outputFilePath[ik_outputFilePathBufSize];
    const int outputFilePathLen = snprintf(outputFilePath, ik_outputFilePathBufSize, "%s\\%s", outputDir, zf3::gk_assetsFileName);

    if (outputFilePathLen >= ik_outputFilePathBufSize) {
        zf3::log_error("The provided output directory \"%s\" is too long!", outputDir);
        return false;
    }

    // Initialise the source asset file path buffer with the source directory.
    char srcAssetFilePathBuf[gk_srcAssetFilePathBufSize] = {0};
    const int srcAssetFilePathStartLen = snprintf(srcAssetFilePathBuf, gk_srcAssetFilePathBufSize, "%s\\", srcDir);

    if (srcAssetFilePathStartLen >= gk_srcAssetFilePathBufSize) {
        zf3::log_error("The provided source directory of \"%s\" is too long!", srcDir);
        return false;
    }

    // Get the contents of the packing instructions JSON file.
    packer->instrsFileChars = get_packing_instrs_file_chars(srcAssetFilePathBuf, srcAssetFilePathStartLen);

    if (!packer->instrsFileChars) {
        return false;
    }

    // Parse the packing instructions file contents.
    packer->instrsCJ = cJSON_Parse(packer->instrsFileChars);

    if (!packer->instrsCJ) {
        zf3::log_error("cJSON failed to parse the packing instructions file contents!");
        return false;
    }

    // Create or replace the output file.
    packer->outputFileStream = fopen(outputFilePath, "wb");

    if (!packer->outputFileStream) {
        zf3::log_error("Failed to open the output file \"%s\"!", outputFilePath);
        return false;
    }

    // Pack assets.
    if (!pack_textures(packer->instrsCJ, packer->outputFileStream, srcAssetFilePathBuf, srcAssetFilePathStartLen)) {
        return false;
    }

    return true;
}

static void asset_packer_cleanup(AssetPacker* const packer) {
    cJSON_Delete(packer->instrsCJ);

    if (packer->outputFileStream) {
        fclose(packer->outputFileStream);
    }

    free(packer->instrsFileChars);
}

cJSON* getCJSONAssetsArrayAndWriteAssetCnt(cJSON* const instrsCJObj, FILE* const outputFileStream, const char* const arrayName) {
    cJSON* const assetsCJ = cJSON_GetObjectItemCaseSensitive(instrsCJObj, arrayName);
    const bool assetsCJFound = cJSON_IsArray(assetsCJ);

    const int assetCnt = assetsCJFound ? cJSON_GetArraySize(assetsCJ) : 0;
    fwrite(&assetCnt, sizeof(assetCnt), 1, outputFileStream);

    if (!assetsCJFound) {
        return NULL;
    }

    return assetsCJ;
}

int main(const int argCnt, const char* const* args) {
    // Get the source directory and output directory if provided.
    if (argCnt != 3) {
        zf3::log_error("Invalid number of command-line arguments! Expected a source directory and an output directory to be provided.");
        return EXIT_FAILURE;
    }

    const char* const srcDir = args[1]; // The directory containing the assets to pack.
    const char* const outputDir = args[2]; // The directory to output the packed assets file to.

    // Run the asset packer.
    AssetPacker packer;
    const bool packingSuccessful = run_asset_packer(&packer, srcDir, outputDir);
    asset_packer_cleanup(&packer);

    return packingSuccessful ? EXIT_SUCCESS : EXIT_FAILURE;
}